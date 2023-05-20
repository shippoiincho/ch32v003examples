/*

 Video Game Demo

 You must use HSE 48MHz clock for video stability.

 Use:

 TIM1CH4:  (PC4) Sync signal
 TIM1CH3:  (NC)  Video out timing interrupt
 SPI1MOSI: (PC6) Video signal
 TIM2CH3:  (PC0) Audio signal

 Connection:

 PC4 -- R1 --+
 PC6 -- R2 --+---> Video

 R1: 470 + 220 ohm
 R2: 180 + 100 ohm

 PC0 ------------> Audio

 PD2 -- SNES pad LATCH
 PD3 -- SNES pad CLK
 PD4 -- SNES pad DATA (pull down with 10k OHM register)

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

#define NTSC_X_PIXELS 400
#define NTSC_Y_PIXELS 192

#define NTSC_PRESCALER SPI_BaudRatePrescaler_4

#endif

#define NTSC_X_CHARS (NTSC_X_PIXELS/8)
#define NTSC_Y_CHARS (NTSC_Y_PIXELS/8)

#define NUM_SPRITES 16

#define TONE_DEFAULT_DURATION 20

/* Global Variable */

volatile uint16_t ntsc_line;
volatile uint8_t ntsc_blank = 0;
volatile uint16_t snes_pad_status = 0;

uint8_t *vram;
uint8_t *scandata[2];

struct {
    uint16_t x;
    uint16_t y;
    uint8_t enable;
    uint8_t *pattern;
}typedef sprite;

sprite sprite_buffer[NUM_SPRITES];

uint8_t cursor_x, cursor_y = 0;

const int16_t title_sound[] = { 523, 587, 659, 698, 784, 880, 988, 1047, 0, -1 };

const uint8_t player_pattern[] = { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55,
        0xaa };

// Snes pad

void snes_pad_init() {

    GPIO_InitTypeDef GPIO_InitStructure = { 0 };

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);

    // PD2:

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOD, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_RESET);
    GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_SET);

}

void snes_pad_read() {
    uint16_t pad_status = 0;

    GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_SET);
    // delay 12us
    Delay_Us(12);
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_RESET);

    for (int i = 0; i < 16; i++) {

        pad_status = pad_status << 1;

        GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET);
        pad_status += (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4) & 1);
        //delay 6us
        Delay_Us(6);
        GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_SET);
        //delay 6us
        Delay_Us(6);
    }

    snes_pad_status = pad_status;
}

// Video out

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

