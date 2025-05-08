#include "ssd1331.h"
#include <stdint.h>
#include <string.h>
#include "stm32f4xx_hal.h"  // Include HAL library (make sure it's correctly included for your STM32 series)

#define SSD1331_WIDTH   96
#define SSD1331_HEIGHT  64

// Define the macros for controlling the GPIO pins (CS, DC, RST)
#define OLED_DC_LOW()   HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET)
#define OLED_DC_HIGH()  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET)
#define OLED_CS_LOW()   HAL_GPIO_WritePin(CS_SC_GPIO_Port, CS_SC_Pin, GPIO_PIN_RESET)
#define OLED_CS_HIGH()  HAL_GPIO_WritePin(CS_SC_GPIO_Port, CS_SC_Pin, GPIO_PIN_SET)
#define OLED_RST_LOW()  HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET)
#define OLED_RST_HIGH() HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET)

// SPI communication functions
static void ssd1331_write_command(uint8_t cmd) {
	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
	    HAL_GPIO_WritePin(CS_SC_GPIO_Port, CS_SC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, &cmd, 1, HAL_MAX_DELAY);
        HAL_GPIO_WritePin(CS_SC_GPIO_Port, CS_SC_Pin, GPIO_PIN_SET);;
}

static void ssd1331_write_data(uint8_t data) {
	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CS_SC_GPIO_Port, CS_SC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, &data, 1, HAL_MAX_DELAY);
    OLED_CS_HIGH();
}

static void ssd1331_write_data16(uint16_t data) {
    uint8_t buf[2] = { data >> 8, data & 0xFF };
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CS_SC_GPIO_Port, CS_SC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, buf, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(CS_SC_GPIO_Port, CS_SC_Pin, GPIO_PIN_SET);
}

// Initialization function for SSD1331
void SSD1331_Init(void) {
	// Pradžioje nustatykite reset į žemą (aktyvuojame reset)
	 HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
	    HAL_Delay(100);   // Laikykite žemą 100 ms (rekomenduojamas laiko intervalas)

	    // Po to nustatykite reset į aukštą (grąžiname į aukštą)
	 HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
	    HAL_Delay(100);   // Laikykite aukštą 100 ms, kad ekranas pradėtų inicializaciją

	    // Tada galite tęsti ekranų inicializacijos komandas:
	    SSD1331_SendCommand(CMD_DISPLAY_OFF);
	    SSD1331_SendCommand(CMD_SET_REMAP);
	    SSD1331_SendCommand(0x72);

    SSD1331_SendCommand(CMD_DISPLAY_OFF);
    SSD1331_SendCommand(CMD_SET_REMAP);
    SSD1331_SendCommand(0x72);
    SSD1331_SendCommand(CMD_SET_DISPLAY_START_LINE);
    SSD1331_SendCommand(0x0);
    SSD1331_SendCommand(CMD_SET_DISPLAY_OFFSET);
    SSD1331_SendCommand(0x0);
    SSD1331_SendCommand(CMD_NORMAL_DISPLAY);
    SSD1331_SendCommand(CMD_SET_MULTIPLEX_RATIO);
    SSD1331_SendCommand(0x3F);
    SSD1331_SendCommand(CMD_SET_MASTER_CONFIGURE);
    SSD1331_SendCommand(0x8E);
    SSD1331_SendCommand(CMD_POWER_SAVE_MODE);
    SSD1331_SendCommand(0x0B);
    SSD1331_SendCommand(CMD_PHASE_PERIOD_ADJUSTMENT);
    SSD1331_SendCommand(0x31);
    SSD1331_SendCommand(CMD_DISPLAY_CLOCK_DIV);
    SSD1331_SendCommand(0xF0);
    SSD1331_SendCommand(CMD_SET_PRECHARGE_SPEED_A);
    SSD1331_SendCommand(0x64);
    SSD1331_SendCommand(CMD_SET_PRECHARGE_SPEED_B);
    SSD1331_SendCommand(0x78);
    SSD1331_SendCommand(CMD_SET_PRECHARGE_SPEED_C);
    SSD1331_SendCommand(0x64);
    SSD1331_SendCommand(CMD_SET_PRECHARGE_VOLTAGE);
    SSD1331_SendCommand(0x3A);
    SSD1331_SendCommand(CMD_SET_V_VOLTAGE);
    SSD1331_SendCommand(0x3E);
    SSD1331_SendCommand(CMD_MASTER_CURRENT_CONTROL);
    SSD1331_SendCommand(0x06);
    SSD1331_SendCommand(CMD_SET_CONTRAST_A);
    SSD1331_SendCommand(0x91);
    SSD1331_SendCommand(CMD_SET_CONTRAST_B);
    SSD1331_SendCommand(0x50);
    SSD1331_SendCommand(CMD_SET_CONTRAST_C);
    SSD1331_SendCommand(0x7D);
    SSD1331_SendCommand(CMD_NORMAL_BRIGHTNESS_DISPLAY_ON);
    SSD1331_Clear();
}

