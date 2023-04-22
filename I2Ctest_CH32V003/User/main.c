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
 *@Note
 7-bit addressing mode, master/slave mode, transceiver routine:
 I2C1_SCL(PC2)\I2C1_SDA(PC1).
 This routine demonstrates that Master sends and Slave receives.
 Note: The two boards download the Master and Slave programs respectively,
 and power on at the same time.
 Hardware connection:
 PC2 -- PC2
 PC1 -- PC1

 */

#include "debug.h"
#include "glcdfont.h"

/* Global define */

#define ssd1306Address   0x78
#define bme280Address    0xec

/* Global Variable */

uint8_t ssd1306InitCommand[] = { 0x00, 0x20, 0x02, 0x21, 0x00, 0x7F, 0x22, 0x00,
        0x07, 0x8D, 0x14, 0xAF };

unsigned long int hum_raw, temp_raw, pres_raw;
signed long int t_fine;

// BME280 trim factor

uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;
uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;
int8_t dig_H1;
int16_t dig_H2;
int8_t dig_H3;
int16_t dig_H4;
int16_t dig_H5;
int8_t dig_H6;

/*********************************************************************
 * @fn      IIC_Init
 *
 * @brief   Initializes the IIC peripheral.
 *
 * @return  none
 */
void IIC_Init(u32 bound, u16 address) {
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };
    I2C_InitTypeDef I2C_InitTSturcture = { 0 };

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure);

    I2C_InitTSturcture.I2C_ClockSpeed = bound;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init( I2C1, &I2C_InitTSturcture);

    I2C_Cmd( I2C1, ENABLE);

    I2C_AcknowledgeConfig( I2C1, ENABLE);

}

void ssd1306Init(void) {

    while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );
    I2C_GenerateSTART( I2C1, ENABLE);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( I2C1, ssd1306Address, I2C_Direction_Transmitter);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

    for (int i = 0; i < 12; i++) {
        I2C_SendData( I2C1, ssd1306InitCommand[i]);
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
    }

    I2C_GenerateSTOP( I2C1, ENABLE);

}

void ssd1306Fill(uint8_t pattern) {

    for (int page = 0; page < 8; page++) {

        // Set draw position

        while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );

        I2C_GenerateSTART( I2C1, ENABLE);

        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );
        I2C_Send7bitAddress( I2C1, ssd1306Address, I2C_Direction_Transmitter);

        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

        I2C_SendData( I2C1, 0x00);
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
        I2C_SendData( I2C1, 0x20);
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
        I2C_SendData( I2C1, 0x02);
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

        I2C_SendData( I2C1, 0xb0 + page);
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
        I2C_SendData( I2C1, 0x10);
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
        I2C_SendData( I2C1, 0x00);
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

        I2C_GenerateSTOP( I2C1, ENABLE);

        // send bitmap

        while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );

        I2C_GenerateSTART( I2C1, ENABLE);

        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );
        I2C_Send7bitAddress( I2C1, ssd1306Address, I2C_Direction_Transmitter);

        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

        I2C_SendData( I2C1, 0x40);
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
        for (int i = 0; i < 128; i++) {
            I2C_SendData( I2C1, pattern);
            while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
        }

        I2C_GenerateSTOP( I2C1, ENABLE);

    }

}

void ssd1306PrintChar(uint8_t x, uint8_t y, uint8_t chr) {

    unsigned int xpos;

    // 128x64 = 21 x 8 characters

    if ((x < 21) && (y < 8)) {

        xpos = x * 6;

        // Set draw position

        while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );

        I2C_GenerateSTART( I2C1, ENABLE);

        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );
        I2C_Send7bitAddress( I2C1, ssd1306Address, I2C_Direction_Transmitter);

        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

        I2C_SendData( I2C1, 0x00);
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
        I2C_SendData( I2C1, 0x20);
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
        I2C_SendData( I2C1, 0x02);
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
        I2C_SendData( I2C1, 0xb0 + (y & 0x7));
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
        I2C_SendData( I2C1, 0x10 + ((xpos & 0xf0) >> 4));
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
        I2C_SendData( I2C1, 0x00 + xpos & 0x0f);
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

        I2C_GenerateSTOP( I2C1, ENABLE);

        // send bitmap

        while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );

        I2C_GenerateSTART( I2C1, ENABLE);

        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );
        I2C_Send7bitAddress( I2C1, ssd1306Address, I2C_Direction_Transmitter);

        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

        I2C_SendData( I2C1, 0x40);
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
        for (int i = 0; i < 5; i++) {
            I2C_SendData( I2C1, font[chr * 5 + i]);
            while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
        }
        I2C_SendData( I2C1, 0x00);
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

        I2C_GenerateSTOP( I2C1, ENABLE);

    }

}

