/*******************************************************************************************************************************//**
 *
 * @file		adc.cpp
 * @brief		Descripcion del modulo
 * @date		9/11/2024
 * @author		Grupo 2
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include <1-modulos/12-adc/Adc.h>
#include "Gpio.h"

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/
#define	ISE_ADC_SEQA			16
#define START					26
#define BURST					27
#define SINGLESTEP				28

#define	TAMANIO_BUFFER_ADC		16
#define	CANTIDAD_DE_CANALES		12

#define SEQA					0
#define SEQB					1

#define CLK_500KHZ 				500000U
#define CLOCKS_PER_SAMPLE		25


/***********************************************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPOS DE DATOS PRIVADOS AL MODULO
 **********************************************************************************************************************************/
struct canal
{
	uint16_t 		indice;
	bool 			enable;
	Gpio::port_t	port;
	uint8_t 		bit;
};

/***********************************************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 **********************************************************************************************************************************/
struct canal CanalesHabilitados[] =
{
		{ 0 , true , Gpio::PORT0 ,   7 },	//canal 0
		{ 0 , false , Gpio::PORT0 ,   6 },	//canal 1
		{ 0 , false , Gpio::PORT0 ,  14 },	//canal 2
		{ 0 , false , Gpio::PORT0 ,  23 },	//canal 3
		{ 0 , false , Gpio::PORT0 ,  22 },	//canal 4
		{ 0 , false , Gpio::PORT0 ,  21 },	//canal 5
		{ 0 , false , Gpio::PORT0 ,  20 },	//canal 6
		{ 0 , false , Gpio::PORT0 ,  19 },	//canal 7
		{ 0 , false , Gpio::PORT0 ,  18 },	//canal 8
		{ 0 , false , Gpio::PORT0 ,  17 },	//canal 9
		{ 0 , false , Gpio::PORT0 ,  13 },	//canal 10
		{ 0 , false , Gpio::PORT0 ,   4 }	//canal 11
};

uint8_t Adc::m_inicializar = 0 ;

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 **********************************************************************************************************************************/
Adc	*g_adc = nullptr ;

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 **********************************************************************************************************************************/
static uint32_t g_resultado[CANTIDAD_DE_CANALES][TAMANIO_BUFFER_ADC] ;


/***********************************************************************************************************************************
 *** IMPLEMENTACION DE LOS METODODS DE LA CLASE
 **********************************************************************************************************************************/
Adc::Adc( uint8_t canal ,  resultado_t TipoDeFiltro )
{
	m_callback = nullptr;
	m_Indice = 0;
	m_NumeroCanal = canal ;
	m_TipoDeFiltro = TipoDeFiltro ;
	Inicializar();
}

Adc::Adc( uint8_t canal , int32_t (*callback)(int32_t) )
{
	m_callback = callback;
	m_Indice = 0;
	m_NumeroCanal = canal ;
	m_TipoDeFiltro = CALLBACK ;
	Inicializar();
}

