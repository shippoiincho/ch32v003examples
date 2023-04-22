/*

Neopixel demo

Connect Neopixel on PC6

*/

#include "debug.h"

/* Global define */

#define NUMPIXELS 10

#define DELAYVAL1 20 // Time (in milliseconds) to pause between pixels
#define COLOR_STEP 1024L
//#define COLOR_STEP 512

/* Global Variable */

uint8_t neopixels[3][NUMPIXELS];  // Order:GRB
uint16_t neoPixelPatterns[4] = { 0xc0c0, 0xf0c0, 0xc0f0, 0xf0f0 };

uint32_t colors[NUMPIXELS];
uint16_t color_new = 0;

/*********************************************************************
 * @fn      SPI_1Lines_HalfDuplex_Init
 *
 * @brief   Configuring the SPI for half-duplex communication.
 *
 * @return  none
 */
void SPI_1Lines_HalfDuplex_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };
    SPI_InitTypeDef SPI_InitStructure = { 0 };
    NVIC_InitTypeDef NVIC_InitStructure = { 0 };

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init( SPI1, &SPI_InitStructure);

    SPI_Cmd( SPI1, ENABLE);
}

uint32_t ColorHSV(uint16_t hue, uint8_t sat, uint8_t val) {

    uint8_t r, g, b;
    hue = (hue * 1530L + 32768) / 65536;

    // Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
    if (hue < 510) {         // Red to Green-1
        b = 0;
        if (hue < 255) {       //   Red to Yellow-1
            r = 255;
            g = hue;            //     g = 0 to 254
        } else {              //   Yellow to Green-1
            r = 510 - hue;      //     r = 255 to 1
            g = 255;
        }
    } else if (hue < 1020) { // Green to Blue-1
        r = 0;
        if (hue < 765) {      //   Green to Cyan-1
            g = 255;
            b = hue - 510;      //     b = 0 to 254
        } else {              //   Cyan to Blue-1
            g = 1020 - hue;     //     g = 255 to 1
            b = 255;
        }
    } else if (hue < 1530) { // Blue to Red-1
        g = 0;
        if (hue < 1275) {      //   Blue to Magenta-1
            r = hue - 1020;     //     r = 0 to 254
            b = 255;
        } else {              //   Magenta to Red-1
            r = 255;
            b = 1530 - hue;     //     b = 255 to 1
        }
    } else {                // Last 0.5 Red (quicker than % operator)
        r = 255;
        g = b = 0;
    }

    // Apply saturation and value to R,G,B, pack into 32-bit result:
    uint32_t v1 = 1 + val; // 1 to 256; allows >>8 instead of /255
    uint16_t s1 = 1 + sat; // 1 to 256; same reason
    uint8_t s2 = 255 - sat; // 255 to 0
    return ((((((r * s1) >> 8) + s2) * v1) & 0xff00) << 8)
            | (((((g * s1) >> 8) + s2) * v1) & 0xff00)
            | (((((b * s1) >> 8) + s2) * v1) >> 8);
}

void drawNeopixel() {

    uint8_t dat = 0;

    // reset

    SPI_I2S_SendData( SPI1, 0);
    while( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_TXE ) == RESET );

    Delay_Us(300);

    for (int i = 0; i < NUMPIXELS; i++) {
        for (int j = 0; j < 3; j++) {
            dat = neopixels[j][i];
            SPI_I2S_SendData( SPI1, neoPixelPatterns[(dat & 0xc0) >> 6]);
            while( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_TXE ) == RESET );
            SPI_I2S_SendData( SPI1, neoPixelPatterns[(dat & 0x30) >> 4]);
            while( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_TXE ) == RESET );
            SPI_I2S_SendData( SPI1, neoPixelPatterns[(dat & 0x0c) >> 2]);
            while( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_TXE ) == RESET );
            SPI_I2S_SendData( SPI1, neoPixelPatterns[dat & 0x3]);
            while( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_TXE ) == RESET );
        }
    }
}

void setPixelColor(uint8_t led, uint32_t color) {
    neopixels[0][led] = (color & 0xff00) >> 8;        // G
    neopixels[1][led] = (color & 0xff0000) >> 16;     // R
    neopixels[2][led] = (color & 0xff);               // B
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void) {

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    Delay_Init();

    SPI_1Lines_HalfDuplex_Init();

    for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
        colors[i] = 0;
    }

    while(1)
    {

        color_new +=COLOR_STEP;

        //  pixels.clear(); // Set all pixel colors to 'off'

        for(int i=NUMPIXELS-1; i>0; i--) {
            colors[i]=colors[i-1];
        }

        //  colors[0]=pixels.ColorHSV(color_new, 255,255);
        colors[0]=ColorHSV(color_new,255,255);

        for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
            setPixelColor(i,colors[i] );
        }
        drawNeopixel();
        Delay_Ms(DELAYVAL1);

    }

}
