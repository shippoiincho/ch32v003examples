#include "debug.h"
#include "glcdfont.h"
#include <string.h>

/* Global define */

/* Global Variable */

void TM1640_Init(void) {

    // PD3 = CLK
    // PD2 = DATA

    GPIO_InitTypeDef GPIO_InitStructure = { 0 };

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_SET);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_SET);

}

void TM1640_Send(uint8_t data) {

    for (int i = 0; i < 8; i++) {

        GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET);
        GPIO_WriteBit(GPIOD, GPIO_Pin_2, data & 1 ? Bit_SET : Bit_RESET);
        data >>= 1;
        GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_SET);

    }

}

void TM1640_Output(uint8_t *buff) {
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_RESET);
    TM1640_Send(0x40);
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_SET);
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_RESET);
    TM1640_Send(0xc0);

    for (int i = 0; i < 16; i++) {
        TM1640_Send(buff[i]);
    }
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_SET);
}

void TM1640_print(unsigned char *str) {

    int len, pos;
    uint8_t buff[32];

    len = strlen(str);

    for (int i = -3; i < len; i++) {
        for (int offset = 0; offset < 6; offset++) {

            // clear buffer
            for (int j = 0; j < 16; j++) {
                buff[j] = 0;
            }

            // render 1st char

            pos = 0;

            if (i >= 0) {
                for (int j = 0; j < 5; j++) {
                    if (j >= offset) {
                        buff[pos++] = font[str[i] * 5 + j];
                    }
                }
            } else {
                pos=5-offset;
            }

            pos++;

            // render 2nd char

            if ((i < len) && (i>=-1)) {
                for (int j = 0; j < 5; j++) {
                    buff[pos++] = font[str[i + 1] * 5 + j];
                }
            } else {
                pos+=5;
            }

            pos++;

            // render 3rd char

            if ((i < len - 1)&&(i>=-2)) {
                for (int j = 0; j < 5; j++) {
                    buff[pos++] = font[str[i + 2] * 5 + j];
                }
            } else {
                pos+=5;
            }

            buff[pos++] = 0;

            // render 4th char

            if ((i < len - 2)&&(i>=-3)) {
                for (int j = 0; j < 5; j++) {
                    buff[pos++] = font[str[i + 3] * 5 + j];
                }
            } else {
                pos+=5;
            }

            pos++;

            TM1640_Output(buff);
            Delay_Ms(100);

        }

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

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();

    TM1640_Init();

    GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_RESET);
    TM1640_Send(0x8f);
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_SET);

    while(1)
    {

        TM1640_print("Hello,world.");
        TM1640_print("CH32V003 test program");

    }
}
