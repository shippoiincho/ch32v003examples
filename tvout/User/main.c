/*

 NTSC Video Output test
 32*24 chars = 256*192 pixels non-interlace output
 You must use HSE 48MHz clock.

 TIM1CH4:  (PC4) Sync signal
 TIM1CH3:  (NC)  Video out timing interrupt
 SPI1MOSI: (PC6) Video signal

 PC4 -- R1 --+
 PC6 -- R2 --+---> Video

 R1: 470 + 220 ohm
 R2: 180 + 100 ohm

 */

#include "debug.h"
#include "msxfont.h"
#include <stdlib.h>
#include <string.h>

/* Global define */

#define NTSC_COUNT 3050 // = 63.56 us / 48MHz
#define NTSC_HSYNC 225  // =  4.7  us / 48MHz
#define NTSC_VSYNC 2825 // = NTSC_COUNT - NTSC_HSYNC
#define NTSC_SCAN_DELAY 100 // Delay for video signal generation
#define NTSC_SCAN_START 40 // Display start line

#define NTSC_RESOLUTION 1

#if NTSC_RESOLUTION==1

#define NTSC_X_PIXELS 256
#define NTSC_Y_PIXELS 192

#define NTSC_PRESCALER SPI_BaudRatePrescaler_8

#elif NTSC_RESOLUTION==2

#define NTSC_X_PIXELS 472
#define NTSC_Y_PIXELS 200

#define NTSC_PRESCALER SPI_BaudRatePrescaler_4

#endif

#define NTSC_X_CHARS (NTSC_X_PIXELS/8)
#define NTSC_Y_CHARS (NTSC_Y_PIXELS/8)

/* Global Variable */

volatile uint16_t ntsc_line;
volatile uint8_t ntsc_blank = 0;

uint8_t *vram;
uint8_t *scandata[2];

uint8_t cursor_x, cursor_y = 0;

void video_init() {

    TIM_OCInitTypeDef TIM_OCInitStructure = { 0 };
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = { 0 };
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };
    SPI_InitTypeDef SPI_InitStructure = { 0 };
    NVIC_InitTypeDef NVIC_InitStructure = { 0 };

    RCC_APB2PeriphClockCmd(
    RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1 | RCC_APB2Periph_TIM1, ENABLE);

    // PC4:Sync

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure);

    // PC6: Video (SPI1)

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure);

    // Initalize TIM1

    TIM_TimeBaseInitStructure.TIM_Period = NTSC_COUNT;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0;                // Presclaer = 0
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = NTSC_HSYNC;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC4Init( TIM1, &TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = NTSC_HSYNC * 2 - NTSC_SCAN_DELAY; // 9.4usec - delay
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC3Init( TIM1, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_OC3PreloadConfig( TIM1, TIM_OCPreload_Disable);
    TIM_OC4PreloadConfig( TIM1, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig( TIM1, ENABLE);
    TIM_Cmd( TIM1, ENABLE);

    // Initialize SPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = NTSC_PRESCALER; // 6MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init( SPI1, &SPI_InitStructure);

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
    SPI_Cmd(SPI1, ENABLE);

    // NVIC

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM1, TIM_IT_CC3, ENABLE);

    // Init VRAM

    vram = malloc(NTSC_X_CHARS * NTSC_Y_CHARS);

    scandata[0] = malloc(NTSC_X_CHARS + 1);
    scandata[1] = malloc(NTSC_X_CHARS + 1);

    scandata[0][NTSC_X_CHARS] = 0;
    scandata[1][NTSC_X_CHARS] = 0;

    //

}

static inline void video_cls() {
    memset(vram, 0, (NTSC_X_CHARS * NTSC_Y_CHARS));
}

static inline void video_scroll() {

    memmove(vram, vram + NTSC_X_CHARS, NTSC_X_CHARS * (NTSC_Y_CHARS - 1));

    memset(vram + NTSC_X_CHARS * (NTSC_Y_CHARS - 1), 0, NTSC_X_CHARS);

}

static inline void video_print(uint8_t *string) {

    int len;

    len = strlen(string);

    for (int i = 0; i < len; i++) {
        vram[cursor_x + cursor_y * NTSC_X_CHARS] = string[i];
        cursor_x++;
        if (cursor_x >= NTSC_X_CHARS) {
            cursor_x = 0;
            cursor_y++;
            if (cursor_y >= NTSC_Y_CHARS) {
                video_scroll();
                cursor_y = NTSC_Y_CHARS - 1;
            }
        }
    }

}

/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr,
        u16 bufsize) {
    DMA_InitTypeDef DMA_InitStructure = { 0 };

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);

}

