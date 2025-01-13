/*******************************************************************************************************************************//**
 *
 * @file		OLED.cpp
 * @brief		Metodos y constructor de la clase OLED
 * @date		11/12/2024
 * @author		Grupo 2
 *
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include <OLED.h>
/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** TIPOS DE DATOS PRIVADOS AL MODULO
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 **********************************************************************************************************************************/
static uint8_t OLED_Buffer[(OLED_WIDTH * OLED_HEIGHT) / 8];
/***********************************************************************************************************************************
 *** IMPLEMENTACION DE LOS METODOS DE LA CLASE
 **********************************************************************************************************************************/

// Constructor OLED que inicializa las respectivas variables de la clase e instala el periferico temporizado, retorna void
OLED::OLED(I2C* i2c, uint8_t slaveAddress){

	m_i2c = i2c;
	m_slaveAddress = slaveAddress;
	fps = 1000;
	handler = false;
	flag_grafica = false;
	flag_refresh = false;
	//InstalarPerifericoTemporizado(this);
}
/*
// Handler del periferico temporizado, retorna void
void OLED::HandlerDelPeriferico(void){
	// cada 40ms se vuelve a ejecutar el refresh
	// lo que en teoria son con 25fps
	static uint8_t cnt_timer = 1;
	if( handler ){
		if( cnt_timer==0 ){
			flag_grafica = true;
			flag_refresh = true;
			cnt_timer = (uint16_t)(1000/fps);
		}
		cnt_timer--;
	}
}
*/
// Setea los fotogramas por segundos del OLED, retorna void
void OLED::set_fps(uint16_t fps){
	this->fps = fps;
}

// Activa el auto refresh de la OLED, retorna void
void OLED::enable_auto_refresh(){
	handler = true;
}

// Get del flag_grafica, retorna bool
bool OLED::get_flagGrafica(){
	bool salida = flag_grafica;
	flag_grafica = false;
	return salida;
}

// Envia los comandos al OLED usando el metodo WriteBlocking de I2C, retorna void
void OLED::OLED_Command (uint8_t Cmd)
{
	// mandar el comando al oled
	m_i2c->WriteBlocking(m_slaveAddress, 0x00 , (uint8_t *)&Cmd, (uint32_t)sizeof(Cmd));
	//for(int i=0; i<1000; i++);
}

// Envia los data al OLED usando el metodo WriteBlocking de I2C, retorna void
void OLED::OLED_Data (uint8_t *Data)
{
	// mandar el dato al oled
	m_i2c->WriteBlocking(m_slaveAddress, 0x40 , Data, (uint32_t)sizeof(OLED_Buffer));
	//for(int i=0; i<1000; i++);
}


// Funcion para Resetear la OLED, retorna void
void OLED::OLED_Reset (void)
{
	OLED_Command(OLED_DISPLAYON);
//	for(int i=0; i<1000; i++);					//delay 1ms
	OLED_Command(OLED_DISPLAYOFF);
//	for(int i=0; i<1000; i++);					//delay 1ms
	OLED_Command(OLED_DISPLAYON);
//	for(int i=0; i<10000; i++);					//delay 10ms
}

