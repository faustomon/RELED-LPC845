
/*******************************************************************************************************************************//**
 *
 * @file		inicializar.h
 * @brief		Breve descripción del objetivo del Módulo
 * @date		5 jul. 2022
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MODULO
 **********************************************************************************************************************************/
#ifndef APLICACION_H_
#define APLICACION_H_

/***********************************************************************************************************************************
 *** INCLUDES GLOBALES
 **********************************************************************************************************************************/
#include "tipos.h"
#include "inicializarInfotronic.h"
#include "perifericotemporizado.h"

/***********************************************************************************************************************************
 *** DEFINES GLOBALES
 **********************************************************************************************************************************/
typedef enum{ACTUAL, MAX_MIN} estados_temp_hum;


/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/

extern uint8_t m_Clock_Hora;
extern uint8_t m_Clock_Minutos;
extern bool m_Alarma_Activada;
extern bool m_Amanecer_Activado;
extern bool m_Buzzer_Activado;
extern uint8_t m_Alarma_Hora ;
extern uint8_t m_Alarma_Minutos;
extern uint8_t m_Tempo_minutos;
extern uint8_t m_Tempo_segundos;
extern uint8_t m_Work_minutos;
extern uint8_t m_Work_segundos;
extern uint8_t m_Break_minutos;
extern uint8_t m_Break_segundos;
extern uint8_t m_Temperatura;
extern uint8_t m_Cronometro_segundos;
extern uint8_t m_Cronometro_minutos;
extern uint8_t m_Cronometro_horas;
extern uint8_t m_Cronometro_aux;
extern uint8_t m_Temperatura_decimal;

/***********************************************************************************************************************************
 *** PROTOTIPOS GLOBALES
 **********************************************************************************************************************************/
void maqEstados( void );

void DisplayHour(void);

void DisplayTemporizador( void );

void DisplayPomodoro( void );

void DisplayAlarma( void );

void DisplayCronometro( void );

void Sheduller ( void );

void buzzerPIP( void );

#endif /* INICIALIZAR_H_ */