void video_print(uint8_t *string) {

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

void video_wait_vsync() {

    while(ntsc_blank==1);
    while(ntsc_blank==0);

    snes_pad_read();

}

void TIM2_PWMOut_Init(u16 arr, u16 psc, u16 ccp) {
    TIM_OCInitTypeDef TIM_OCInitStructure = { 0 };
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = { 0 };

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM2, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC3Init( TIM2, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    TIM_OC3PreloadConfig( TIM2, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig( TIM2, ENABLE);
    TIM_Cmd( TIM2, ENABLE);
}

void toneinit(void) {

    GPIO_InitTypeDef GPIO_InitStructure = { 0 };

    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure);

}

void tone(uint16_t freq) {

    uint16_t duration;

    duration = 1000000 / freq;   // in usec

    TIM2_PWMOut_Init(duration, 47, duration / 2);

}

void notone(void) {

    TIM_Cmd(TIM2, DISABLE);
}

//

void game_draw_frame() {
    memset(vram, 0x85, (NTSC_X_CHARS * NTSC_Y_CHARS));
    for (int i = 1; i < NTSC_Y_CHARS - 1; i++) {
        memset(vram + NTSC_X_CHARS * i + 1, 0, NTSC_X_CHARS - 2);
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
    uint8_t char_x, char_y, slice_x, slice_y;
    uint16_t render_line;

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
        ntsc_blank = 0;
        DMA_Tx_Init(DMA1_Channel3, (u32) (&SPI1->DATAR + 1),
                (u32) scandata[ntsc_line % 2], NTSC_X_CHARS + 1);
        DMA_Cmd(DMA1_Channel3, ENABLE);
    } else {
        ntsc_blank = 1;
    }

    // Redner fonts for next scanline

    if ((ntsc_line >= NTSC_SCAN_START - 1)
            && (ntsc_line < (NTSC_SCAN_START + NTSC_Y_PIXELS - 1))) {

        render_line = ntsc_line + 1 - NTSC_SCAN_START;

        // TEXT VRAM

        char_y = render_line / 8;
        slice_y = render_line % 8;
        for (char_x = 0; char_x < NTSC_X_CHARS; char_x++) {
            scandata[(ntsc_line + 1) % 2][char_x] = msxfont[vram[char_x
                    + char_y * NTSC_X_CHARS] * 8 + slice_y];
        }

        // Sprites

        for (int i = 0; i < NUM_SPRITES; i++) {
            if (sprite_buffer[i].enable == 1) {
                if ((render_line >= sprite_buffer[i].y)
                        && (render_line < sprite_buffer[i].y + 8)) {
                    slice_y = (render_line - sprite_buffer[i].y) % 8;
                    char_x = sprite_buffer[i].x / 8;
                    slice_x = sprite_buffer[i].x % 8;
                    if (slice_x == 0) {
                        if (char_x < NTSC_X_CHARS) {
                            scandata[(ntsc_line + 1) % 2][char_x] |=
                                    sprite_buffer[i].pattern[slice_y];
                        }
                    } else {
                        if (char_x < NTSC_X_CHARS) {
                            scandata[(ntsc_line + 1) % 2][char_x] |=
                                    sprite_buffer[i].pattern[slice_y]
                                            >> slice_x;
                        }
                        if (char_x < NTSC_X_CHARS - 1) {
                            scandata[(ntsc_line + 1) % 2][char_x + 1] |=
                                    sprite_buffer[i].pattern[slice_y]
                                            << (8 - slice_x);
                        }
                    }
                }
            }
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

    int8_t player_x, player_y = 0;
    uint16_t tone_dutation = 0;
    uint8_t tone_note = 0;
    int8_t dx[NUM_SPRITES], dy[NUM_SPRITES];

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    video_init();
    video_cls();

    Delay_Init();
    snes_pad_init();
    toneinit();

    //  printf("SystemClk:%d\r\n", SystemCoreClock);

    for (int i = 0; i < NUM_SPRITES; i++) {

        sprite_buffer[i].x = 0;
        sprite_buffer[i].y = 0;
        sprite_buffer[i].enable = 0;
        sprite_buffer[i].pattern = player_pattern;

    }

    while(1)
    {

        // Title screen

        video_wait_vsync();
        game_draw_frame();

        for(int i=0;i<NUM_SPRITES;i++) {
            sprite_buffer[i].enable=0;
        }

        tone_note=0;
        tone_dutation=TONE_DEFAULT_DURATION;
        tone(title_sound[tone_note]);

        cursor_x=4;
        cursor_y=5;

        video_print("CH32V003 Video Game Demo");

        cursor_x=10;
        cursor_y=18;

        video_print("Press START");

        while((snes_pad_status&0x1000)!=0) {   // wait Press START
            video_wait_vsync();
            if(tone_dutation==0) {
                tone_note++;
                if(title_sound[tone_note]== -1) {
                    tone_note=0;
                }
                if(title_sound[tone_note]!=0) {
                    tone(title_sound[tone_note]);
                } else {
                    notone();
                }
                tone_dutation=TONE_DEFAULT_DURATION;
            } else {
                tone_dutation--;
            }
        }

        while((snes_pad_status&0x1000)==0) {  // wait release START
            video_wait_vsync();
        }

        // Game ?

        notone();

        video_cls();
        game_draw_frame();

        player_x=1;
        player_y=1;

        for(int i=0;i<NUM_SPRITES;i++) {
            sprite_buffer[i].x=rand()%(NTSC_X_PIXELS-16);
            sprite_buffer[i].y=rand()%(NTSC_Y_PIXELS-16);
            sprite_buffer[i].enable=1;
            dx[i]=rand()%2;
            dy[i]=rand()%2;
            if(dx[i]==0) dx[i]=-1;
            if(dy[i]==0) dy[i]=-1;
        }

        while(1) {
            video_wait_vsync();
            vram[player_x+player_y*NTSC_X_CHARS]=0;
            if((snes_pad_status&0x0800)==0) {            // Up
                player_y--;
            } else if ((snes_pad_status&0x0400)==0) {    // Down
                player_y++;
            } else if ((snes_pad_status&0x0200)==0) {    // Left
                player_x--;
            } else if ((snes_pad_status&0x0100)==0) {    // Right
                player_x++;
            }
            if ((snes_pad_status&0x1000)==0) {
                break;
            }
            if(player_x<=0) player_x=1;
            if(player_x>=NTSC_X_CHARS-1) player_x=NTSC_X_CHARS-2;
            if(player_y<=0) player_y=1;
            if(player_y>=NTSC_Y_CHARS-1) player_y=NTSC_Y_CHARS-2;
            vram[player_x+player_y*NTSC_X_CHARS]=0x40;

            notone();

            for(int i=0;i<NUM_SPRITES;i++) {
                sprite_buffer[i].x+=dx[i];
                sprite_buffer[i].y+=dy[i];
                if(sprite_buffer[i].x>NTSC_X_PIXELS-16) {
                    sprite_buffer[i].x--;
                    dx[i]=-1;
                    tone(880);
                } else if(sprite_buffer[i].x<8) {
                    sprite_buffer[i].x++;
                    dx[i]=1;
                    tone(880);
                }
                if(sprite_buffer[i].y>NTSC_Y_PIXELS-16) {
                    sprite_buffer[i].y--;
                    dy[i]=-1;
                    tone(880);
                } else if(sprite_buffer[i].y<8) {
                    sprite_buffer[i].y++;
                    dy[i]=1;
                    tone(880);
                }
            }

        }

        while((snes_pad_status&0x1000)==0) {  // wait release START
            video_wait_vsync();
        }

    }
}
