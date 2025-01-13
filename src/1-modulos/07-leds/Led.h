/*******************************************************************************************************************************//**
 *
 * @file		Led.h
 * @brief		Breve descripción del objetivo del Módulo
 * @date		14/7/2024
 * @author		Grupo 2
 *
 **********************************************************************************************************************************/

#ifndef LEDS_LED_H_
#define LEDS_LED_H_

#include "Gpio.h"
#include "perifericotemporizado.h"
#include "systick.h"

#define BLINK_DEFAULT		500

class Led:public PerifericoTemporizado
{
	public:
		enum led_t { OFF , ON , BLINK } ;
	private:
		led_t 		m_Estado;
		const uint8_t 	m_NumeroDeLed;
		uint32_t 	m_Ticks;
		uint32_t 	m_SemiPeriodo;
		bool 		m_flag ;
		void 		(*m_CallbackLeds)(uint8_t , led_t );
	public:
		Led( uint8_t Nled , void (*callbackLeds)(uint8_t , led_t ),
				uint32_t periodo = BLINK_DEFAULT);
		void On( void );
		void Off( void );
		void Blink( void );
		void HandlerDelPeriferico( void );
		void SemiPeriodo( uint32_t periodo );
		Led& operator=( led_t estado );

		virtual ~Led();
};


#endif /* LEDS_LED_H_ */
