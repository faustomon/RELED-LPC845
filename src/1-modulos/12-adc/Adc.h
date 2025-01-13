/*******************************************************************************************************************************//**
 *
 * @file		adc.h
 * @brief		Breve descripción del objetivo del Módulo
 * @date		9/11/2024
 * @author		Grupo 2
 *
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** MODULO
 **********************************************************************************************************************************/

#ifndef CLASES_I1_ADC_ADC_H_
#define CLASES_I1_ADC_ADC_H_

/***********************************************************************************************************************************
 *** INCLUDES GLOBALES
 **********************************************************************************************************************************/
#include "systick.h"
#include "Gpio.h"

/***********************************************************************************************************************************
 *** DEFINES GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/
#if defined (__cplusplus)
	extern "C" {
	void ADC_SEQA_IRQHandler(void);
	void ADC_SEQB_IRQHandler(void) ;
	void ADC_THCMP_IRQHandler(void);
	void ADC_OVR_IRQHandler(void);
	}
#endif

/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** IMPLANTACION DE UNA CLASE
 **********************************************************************************************************************************/


class Adc
{
	private:
		uint32_t m_TipoDeFiltro ;
		uint8_t  m_Indice ;
		uint8_t  m_NumeroCanal;
		int32_t (*m_callback)(int32_t);
		static 	uint8_t m_inicializar;

	public:
		enum	resultado_t	{ CRUDO , PROMEDIO , CALLBACK };
		Adc(uint8_t , resultado_t TipoDeFiltro );
		Adc(uint8_t , int32_t (*callback)(int32_t));

		void SeqA_IRQHandler( void );
		void SeqB_IRQHandler( void );
		void Thcmp_IRQHandler( void );
		void Ovr_IRQHandler( void ) ;

		void Set_filtro( uint32_t tipo );
		int32_t Get_resultado( void );

		virtual ~Adc();
	private:
		void Inicializar( void );
		void Set_clk_div( uint8_t div );
		uint32_t GetDivisor(uint32_t clk_freq, uint32_t sample_rate);

		void Set_mode_end_of_conversion( uint8_t seq );
		void Set_mode_end_of_sequence( uint8_t seq );
		void Set_burst_mode( uint8_t seq );
		void Set_channels( uint8_t seq , uint16_t channel );
		void Set_single_sequence_mode( uint8_t seq );
		void Set_step_mode( uint8_t seq );

		void Enable_sequence( uint8_t seq );
		void Enable_interrupt( uint8_t seq );
		void Disable_interrupt( uint8_t seq );

		uint32_t Promedio( void );

		void Start_conversion( uint8_t seq );
		void Calibrar( void );
};

#endif /* CLASES_I1_ADC_ADC_H_ */