void ssd1306Print(uint8_t x, uint8_t y, uint8_t *str) {

    while(*str!=0) {

        ssd1306PrintChar(x, y, *str);
        str++;
        x++;
        if(x>21) break;
    }

}

void bme280Init(void) {

    uint8_t osrs_t = 1;             //Temperature oversampling x 1
    uint8_t osrs_p = 1;             //Pressure oversampling x 1
    uint8_t osrs_h = 1;             //Humidity oversampling x 1
    uint8_t mode = 3;               //Normal mode
    uint8_t t_sb = 5;               //Tstandby 1000ms
    uint8_t filter = 0;             //Filter off
    uint8_t spi3w_en = 0;           //3-wire SPI Disable

    uint8_t ctrl_meas_reg = (osrs_t << 5) | (osrs_p << 2) | mode;
    uint8_t config_reg = (t_sb << 5) | (filter << 2) | spi3w_en;
    uint8_t ctrl_hum_reg = osrs_h;

    // F2

    while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );
    I2C_GenerateSTART( I2C1, ENABLE);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( I2C1, bme280Address, I2C_Direction_Transmitter);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

    I2C_SendData( I2C1, 0xf2);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
    I2C_SendData( I2C1, ctrl_hum_reg);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

    I2C_GenerateSTOP( I2C1, ENABLE);

    // F4

    while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );
    I2C_GenerateSTART( I2C1, ENABLE);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( I2C1, bme280Address, I2C_Direction_Transmitter);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

    I2C_SendData( I2C1, 0xf4);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
    I2C_SendData( I2C1, ctrl_meas_reg);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

    I2C_GenerateSTOP( I2C1, ENABLE);

    // F5

    while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );
    I2C_GenerateSTART( I2C1, ENABLE);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( I2C1, bme280Address, I2C_Direction_Transmitter);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

    I2C_SendData( I2C1, 0xf2);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
    I2C_SendData( I2C1, config_reg);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

    I2C_GenerateSTOP( I2C1, ENABLE);

}

void bme280ReadTrim(void) {

    // read trimming data

    uint8_t data[32], i;

    while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );

    I2C_GenerateSTART( I2C1, ENABLE);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( I2C1, bme280Address, I2C_Direction_Transmitter);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

    I2C_SendData( I2C1, 0x88);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

    I2C_GenerateSTART( I2C1, ENABLE);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( I2C1, bme280Address, I2C_Direction_Receiver);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) );

    for (i = 0; i < 25; i++) {
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED ) );
        data[i] = I2C_ReceiveData( I2C1);
    }

    I2C_GenerateSTOP( I2C1, ENABLE);

    while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );

    I2C_GenerateSTART( I2C1, ENABLE);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( I2C1, bme280Address, I2C_Direction_Transmitter);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

    I2C_SendData( I2C1, 0xE1);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

    I2C_GenerateSTART( I2C1, ENABLE);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( I2C1, bme280Address, I2C_Direction_Receiver);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) );

    for (i = 0; i < 7; i++) {
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED ) );
        data[i + 25] = I2C_ReceiveData( I2C1);
    }

    I2C_GenerateSTOP( I2C1, ENABLE);

    dig_T1 = (data[1] << 8) | data[0];
    dig_T2 = (data[3] << 8) | data[2];
    dig_T3 = (data[5] << 8) | data[4];
    dig_P1 = (data[7] << 8) | data[6];
    dig_P2 = (data[9] << 8) | data[8];
    dig_P3 = (data[11] << 8) | data[10];
    dig_P4 = (data[13] << 8) | data[12];
    dig_P5 = (data[15] << 8) | data[14];
    dig_P6 = (data[17] << 8) | data[16];
    dig_P7 = (data[19] << 8) | data[18];
    dig_P8 = (data[21] << 8) | data[20];
    dig_P9 = (data[23] << 8) | data[22];
    dig_H1 = data[24];
    dig_H2 = (data[26] << 8) | data[25];
    dig_H3 = data[27];
    dig_H4 = (data[28] << 4) | (0x0F & data[29]);
    dig_H5 = (data[30] << 4) | ((data[29] >> 4) & 0x0F);
    dig_H6 = data[31];

}

