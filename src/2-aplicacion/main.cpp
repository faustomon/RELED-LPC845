/*
===============================================================================
 Name        : main.c
 Author      : Ing.Marcelo Trujillo
 Description : Prueba Expansion 1
===============================================================================
*/
#include "aplicacion.h"
#include "inicializarInfotronic.h"

int main( void )
{
	InicializarInfotronic ( ) ;
	oled->OLED_Init();
	rtc->SetupHour(0, 0, 0);
	while(1){
		maqEstados();
	}

	return 0;
}