// Configura todos los parametros de la OLED para inicializarla, retorna void
void OLED::OLED_Config_Display (void)
{

	OLED_Reset();


	OLED_Command(OLED_DISPLAYOFF);				// Registro DISPLAYOFF

	// Configira el display para 128x64 pixels, modo KS0108
	OLED_Command(OLED_SETDISPLAYCLOCKDIV); 		// Registro SETDISPLAYCLOCKDIV
	OLED_Command(0x80);							// Envia 0x80
	OLED_Command(OLED_SETMULTIPLEX);			// Registro SETMULTIPLEX
	OLED_Command(OLED_HEIGHT-1);				// LCD Height
	OLED_Command(OLED_SETDISPLAYOFFSET);		// Registro SETDISPLAYOFFSET
	OLED_Command(0x0);							// Envia 0x0
	OLED_Command(OLED_SETSTARTLINE | 0x0);		// Registro SETSTARTLINE | 0x0
	OLED_Command(OLED_CHARGEPUMP);				// Registro CHARGEPUMP
	OLED_Command(0x14);							// Envia 0x14, usa alimentacion 3.3V para generar alimentacion de alto voltaje
	OLED_Command(OLED_MEMORYMODE);				// Registro MEMORYMODE
	OLED_Command(0x00);							// Envia 0x00
	OLED_Command(OLED_SEGREMAP | 0x1);			// Registro OLED_SEGREMAP | 0x1
	OLED_Command(OLED_COMSCANDEC);				// Registro COMSCANDEC
	OLED_Command(OLED_SETCOMPINS);				// Registro SETCOMPINS
	OLED_Command(0x12);							// Envia 0x12
	OLED_Command(OLED_SETCONTRAST);				// Registro SETCONTRAST
	OLED_Command(0xCF);							// Envia 0xCF
	OLED_Command(OLED_SETPRECHARGE);			// Registro SETPRECHARGE
	OLED_Command(0xF1);							// Envia 0xF1
	OLED_Command(OLED_SETVCOMDETECT);			// Registro SETVCOMDETECT
	OLED_Command(0x40);							// Envia 0x40
	OLED_Command(OLED_DISPLAYALLON_RESUME);		// Registro DISPLAYALLON_RESUME
	OLED_Command(OLED_NORMALDISPLAY);			// Registro NORMALDISPLAY
	OLED_Command(OLED_DEACTIVATE_SCROLL);		// Registro DEACTIVATE_SCROLL

	OLED_Command(OLED_COLUMNADDR);				// Registro COLUMNADDR
	OLED_Command(0);							// Envia 0
	OLED_Command(OLED_WIDTH-1);					// LCD Height
	OLED_Command(OLED_PAGEADDR);				// Registro PAGEADDR
	OLED_Command(0);							// Envia 0
	OLED_Command(OLED_HEIGHT/8-1);				// LCD Height / 8


	OLED_Command(OLED_DISPLAYON);				// Registro DISPLAYON

	OLED_Command(OLED_SETLOWCOLUMN | 0x0);  	// low column = 0
	OLED_Command(OLED_SETHIGHCOLUMN | 0x0);  	// high column = 0
	OLED_Command(OLED_SETSTARTLINE | 0x0); 		// line #0

}


// Renderiza pixel por pixel lo que se quiere mostrar en la OLED sabiendo pos X e Y, texto a mostrar, y tamaño, retorna int
int OLED::OLED_Render_Char (uint8_t X_axis, uint8_t Y_axis, uint8_t SC, int8_t String, uint8_t Scale)
{
	uint8_t px, py;
	uint16_t start_pos;

	if ((X_axis >= OLED_WIDTH) || (Y_axis >= OLED_HEIGHT)) {
		return 1;
	}
	if (String > 127) {
		return 2;
	}
	if (Scale > 3) {
		return 3;
	}

	start_pos = ((uint8_t)String) * 7;			// Los caracteres tiene un offstet de 7 filas
	for (px=0; px<5; px++) {
		for (py=0; py<7; py++) {
			if ((font5x7[start_pos+py] >> (7-px)) & 1) {
				switch (Scale) {
				case 3:
					OLED_Set_Pixel(X_axis+(px*Scale),   Y_axis+(py*Scale),  SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+1, Y_axis+(py*Scale),  SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+2, Y_axis+(py*Scale),  SC);
					OLED_Set_Pixel(X_axis+(px*Scale),   Y_axis+(py*Scale)+1, SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+1, Y_axis+(py*Scale)+1, SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+2, Y_axis+(py*Scale)+1, SC);
					OLED_Set_Pixel(X_axis+(px*Scale),   Y_axis+(py*Scale)+2, SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+1, Y_axis+(py*Scale)+2, SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+2, Y_axis+(py*Scale)+2, SC);
					break;
				case 2:
					OLED_Set_Pixel(X_axis+(px*Scale),   Y_axis+(py*Scale),  SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+1, Y_axis+(py*Scale),  SC);
					OLED_Set_Pixel(X_axis+(px*Scale),   Y_axis+(py*Scale)+1, SC);
					OLED_Set_Pixel(X_axis+(px*Scale)+1, Y_axis+(py*Scale)+1, SC);
					break;
				case 1:
				default:
					OLED_Set_Pixel(X_axis+px, Y_axis+py, SC);
					break;
				}
			}
		}
	}
	return 0;
}


// Inicializa la OLED, primero la resetea, luego la limpia y luego hace toda la rutina de configuracion
void OLED::OLED_Init(void){
	OLED_Reset();

	OLED_Clear();

	OLED_Config_Display();
}