void bme280Read(void) {

    uint8_t data[8], i;

    while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );

    I2C_GenerateSTART( I2C1, ENABLE);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( I2C1, bme280Address, I2C_Direction_Transmitter);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

    I2C_SendData( I2C1, 0xf7);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

    I2C_GenerateSTART( I2C1, ENABLE);

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( I2C1, bme280Address, I2C_Direction_Receiver);
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) );

    for (i = 0; i < 8; i++) {
        while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED ) );
        data[i] = I2C_ReceiveData( I2C1);
    }

    I2C_GenerateSTOP( I2C1, ENABLE);

    pres_raw = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    temp_raw = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
    hum_raw = (data[6] << 8) | data[7];

}

signed long int calibration_T(signed long int adc_T) {

    signed long int var1, var2, T;
    var1 = ((((adc_T >> 3) - ((signed long int) dig_T1 << 1)))
            * ((signed long int) dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((signed long int) dig_T1))
            * ((adc_T >> 4) - ((signed long int) dig_T1))) >> 12)
            * ((signed long int) dig_T3)) >> 14;

    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

unsigned long int calibration_P(signed long int adc_P) {
    signed long int var1, var2;
    unsigned long int P;
    var1 = (((signed long int) t_fine) >> 1) - (signed long int) 64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((signed long int) dig_P6);
    var2 = var2 + ((var1 * ((signed long int) dig_P5)) << 1);
    var2 = (var2 >> 2) + (((signed long int) dig_P4) << 16);
    var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3)
            + ((((signed long int) dig_P2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((signed long int) dig_P1)) >> 15);
    if (var1 == 0) {
        return 0;
    }
    P = (((unsigned long int) (((signed long int) 1048576) - adc_P)
            - (var2 >> 12))) * 3125;
    if (P < 0x80000000) {
        P = (P << 1) / ((unsigned long int) var1);
    } else {
        P = (P / (unsigned long int) var1) * 2;
    }
    var1 = (((signed long int) dig_P9)
            * ((signed long int) (((P >> 3) * (P >> 3)) >> 13))) >> 12;
    var2 = (((signed long int) (P >> 2)) * ((signed long int) dig_P8)) >> 13;
    P =
            (unsigned long int) ((signed long int) P
                    + ((var1 + var2 + dig_P7) >> 4));
    return P;
}

unsigned long int calibration_H(signed long int adc_H) {
    signed long int v_x1;

    v_x1 = (t_fine - ((signed long int) 76800));
    v_x1 = (((((adc_H << 14) - (((signed long int) dig_H4) << 20)
            - (((signed long int) dig_H5) * v_x1)) + ((signed long int) 16384))
            >> 15)
            * (((((((v_x1 * ((signed long int) dig_H6)) >> 10)
                    * (((v_x1 * ((signed long int) dig_H3)) >> 11)
                            + ((signed long int) 32768))) >> 10)
                    + ((signed long int) 2097152)) * ((signed long int) dig_H2)
                    + 8192) >> 14));
    v_x1 = (v_x1
            - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7)
                    * ((signed long int) dig_H1)) >> 4));
    v_x1 = (v_x1 < 0 ? 0 : v_x1);
    v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);
    return (unsigned long int) (v_x1 >> 12);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void) {

    uint8_t x, y = 0;
    uint8_t str[64];

    signed long int temp_cal;
    unsigned long int press_cal, hum_cal;

    Delay_Init();

    //  IIC_Init(400000, ssd1306Address);
    IIC_Init(100000, bme280Address);

    ssd1306Init();

    bme280Init();
    bme280ReadTrim();

    while(1) {

        ssd1306Fill(0);

        bme280Read();

        temp_cal = calibration_T(temp_raw);
        press_cal = calibration_P(pres_raw);
        hum_cal = calibration_H(hum_raw);

        sprintf(str,"Temp: %2d.%02d",temp_cal/100,temp_cal%100);
        ssd1306Print(0, 0, str);

        sprintf(str,"Pres: %4d.%02d",press_cal/100,press_cal%100);
        ssd1306Print(0, 1, str);

        sprintf(str,"Humi: %2d.%02d",hum_cal/1024,hum_cal%1024);
        ssd1306Print(0, 2, str);

        Delay_Ms(5000);

    }

}

