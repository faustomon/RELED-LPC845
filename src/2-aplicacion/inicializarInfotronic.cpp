/*******************************************************************************************************************************//**
 *
 * @file		inicializar.cpp
 * @brief		Descripcion del modulo
 * @date		5 jul. 2022
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/

#include "inicializarInfotronic.h"
#include "aplicacion.h"

/***********************************************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/**********************************************************************************
 *** OBJETOS GLOBALES PUBLICOS
 **********************************************************************************/

//teclado
#ifdef KIT2022
	Gpio g_ret0 ( Gpio::PORT0 ,  8 , Gpio::PULLUP , Gpio::INPUT , Gpio::LOW );
	Gpio g_ret1 ( Gpio::PORT0 , 31 , Gpio::PULLUP , Gpio::INPUT , Gpio::LOW );
	Gpio g_ret2 ( Gpio::PORT0 , 30 , Gpio::PULLUP , Gpio::INPUT , Gpio::LOW );

	Gpio g_scn0 ( Gpio::PORT0, 27, Gpio::PUSHPULL , Gpio::OUTPUT , Gpio::HIGH );
	Gpio g_scn1 ( Gpio::PORT0, 28, Gpio::PUSHPULL , Gpio::OUTPUT , Gpio::HIGH );
#else
	Gpio g_ret0 ( Gpio::PORT0 , 26 , Gpio::PULLUP , Gpio::INPUT , Gpio::LOW );
	Gpio g_ret1 ( Gpio::PORT0 , 15 , Gpio::PULLUP , Gpio::INPUT , Gpio::LOW );
	Gpio g_ret2 ( Gpio::PORT0 ,  8 , Gpio::PULLUP , Gpio::INPUT , Gpio::LOW );

	Gpio g_scn0 ( Gpio::PORT0, 27, Gpio::PUSHPULL , Gpio::OUTPUT , Gpio::HIGH );
	Gpio g_scn1 ( Gpio::PORT0, 28, Gpio::PUSHPULL , Gpio::OUTPUT , Gpio::HIGH );
#endif

Gpio *g_scn[] = {&g_scn0 , &g_scn1, nullptr};
Gpio *g_ret[] = {&g_ret0 , &g_ret1, &g_ret2,  nullptr};

DigitalOutputs g_BUZZER( Gpio::PORT0 , 4 , Gpio::PUSHPULL ,  Gpio::HIGH , Gpio::OFF);

DigitalOutputs g_LED_AMANECER( Gpio::PORT0 , 13 , Gpio::PUSHPULL ,  Gpio::HIGH , Gpio::OFF);

Teclado g_Teclado( g_ret , g_scn );

// terminal MCUXpresso
Uart uart0(
        0 ,                 // PortTx
        25,                 // Pin Tx
        0 ,                    // PortRx
        24,                 // Pin Rx
        0 ,                    // uart seleccionada
        9600 ,                // baudios
        Uart::ocho_bits ,    // bits de datos
        Uart::NoParidad ,    // tipo de paridad
        64,                    // tamaño de buffer de Rx
        64 );                // tamaño de buffer de Tx



I2C *i2c = new I2C();
RTC *rtc = new RTC(i2c, 0x68, nullptr);
OLED *oled = new OLED(i2c, SSD1306_ADDRESS_1);

void InicializarInfotronic ( void )
{
	Inicializar_PLL( );

	SysTick_InstalarCallBack( Sheduller );
	SysTick_Inicializar(1);
}