// Refresca la pantalla de la OLED, retorna void
void OLED::OLED_Refresh(void){
	//if( flag_refresh ){
		OLED_Command(0xb0);							// Envia (0xb0)
		OLED_Command(((0&0xf0)>>4) | 0x10);			// Envia ((0&0xf0)>>4) | 0x10
		OLED_Command((0&0x0f) | 0x01);				// Envia (0&0x0f) | 0x01

		OLED_Data(&OLED_Buffer[0]);					// Envia data de &OLED_Buffer[0]
		flag_refresh = false;
	//}
}


// Limpia la OLED y su buffer, retorna void
void OLED::OLED_Clear(void){
	memset(OLED_Buffer, 0, sizeof(OLED_Buffer));
}


// Funcion para mostrar algun patron en la pantalla, retorna void
void OLED::OLED_Fill(uint8_t Pattern){
	memset(OLED_Buffer, Pattern, sizeof(OLED_Buffer));
}


// Funcion para setear si el display se lee Horizontal o Vertical, retorna void
void OLED::OLED_Display_Mode (uint8_t Mode){
	if (Mode) {
		OLED_Command(OLED_INVERTDISPLAY); // Vertical
	}
	else {
		OLED_Command(OLED_NORMALDISPLAY); // Horizontal
	}

}


// Funcion para configurar cada pixel de la OLED, retorna void
void OLED::OLED_Set_Pixel (uint8_t X_axis, uint8_t Y_axis, uint8_t SC){
	if((X_axis >= OLED_WIDTH) || (Y_axis >= OLED_HEIGHT)){
			// Error al pasarle los parametros al OLED, exceso de tamaño
		}
		else{
			switch(SC){
				case kOLED_Pixel_Clear:
					OLED_Buffer [X_axis + (Y_axis / 8) * OLED_WIDTH] &= ~(1 << (Y_axis & 7));
					break;
				case kOLED_Pixel_Set:
					OLED_Buffer [X_axis + (Y_axis / 8) * OLED_WIDTH] |= (1 << (Y_axis & 7));
					break;
			}
		}
}


// Funcion para escribir texto en la OLED, usa, posicion XY, tamaño, texto y SC para setPixel, retorna void
void OLED::OLED_Set_Text (uint8_t X_axis, uint8_t Y_axis, uint8_t SC, char* String, uint8_t Scale){
	uint16_t Cont;
		uint16_t xscaled;

		if ((X_axis >= OLED_WIDTH) || (Y_axis >= OLED_HEIGHT)) {
			// Error al pasarle los parametros al OLED, exceso de tamaño
		}
		else{
			if (Scale > 3) {
				// Error al pasarle los parametros al OLED, exceso de escala
			}
			else{
				for (Cont = 0; String[Cont] != '\0'; Cont++) {
					// Overflow de escala
					xscaled = X_axis+(Cont*5*Scale); // Aca puedo manipular el tamaño a mi gusto
					if (xscaled > OLED_WIDTH){
						// Error al pasarle los parametros al OLED, exceso de tamaño
					}
					else{
						OLED_Render_Char(xscaled, Y_axis, SC, String[Cont], Scale);
					}
				}
			}
		}
}


// Funcion para llenar de rectas la OLED, retorna void
void OLED::OLED_Fill_Rect (uint8_t X_axis, uint8_t Y_axis, uint8_t Width, uint8_t Height, uint8_t SC){
	if((X_axis >= OLED_WIDTH) || (Y_axis >= OLED_HEIGHT)){
		// Error al pasarle los parametros al OLED, exceso de tamaño
	}
	else{
		switch(SC){
			case kOLED_Pixel_Clear:
				for(uint8_t i=0;i<Height;i++){
					for(uint8_t j=0;j<Width;j++){
						OLED_Buffer [(X_axis+j) + ((Y_axis+i) / 8) * OLED_WIDTH] &= ~(1 << ((Y_axis+i) & 7));
					}
				}
				break;
			case kOLED_Pixel_Set:
				for(uint8_t i=0;i<Height;i++){
					for(uint8_t j=0;j<Width;j++){
						OLED_Buffer [(X_axis+j) + ((Y_axis+i) / 8) * OLED_WIDTH] |= (1 << ((Y_axis+i) & 7));
					}
				}
				break;
		}
	}
}


// Funcion para mostrar una imagen en el OLED, retorna void
void OLED::OLED_Copy_Image(const uint8_t *Img, uint16_t size){
	uint16_t CpyBuffer;
		OLED_Clear();
		for(CpyBuffer = 0; CpyBuffer < size - 1; CpyBuffer++){
			OLED_Buffer[CpyBuffer] = *(Img + CpyBuffer);
		}
}