void TIM1_CC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM1_CC_IRQHandler(void) {

    TIM_ClearFlag(TIM1, TIM_FLAG_CC3);
    uint8_t char_x, char_y, slice_y;

    ntsc_line++;

    // VSYNC/HSYNC slection for next scanline

    if ((ntsc_line == 3) || (ntsc_line == 4) || (ntsc_line == 5)) { // VSYNC : ntsc_line : 4-6
        TIM_SetCompare4(TIM1, NTSC_VSYNC);
        //    TIM1->CH4CVR = NTSC_VSYNC;
    } else {
        TIM_SetCompare4(TIM1, NTSC_HSYNC);
        //    TIM1->CH4CVR = NTSC_HSYNC;
    }

    // Video Out

    if ((ntsc_line >= NTSC_SCAN_START)
            && (ntsc_line < (NTSC_SCAN_START + NTSC_Y_PIXELS))) { // video out
        DMA_Tx_Init(DMA1_Channel3, (u32) (&SPI1->DATAR + 1),
                (u32) scandata[ntsc_line % 2], NTSC_X_CHARS + 1);
        DMA_Cmd(DMA1_Channel3, ENABLE);
    }

    // Redner fonts for next scanline

    if ((ntsc_line >= NTSC_SCAN_START - 1)
            && (ntsc_line < (NTSC_SCAN_START + NTSC_Y_PIXELS - 1))) {

        char_y = (ntsc_line + 1 - NTSC_SCAN_START) / 8;
        slice_y = (ntsc_line + 1 - NTSC_SCAN_START) % 8;
        for (char_x = 0; char_x < NTSC_X_CHARS; char_x++) {
            scandata[(ntsc_line + 1) % 2][char_x] = msxfont[vram[char_x
                    + char_y * NTSC_X_CHARS] * 8 + slice_y];
        }
    }

    if (ntsc_line > 262) {
        ntsc_line = 0;
    }

}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void) {

    uint8_t ii = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    video_init();

    video_cls();

    Delay_Init();

    //  printf("SystemClk:%d\r\n", SystemCoreClock);

    while(1)
    {

        for (int y = 0; y < NTSC_Y_CHARS; y++) {
            for (int x = 0; x < NTSC_X_CHARS; x++) {
                vram[x + y * NTSC_X_CHARS] = ii++;
                Delay_Ms(10);
            }
        }

        Delay_Ms(1000);

        for (int y = 0; y < NTSC_Y_CHARS; y++) {
            video_scroll();
            Delay_Ms(100);
        }

        video_cls();

        cursor_x=0;
        cursor_y=0;

        for(int i=0;i<500;i++) {

            video_print("CH32V003 ");
            Delay_Ms(30);

        }

        Delay_Ms(1000);

        video_cls();

        cursor_x=0;
        cursor_y=0;
        video_print("MSX BASIC version 1.0");

        cursor_x=0;
        cursor_y=1;
        video_print("Copyright 1983 by Microsoft");

        cursor_x=0;
        cursor_y=2;
        video_print("23430 Bytes free");

        cursor_x=0;
        cursor_y=3;
        video_print("Ok");

        cursor_x=0;
        cursor_y=NTSC_Y_CHARS-1;
        video_print("color auto  goto  list  run");

        Delay_Ms(5000);

        video_cls();

    }
}