void Adc::Inicializar( void )
{
	if ( !m_inicializar )
	{
		m_inicializar = true;
		// #### 1 ####
		SYSCON->PDRUNCFG &= ~SYSCON_PDRUNCFG_ADC_MASK;				// Habilitamos alimentacion del periferico

		// #### 2 ####
		SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_ADC_MASK;	// Habilitamos clock del periferico
		SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_SWM_MASK; // 7 = SWM

		// #### 3 ####
		SYSCON->ADCCLKSEL = 0;
		SYSCON->ADCCLKDIV = 1; 										// Setea divisor de ADC en 1

		// #### 4 ####
		for ( uint8_t i = 0 ; i < (sizeof( CanalesHabilitados) / sizeof( struct canal) ) ; i++ )
		{
			if ( CanalesHabilitados[i].enable )
				SWM0->PINENABLE0 &= ~SWM_PINENABLE0_ADC_MASK( i ); 	//registro activo bajo;

		}

		// #### 5 ####
		Calibrar( );

		// #### 6 ####
		ADC0->CTRL &= ~(uint32_t)ADC_CTRL_ASYNMODE_MASK; // Modo Sincrónico de clock
		Set_clk_div( 12 );
		ADC0->CTRL &= ~(uint32_t)ADC_CTRL_LPWRMODE_MASK; // Modo Low Power off

		// Configuramos secuencia de conversion A para canal 0
		for ( uint8_t i = 0 ; i < (sizeof( CanalesHabilitados) / sizeof( struct canal) ); i++)
		{
			if ( CanalesHabilitados[i].enable )
				Set_channels( SEQA , i );
		}

		// Configuramos modo para que la interrupcion se efectue al finalizar toda la secuencia
		Set_mode_end_of_conversion( SEQA );
		// Habilitamos secuencia de conversion A
		Enable_sequence( SEQA );
		// Habilitamos interrupciones para fin de conversion de secuencia A
		Enable_interrupt( SEQA );
		// Set_step_mode( SEQA ) ;

		g_adc = this ;

		NVIC->ISER[0] |= (1 << ISE_ADC_SEQA);
		Start_conversion( SEQA );
	}
}

void Adc::Set_mode_end_of_conversion( uint8_t seq )
{
	ADC0->SEQ_CTRL[ seq ] &= ~(1 << 30);
}

void Adc::Set_mode_end_of_sequence( uint8_t seq )
{
	ADC0->SEQ_CTRL[ seq ] |= (1 << 30);
}

void Adc::Enable_sequence( uint8_t seq )
{
	ADC0->SEQ_CTRL[ seq ] |= (1 << 31) ;
}

void Adc::Enable_interrupt( uint8_t seq )
{
	ADC0->INTEN |= (1 << seq ) ;
}

void Adc::Disable_interrupt( uint8_t seq )
{
	ADC0->INTEN &= ~(1 << seq ) ;
}

void Adc::Set_clk_div( uint8_t div )
{
	ADC0->CTRL &= ~0x000000FF;
	ADC0->CTRL |= div;
}

void Adc::Set_channels( uint8_t seq , uint16_t channel )
{
	uint32_t channels = 0;

	channels = 1 << channel ;
	ADC0->SEQ_CTRL[ seq ] |= ( channels & 0x03FF ) ;
}

void Adc::Start_conversion( uint8_t seq )
{
	ADC0->SEQ_CTRL[ seq ] |= (1 << 26) ;
}

void Adc::Set_burst_mode( uint8_t seq )
{
	ADC0->SEQ_CTRL[ seq ] &= ~( 1 << START);
	ADC0->SEQ_CTRL[ seq ] &= ~( 1 << SINGLESTEP);
	ADC0->SEQ_CTRL[ seq ] |=  ( 1 << BURST);
}

void Adc::Set_single_sequence_mode( uint8_t seq )
{
	ADC0->SEQ_CTRL[ seq ] &= ~( 1<< SINGLESTEP );
	ADC0->SEQ_CTRL[ seq ] &= ~( 1<< BURST );
	ADC0->SEQ_CTRL[ seq ] |=  ( 1 << START );
}

void Adc::Set_step_mode( uint8_t seq )
{
	ADC0->SEQ_CTRL[ seq ] &= ~( 1 << BURST );
	ADC0->SEQ_CTRL[ seq ] &= ~( 1 << START );
	ADC0->SEQ_CTRL[ seq ] |=  ( 1 << SINGLESTEP );
}

		
void Adc::SeqA_IRQHandler(void)
{
	uint32_t resultado;
	uint8_t canal ;

	resultado =  ADC0->SEQ_GDAT[0];
	canal = ( resultado >> 26 ) & 0xf;

	switch ( m_TipoDeFiltro )
	{
		case CRUDO:
			g_resultado[ canal ][ 0 ] = ((resultado >> 4) & 0xFFF);
			break;
		case PROMEDIO:
		case CALLBACK:

			g_resultado[ canal ][CanalesHabilitados[canal].indice] = ((resultado >> 4) & 0xFFF);

			break;
	}
	CanalesHabilitados[canal].indice ++;
	CanalesHabilitados[canal].indice %= TAMANIO_BUFFER_ADC;
	ADC0->FLAGS |= (1 << 28); // Limpiar interrupcion por SEQA
	Start_conversion( SEQA );
}



