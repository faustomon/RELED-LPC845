/*******************************************************************************************************************************//**
 *
 * @file		Led.cpp
 * @brief		Descripcion del modulo
 * @date		14/10/2024
 * @author		Grupo 2
 *
 **********************************************************************************************************************************/
#include "Led.h"

Led::Led( uint8_t NumeroDeLed ,
		void (*CallbackLeds)( uint8_t , led_t ) ,
		uint32_t Semi_Periodo ):m_NumeroDeLed (NumeroDeLed)
{
	m_SemiPeriodo = Semi_Periodo ;
	m_Estado = OFF ;
	m_CallbackLeds = CallbackLeds ;
	m_Ticks = 0 ;
	m_flag = true ;

	InstalarPerifericoTemporizado(this);

	return ;
}

void Led::On(void)
{
	m_Estado = ON ;
	m_Ticks = 0 ;
	return ;
}

void Led::Off( void )
{
	m_Estado = OFF ;
	m_Ticks = 0 ;
	return ;
}

void Led::Blink( void )
{
	m_Estado = BLINK ;
	if ( !m_Ticks )
		m_Ticks = m_SemiPeriodo ;
	return ;
}

void Led::SemiPeriodo( uint32_t Semi_Periodo )
{
	m_SemiPeriodo = Semi_Periodo ;
	return ;
}

Led& Led::operator=( led_t Estado )
{
	switch ( Estado )
	{
		case OFF:
			Off();
			break;
		case ON:
			On();
			break;
		case BLINK:
			Blink();
			break;
	}
	return *this ;
}

void Led::HandlerDelPeriferico(void)
{
	switch( m_Estado )
	{
		case ON:
		case OFF:
			m_CallbackLeds( m_NumeroDeLed , m_Estado );
			break;
		case BLINK:
			m_Ticks --;

			if ( !m_Ticks )
			{
				m_Ticks = m_SemiPeriodo ;
				if( m_flag )
					m_CallbackLeds( m_NumeroDeLed , ON);
				else
					m_CallbackLeds( m_NumeroDeLed , OFF);
				m_flag = !m_flag;
			}
			break;
	}
	return ;
}


Led::~Led()
{
	// TODO Auto-generated destructor stub
}

