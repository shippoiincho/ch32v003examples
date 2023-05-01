/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/08
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 * Standby Mode test
 *
 * PD1 Input (Pull down to wake up)
 * PD2 Buzzer Out
 *
 */

#include "debug.h"

/* Global define */

/* PWM Output Mode Definition */
#define PWM_MODE1   0
#define PWM_MODE2   1

/* PWM Output Mode Selection */
#define PWM_MODE PWM_MODE1
//#define PWM_MODE PWM_MODE2

/* Global Variable */

/*********************************************************************
 * @fn      EXTI0_INT_INIT
 *
 * @brief   Initializes EXTI0 collection.
 *
 * @return  none
 */
void EXTI0_INT_INIT(void) {
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };
    EXTI_InitTypeDef EXTI_InitStructure = { 0 };

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* GPIOA.0 ----> EXTI_Line0 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    //   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

}

/*********************************************************************
 * @fn      TIM1_OutCompare_Init
 *
 * @brief   Initializes TIM1 output compare.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void TIM1_PWMOut_Init(u16 arr, u16 psc, u16 ccp) {
    TIM_OCInitTypeDef TIM_OCInitStructure = { 0 };
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = { 0 };

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

#if (PWM_MODE == PWM_MODE1)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

#elif (PWM_MODE == PWM_MODE2)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;

#endif

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init( TIM1, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig( TIM1, ENABLE);
    TIM_Cmd( TIM1, ENABLE);
}

void toneinit(void) {

    GPIO_InitTypeDef GPIO_InitStructure = { 0 };

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_TIM1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOD, &GPIO_InitStructure);

}

void tone(uint16_t freq) {

    uint16_t duration;

    duration = 1000000 / freq;   // in usec

    TIM1_PWMOut_Init(duration, 47, duration / 2);

}

void notone(void) {
    TIM_Cmd(TIM1, DISABLE);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void) {
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    Delay_Ms(5000); // fail safe for reprogramming

    RCC_APB2PeriphClockCmd(
    RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //GPIO_Init(GPIOD, &GPIO_InitStructure);

    while(1) {

        EXTI0_INT_INIT();

//        RCC_LSICmd(ENABLE);
//        while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

        PWR_EnterSTANDBYMode(PWR_STANDBYEntry_WFE);

        //  __WFI();

        // re-configure system clock

        SystemInit();
        Delay_Init();

        // Pi-Po

        toneinit();

        tone(2000);
        Delay_Ms(250);

        tone(1000);
        Delay_Ms(250);

        notone();
    }

}

void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      EXTI0_IRQHandler
 *
 * @brief   This function handles EXTI0 Handler.
 *
 * @return  none
 */
void EXTI7_0_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        printf("EXTI0 Wake_up\r\n");
        EXTI_ClearITPendingBit(EXTI_Line0); /* Clear Flag */
    }
}