void Adc::SeqB_IRQHandler(void)
{
	// ### codigo de la funcion  ##

	return ;
}
void Adc::Thcmp_IRQHandler(void)
{
	// ### codigo de la funcion  ##

	return ;
}
void Adc::Ovr_IRQHandler(void)
{
	// ### codigo de la funcion  ##
	return ;
}

uint32_t Adc::GetDivisor(uint32_t clk_freq, uint32_t sample_rate){
	return (uint32_t)(((clk_freq / sample_rate) - 1)/CLOCKS_PER_SAMPLE);
}

void Adc::Calibrar( void )
{
	uint32_t old_ctrl = ADC0->CTRL;
	uint32_t new_ctrl = old_ctrl;
	// Configurar ADC freq @ 500kHz
	new_ctrl &= ~ADC_CTRL_CLKDIV_MASK; //CLKDIV = 0x00
	new_ctrl |= ADC_CTRL_CLKDIV(GetDivisor(FREQ_CLOCK, CLK_500KHZ)); // Configurar a 500KHz
	new_ctrl &= ~ADC_CTRL_LPWRMODE_MASK; // LPWR bit = 0
	new_ctrl |= ADC_CTRL_CALMODE_MASK; // CALMODE bit = 1
	/* Start ADC self-calibration. */
	ADC0->CTRL = new_ctrl;

	while( ADC0->CTRL & ADC_CTRL_CALMODE_MASK) {
		//Esperar CALMODE = 0. Podría haber un timeout de 300us...
	}; 

	/* Recuperar el valor original del registro CTRL. */
	ADC0->CTRL = old_ctrl;
}

void Adc::Set_filtro (uint32_t tipo) {

	switch (tipo) {

		case PROMEDIO:
			m_TipoDeFiltro = PROMEDIO;
			break;

		case CRUDO:
		default:
			m_TipoDeFiltro = CRUDO;
			break;
	}
}

void ADC_SEQA_IRQHandler(void)
{
	if ( g_adc )
		g_adc->SeqA_IRQHandler( );
}

void ADC_SEQB_IRQHandler(void)
{
	if ( g_adc )
		g_adc->SeqB_IRQHandler( );

}
void ADC_THCMP_IRQHandler(void)
{
	if ( g_adc )
		g_adc->Thcmp_IRQHandler( );

}
void ADC_OVR_IRQHandler(void)
{
	if ( g_adc )
		g_adc->Ovr_IRQHandler( );

}

int32_t Adc::Get_resultado(void) {

	uint32_t resultadoParcial;

	switch ( m_TipoDeFiltro )
	{
		case CRUDO :
			resultadoParcial = g_resultado[m_NumeroCanal][0];
			break;
		case PROMEDIO:
			resultadoParcial = Promedio( );
			break;
		case CALLBACK:
			resultadoParcial = Promedio( );
			resultadoParcial = m_callback ( resultadoParcial );
			break;
	}

	return resultadoParcial;
}

uint32_t Adc::Promedio( void )
{
	uint32_t resultadoParcial = 0;
	for (uint32_t i=0 ; i < TAMANIO_BUFFER_ADC ; i++)
		resultadoParcial += g_resultado[m_NumeroCanal][i];
	resultadoParcial /= TAMANIO_BUFFER_ADC;
	return resultadoParcial ;
}

Adc::~Adc()
{
	// TODO Auto-generated destructor stub
}
