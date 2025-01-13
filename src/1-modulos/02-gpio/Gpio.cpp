/*******************************************************************************************************************************//**
 *
 * @file		gpio.cpp
 * @brief		Descripcion del modulo
 * @date		22/6/2024
 * @author		Grupo 2
 *
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "Gpio.h"

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

/***********************************************************************************************************************************
 *** IMPLEMENTACION DE LOS METODODS DE LA CLASE
 **********************************************************************************************************************************/
Gpio::Gpio ( uint8_t port , uint8_t bit , uint8_t mode , uint8_t direction , uint8_t activity) :
m_port ( port) , m_bit ( bit ) , m_modo ( mode ) , m_direccion ( direction ) , m_actividad ( activity )
{
	m_error = OK;

	if ( m_port > PORT1 )
		m_error = ERROR;
	if ( m_port == PORT0 && m_bit > B_PORT0 )
		m_error = ERROR;
	if ( m_port == PORT1 && m_bit > B_PORT1 )
		m_error = ERROR;

	if ( m_error == OK)
	{

		SYSCON->SYSAHBCLKCTRL0 |= (1 << 7);								// 7 = SWM
		SYSCON->SYSAHBCLKCTRL0 |= (1 << 6) | (1 << 20) | (1 << 18);		// 6 = GPIO0	20 = GPIO1	18 = IOCON

		if ( m_direccion == Gpio::OUTPUT )
		{
			SetPinModeOut ( );
			SetDirOutputs ( );
		}
		else
		{
			SetDirInputs ( );
			SetPinModeIn();
		}
	}

}

uint8_t Gpio::MSetPin(uint8_t mascara)
{
	if ( m_error == OK )
	{
		GPIO->MPIN[m_port] |= (mascara << m_bit);
	}

	return m_error;
}

uint8_t Gpio::MClrPin(uint8_t mascara)
{
	if ( m_error == OK )
	{
		GPIO->MPIN[m_port] &= ~(mascara << m_bit);
	}

	return m_error;
}

uint8_t Gpio::SetPin ( void )
{
	if ( m_error == OK )
	{
		if ( m_actividad == HIGH )
			GPIO->B[ m_port ][m_bit]  = ON;
		else
			GPIO->B[ m_port ][m_bit]  = OFF;
	}

	return m_error;
}

uint8_t Gpio::ClrPin ( void )
{
	if ( m_error == OK )
	{
		if ( m_actividad == HIGH )
			GPIO->CLR[ m_port ] = 1 << m_bit ;
		else
			GPIO->SET[ m_port ] = 1 << m_bit ;
	}
	return m_error;
}

uint8_t Gpio::SetTogglePin ( void )
{
	if ( m_error == OK )
		GPIO->NOT[ m_port ] = 1 << m_bit ;

	return m_error;
}

uint8_t Gpio::SetDirOutputs ( void )
{
	if ( m_error == OK )
	{
		m_direccion = OUTPUT ;
		GPIO->DIRSET[ m_port ] = 1 << m_bit ;
	}

	return m_error ;
}

uint8_t Gpio::SetDirInputs ( void )
{
	if ( m_error == OK )
	{
		m_direccion = INPUT ;
		GPIO->DIRCLR[ m_port ] = 1 << m_bit ;
	}

	return m_error ;
}

uint8_t Gpio::SetToggleDir ( void )
{
	if ( m_error == OK )
		GPIO->DIRNOT[m_port] = ( 1 << m_bit );

	return m_error;
}

uint8_t Gpio::GetPin ( void ) const
{
	uint8_t in;
	in = GPIO->B[m_port][m_bit] ;
	in = (m_actividad == HIGH ) ? in : !in;

	if ( m_error == OK )
		return in;
	return m_error;
}

uint8_t Gpio::SetPinModeOut( void )
{
	return 1;
}

uint8_t Gpio::SetPinModeIn ( void )
{
	uint8_t Indice_PortPin ;

	if ( m_error == OK )
	{
		Indice_PortPin = IOCON_INDEX_PIO0[m_bit];
		if ( m_port )
			Indice_PortPin = IOCON_INDEX_PIO1[m_bit];

		IOCON->PIO[ Indice_PortPin ] = ~0x18;
		IOCON->PIO[ Indice_PortPin ] = m_modo << 3;
	}
	return m_error;
}

Gpio & Gpio::operator=( bool estado )
{
	if ( estado == ON )
		SetPin();
	else
		ClrPin();
	return *this;
}

bool Gpio::operator== ( bool estado )
{
	if ( GetPin() == estado )
		return true ;
	return false;
}

bool Gpio::operator== ( Gpio & gpio )
{
	if ( GetPin() == gpio.GetPin() )
		return true ;
	return false;
}
Gpio::operator bool( )
{
	return GetPin();
}
