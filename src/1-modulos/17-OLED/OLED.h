/*******************************************************************************************************************************//**
 *
 * @file		OLED.h
 * @brief		Clase OLED
 * @date		11/12/2024
 * @author		Grupo 2
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MODULO
 **********************************************************************************************************************************/
#ifndef SRC_1_MODULOS_17_OLED_OLED_H_
#define SRC_1_MODULOS_17_OLED_OLED_H_

/***********************************************************************************************************************************
 *** INCLUDES GLOBALES
 **********************************************************************************************************************************/
#include "LPC845.h"
#include "I2C.h"
#include "fsl_Font5x7.h"
#include "string.h"

/***********************************************************************************************************************************
 *** DEFINES GLOBALES
 **********************************************************************************************************************************/
#define OLED_SETCONTRAST 							(0x81)
#define OLED_DISPLAYALLON_RESUME 					(0xA4)
#define OLED_DISPLAYALLON 							(0xA5)
#define OLED_NORMALDISPLAY 							(0xA6)
#define OLED_INVERTDISPLAY 							(0xA7)
#define OLED_DISPLAYOFF 							(0xAE)
#define OLED_DISPLAYON 								(0xAF)
#define OLED_SETDISPLAYOFFSET 						(0xD3)
#define OLED_SETCOMPINS 							(0xDA)
#define OLED_SETVCOMDETECT 							(0xDB)
#define OLED_SETDISPLAYCLOCKDIV 					(0xD5)
#define OLED_SETPRECHARGE 							(0xD9)
#define OLED_SETMULTIPLEX 							(0xA8)
#define OLED_SETLOWCOLUMN 							(0x00)
#define OLED_SETHIGHCOLUMN 							(0x10)
#define OLED_SETSTARTLINE 							(0x40)
#define OLED_MEMORYMODE 							(0x20)
#define OLED_COLUMNADDR 							(0x21)
#define OLED_PAGEADDR   							(0x22)
#define OLED_COMSCANINC 							(0xC0)
#define OLED_COMSCANDEC 							(0xC8)
#define OLED_SEGREMAP 								(0xA0)
#define OLED_CHARGEPUMP 							(0x8D)
#define OLED_EXTERNALVCC 							(0x01)
#define OLED_SWITCHCAPVCC 							(0x02)
#define OLED_ACTIVATE_SCROLL 						(0x2F)
#define OLED_DEACTIVATE_SCROLL 						(0x2E)
#define OLED_SET_VERTICAL_SCROLL_AREA 				(0xA3)
#define OLED_RIGHT_HORIZONTAL_SCROLL 				(0x26)
#define OLED_LEFT_HORIZONTAL_SCROLL 				(0x27)
#define OLED_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 	(0x29)
#define OLED_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 	(0x2A)

// El display puede responder a alguna de estas 2 direcciones.
// Si con una no funciona cambiar a la otra:
#define SSD1306_ADDRESS_1							(0x3C)
#define SSD1306_ADDRESS_2							(0x3D)

// OLED Data/Command PIN
#ifndef BOARD_OLED_DC_GPIO
#define BOARD_OLED_DC_GPIO GPIO
#endif
#define BOARD_OLED_DC_GPIO_PORT 0U
#ifndef BOARD_OLED_DC_GPIO_PIN
#define BOARD_OLED_DC_GPIO_PIN 	1U
#endif

// OLED Reset PIN
#ifndef BOARD_OLED_RST_GPIO
#define BOARD_OLED_RST_GPIO GPIO
#endif
#define BOARD_OLED_RST_GPIO_PORT 0U
#ifndef BOARD_OLED_RST_GPIO_PIN
#define BOARD_OLED_RST_GPIO_PIN  16U
#endif

// OLED CS PIN
#ifndef BOARD_OLED_CS_GPIO
#define BOARD_OLED_CS_GPIO GPIO
#endif
#define BOARD_OLED_CS_GPIO_PORT 0U
#ifndef BOARD_OLED_CS_GPIO_PIN
#define BOARD_OLED_CS_GPIO_PIN 	20U
#endif

// OLED Width
#ifndef OLED_WIDTH
#define OLED_WIDTH 128
#endif

// OLED Height
#ifndef OLED_HEIGHT
#define OLED_HEIGHT 64
#endif

/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** IMPLANTACION DE UNA CLASE
 **********************************************************************************************************************************/
class OLED{ // hereda de PerifericoTemporizado
private:
	I2C * m_i2c;
	uint8_t m_slaveAddress;
	uint16_t fps;
	bool handler;
	bool flag_grafica;
	bool flag_refresh;
public:
	OLED(I2C* i2c, uint8_t slaveAddress);

	void OLED_Init(void);

	void OLED_Refresh(void);

	void OLED_Clear(void);

	void OLED_Fill(uint8_t Pattern);

	void OLED_Display_Mode (uint8_t Mode);

	void OLED_Set_Pixel (uint8_t X_axis, uint8_t Y_axis, uint8_t SC);

	void OLED_Set_Text (uint8_t X_axis, uint8_t Y_axis, uint8_t SC, char* String, uint8_t Scale);

	void OLED_Fill_Rect (uint8_t X_axis, uint8_t Y_axis, uint8_t Width, uint8_t Height, uint8_t SC);

	void OLED_Copy_Image(const uint8_t *Img, uint16_t size);

	int32_t OLED_Render_Char (uint8_t X_axis, uint8_t Y_axis, uint8_t SC, int8_t String, uint8_t Scale);

	void OLED_Config_Display (void);

	void OLED_Reset (void);

	void OLED_Data (uint8_t *Data);

	void OLED_Command (uint8_t Cmd);

	void set_fps(uint16_t fps);

	void enable_auto_refresh();

	bool get_flagGrafica();

	enum _OLED_Display_Mode{ kOLED_Display_Normal, kOLED_Dislpay_Invert };
	enum _OLED_Pixel_CLR_SET{ kOLED_Pixel_Clear, kOLED_Pixel_Set };


};


#endif /* SRC_1_MODULOS_17_OLED_OLED_H_ */
