/*

 SNES pad to PC-98DO+11 converter

 to PC-98Do Mouse connector (ATARI type joystick)
 PC0 -- UP
 PC1 -- DOWN
 PC2 -- LEFT
 PC3 -- RIGHT
 PC4 -- Trigger A
 PC5 -- Trigger B

 to SNES Pad
 PD2 -- LATCH
 PD3 -- CLK
 PD4 -- DATA (pull down with 10k OHM register)

 */

#include "debug.h"


/* Global define */


/* Global Variable */
vu8 val;
volatile uint16_t snes_pad_status = 0;

/*********************************************************************
 * @fn      USARTx_CFG
 *
 * @brief   Initializes the USART2 & USART3 peripheral.
 *
 * @return  none
 */
void USARTx_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);

    /* USART1 TX-->D.5   RX-->D.6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

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
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
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

void joystick_init() {

    GPIO_InitTypeDef GPIO_InitStructure = { 0 };

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);

    // PC0 to PC5

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5, Bit_SET);

}

void joystick_write() {

    if(snes_pad_status&0x0800) {  // UP
        GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_SET);
    } else {
        GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_RESET);
    }

    if(snes_pad_status&0x0400) {  // DOWN
        GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_SET);
    } else {
        GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_RESET);
    }

    if(snes_pad_status&0x0200) {  // LEFT
        GPIO_WriteBit(GPIOC, GPIO_Pin_2, Bit_SET);
    } else {
        GPIO_WriteBit(GPIOC, GPIO_Pin_2, Bit_RESET);
    }

    if(snes_pad_status&0x0100) {  // RIGHT
        GPIO_WriteBit(GPIOC, GPIO_Pin_3, Bit_SET);
    } else {
        GPIO_WriteBit(GPIOC, GPIO_Pin_3, Bit_RESET);
    }

    if(snes_pad_status&0x0080) {  // A
        GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_SET);
    } else {
        GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_RESET);
    }

    if(snes_pad_status&0x8000) {  // B
        GPIO_WriteBit(GPIOC, GPIO_Pin_5, Bit_SET);
    } else {
        GPIO_WriteBit(GPIOC, GPIO_Pin_5, Bit_RESET);
    }

}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
//    Delay_Init();
    snes_pad_init();
    joystick_init();
//#if (SDI_PRINT == SDI_PR_OPEN)
//    SDI_Printf_Enable();
//#else
//    USART_Printf_Init(115200);
//#endif
//    printf("SystemClk:%d\r\n",SystemCoreClock);
//    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
//
//    USARTx_CFG();

    while(1)
    {

           snes_pad_read();
           joystick_write();

    }
}