// Function to clear the screen
void SSD1331_Clear(void) {
    SSD1331_SendCommand(CMD_CLEAR_WINDOW);
    SSD1331_SendCommand(0);
    SSD1331_SendCommand(0);
    SSD1331_SendCommand(SSD1331_WIDTH - 1);
    SSD1331_SendCommand(SSD1331_HEIGHT - 1);
}

// Function to draw a pixel on the screen
void SSD1331_DrawPixel(uint8_t x, uint8_t y, uint32_t color) {
    if (x >= SSD1331_WIDTH || y >= SSD1331_HEIGHT) return;

    // Siųsti komandą nustatyti stulpelio ir eilutės adresus
    SSD1331_SendCommand(CMD_SET_COLUMN_ADDRESS);
    SSD1331_SendCommand(x);   // Nustatyti pradinią stulpelio adresą
    SSD1331_SendCommand(x);   // Nustatyti paskutinį stulpelio adresą

    SSD1331_SendCommand(CMD_SET_ROW_ADDRESS);
    SSD1331_SendCommand(y);   // Nustatyti pradinį eilutės adresą
    SSD1331_SendCommand(y);   // Nustatyti paskutinį eilutės adresą

    // Siųsti RGB duomenis
    uint8_t rgb[] = {
        (color >> 16) & 0xFF,  // Raudona
        (color >> 8) & 0xFF,   // Žalia
        color & 0xFF           // Mėlyna
    };
    SSD1331_SendData(rgb, 3); // Siųsti RGB duomenis
}

// Helper functions for sending commands and data
void SSD1331_SendCommand(uint8_t cmd) {
    // CS = LOW
    HAL_GPIO_WritePin(CS_SC_GPIO_Port, CS_SC_Pin, GPIO_PIN_RESET);
    // DC = 0 (komanda)
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
    // Siųsti komandą
    HAL_SPI_Transmit(&hspi2, &cmd, 1, HAL_MAX_DELAY);
    // CS = HIGH
    HAL_GPIO_WritePin(CS_SC_GPIO_Port, CS_SC_Pin, GPIO_PIN_SET);
}

void SSD1331_SendData(uint8_t *data, uint16_t size) {
    // CS = LOW
    HAL_GPIO_WritePin(CS_SC_GPIO_Port, CS_SC_Pin, GPIO_PIN_RESET);
    // DC = 1 (duomenys)
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
    // Siųsti duomenis
    HAL_SPI_Transmit(&hspi2, data, size, HAL_MAX_DELAY);
    // CS = HIGH
    HAL_GPIO_WritePin(CS_SC_GPIO_Port, CS_SC_Pin, GPIO_PIN_SET);
}

void SSD1331_DrawChar(uint8_t x, uint8_t y, char c, uint32_t color) {
    if (c < 32 || c > 126) return;  // Patikrinimas, ar simbolis yra validus
    const uint8_t *bitmap = Font5x7[c - 32];  // Gauti simbolio duomenis iš šrifto

    for (uint8_t col = 0; col < 5; col++) {  // 5 stulpeliai
        uint8_t line = bitmap[col];  // Paimti atitinkamą simbolio eilutę
        for (uint8_t row = 0; row < 7; row++) {  // 7 eilutės
            if (line & 0x01) {  // Patikrinimas, ar bitas yra 1
                SSD1331_DrawPixel(x + col, y + row, color);  // Piešti pikselį
            }
            line >>= 1;  // Pereiti prie kito bito
        }
    }
}


// Atvaizduoja tekstą
void SSD1331_DrawString(uint8_t x, uint8_t y, const char *str, uint32_t color) {
    while (*str) {
        SSD1331_DrawChar(x, y, *str++, color);
        x += 6;
        if (x > SSD1331_WIDTH - 6) {
            x = 0;
            y += 8;
        }
        if (y > SSD1331_HEIGHT - 8) break;
    }
}
void SSD1331_DrawFilledRectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color) {
    if (x0 >= SSD1331_WIDTH || y0 >= SSD1331_HEIGHT || x1 >= SSD1331_WIDTH || y1 >= SSD1331_HEIGHT)
        return;

    // Siunčiam komandą piešti užpildytą stačiakampį
    SSD1331_SendCommand(0x22);  // Draw filled rectangle

    // Koordinatės (kampai)
    SSD1331_SendCommand(x0);
    SSD1331_SendCommand(y0);
    SSD1331_SendCommand(x1);
    SSD1331_SendCommand(y1);

    // Kontūro spalva (naudojam tą pačią kaip ir užpildymo, arba galima daryti kitaip)
    SSD1331_SendCommand((color >> 16) & 0xFF); // Red
    SSD1331_SendCommand((color >> 8) & 0xFF);  // Green
    SSD1331_SendCommand(color & 0xFF);         // Blue

    // Užpildo spalva
    SSD1331_SendCommand((color >> 16) & 0xFF); // Red
    SSD1331_SendCommand((color >> 8) & 0xFF);  // Green
    SSD1331_SendCommand(color & 0xFF);         // Blue
}


