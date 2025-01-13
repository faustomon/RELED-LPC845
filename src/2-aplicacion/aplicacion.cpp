/*******************************************************************************************************************************//**
 *
 * @file		Aplicacion.cpp
 * @brief		Desrrollo de la consigna solicitada
 * @date		23/04/24
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/

#include "inicializarInfotronic.h"

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
 *** OBJETOS GLOBALES PUBLICOS
 *********************************************************************************************************************************/

#include "aplicacion.h"

uint8_t m_Clock_Hora;
uint8_t m_Clock_Minutos;

bool m_Alarma_Activada = 1;
bool m_Amanecer_Activado = 1;
bool m_Buzzer_Activado = 1;

uint8_t m_Alarma_Hora = 7;
uint8_t m_Alarma_Minutos = 0;

uint8_t m_Tempo_minutos = 0;
uint8_t m_Tempo_segundos = 0;

uint8_t m_Work_minutos = 0;
uint8_t m_Work_segundos = 10;

uint8_t m_Break_minutos = 0;
uint8_t m_Break_segundos = 5;

uint8_t m_Cronometro_segundos = 0;
uint8_t m_Cronometro_minutos = 0;
uint8_t m_Cronometro_horas = 0;
uint8_t m_Cronometro_aux = 0;

//float m_Temperatura = 25.3;
//uint8_t temp_part_ent = 25;
//uint8_t temp_part_dec = 3;
uint8_t m_Temperatura_decimal;
uint8_t m_Temperatura;

uint8_t config_minutero;
uint8_t config_hora;

uint8_t config_temp_minuto = 0;
uint8_t config_temp_segundo = 0;

uint8_t config_alarma_hora;
uint8_t config_alarma_minuto;

bool flagPomodoroWork;

uint8_t tecla ;

uint8_t aux[16];

Timer t_Temporizador(Timer::SEG);
Timer t_PomodoroWork(Timer::SEG);
Timer t_PomodoroBreak(Timer::SEG);

uint32_t tiempoActual = 0;
uint8_t tiempoActual_aux = 0;

uint32_t PomodoroActual = 0;
uint8_t PomodoroActual_aux = 0;

char displayHour[40];

//uint8_t state = WAIT_ALARM;

typedef enum
{
	HORA_ACTUAL=0, CONFIG_MINUTOS_CLOCK, CONFIG_HORA_CLOCK, CLIMA, TEMPORIZADOR, CONFIG_SEGUNDOS_TEMP,CONFIG_MINUTOS_TEMP,
	TIMER_ON, TIMER_PAUSE,POMODORO, WORK_ON, BREAK_ON, POMODORO_PAUSE, ALARMA, CONFIG_MINUTOS_ALARM, CONFIG_HORA_ALARM, CRONOMETRO,
	CRONOMETRO_PAUSA, CRONOMETRO_ON

}EstadoReloj;

void DisplayHour( void ){
	checkPuerto();
	//Le pide al rtc la hora y hace un display de la hora
	//Dos textos en disitntas coordenadas
	//Hora Actual:
	//12:25:14
	uint8_t clock[6];
	rtc->Clock(clock);
	sprintf((char*)displayHour, "%u : %u : %u", clock[0], clock[1], clock[2]);
	oled->OLED_Clear();
	oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 1", 2);
	oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"HORA ACTUAL", 2);
	oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char *)displayHour, 2);
	oled->OLED_Refresh();

}
void DisplayTemporizador( void ){
	//Muestra el temporizador predeterminado
	//Temporizador
	//05:00
	sprintf((char*)aux, "    %u : %u", m_Tempo_minutos, m_Tempo_segundos);
	oled->OLED_Clear();
	oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 2", 2);
	oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"TEMPORIZADOR", 2);
	oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
	oled->OLED_Refresh();
}

void DisplayPomodoro( void ){
	//Muestra el temporizador pomodoro
	//POMODORO
	//25:00
	sprintf((char*)aux, "    %u : %u", m_Work_minutos, m_Work_segundos);
	oled->OLED_Clear();
	oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 2", 2);
	oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"POMODORO", 2);
	oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
	oled->OLED_Refresh();
}

void DisplayAlarma( void ){
	//Muestra la alarma default
	//ALARMA
	//07:00
	sprintf((char*)aux, "    %u : %u", m_Alarma_Hora, m_Alarma_Minutos);
	oled->OLED_Clear();
	oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 3", 2);
	oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"ALARMA", 2);
	oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
	oled->OLED_Refresh();
}
void DisplayCronometro( void ){
	//mmuestra el cronometro en 00:00
	//Cronometro
	// 00:00
	sprintf((char*)aux, "%u : %u : %u", m_Cronometro_horas, m_Cronometro_minutos, m_Cronometro_segundos);
	oled->OLED_Clear();
	oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 4", 2);
	oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"CRONOMETRO", 2);
	oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
	oled->OLED_Refresh();
}

void DisplayClima( void ){
	//mmuestra el cronometro en 00:00
	//Cronometro
	// 00:00
	sprintf((char*)aux, "TEMP: %u.%u", m_Temperatura, m_Temperatura_decimal);
	oled->OLED_Clear();
	oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 1", 2);
	oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"CLIMA ACTUAL", 2);
	oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
	oled->OLED_Refresh();
}

void buzzerPIP(){
	for(int i = 0; i < 200000; i++){
		g_BUZZER.set();
	}
	g_BUZZER.clr();
}

void maqEstados(void){
	static uint32_t estados = HORA_ACTUAL;
	static uint8_t clock[6];
	tecla = g_Teclado.GetKey();
	//TODO agregar el la llamar a la funcion que carga los datos;
	switch(estados){
	case HORA_ACTUAL:
		DisplayHour();
		if(tecla == LEFT){
			rtc->Clock(clock);
			config_minutero = clock[1];
			config_hora = clock[0];
			estados = CONFIG_MINUTOS_CLOCK;
		}
		else if(tecla == RIGHT){
			DisplayClima();
			estados = CLIMA;
		}
		else if(tecla  == UP){
			DisplayTemporizador();
			estados = TEMPORIZADOR;
		}
		break;
	case CONFIG_MINUTOS_CLOCK:
		sprintf((char*)aux, "%u : %u : 0", config_hora, config_minutero);
		oled->OLED_Clear();
		oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 1", 2);
		oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"CONFIG: MIN", 2);
		oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
		oled->OLED_Refresh();

		if(tecla == UP){
			config_minutero ++;
			if(config_minutero > 59){
				config_minutero = 1;
			}
		}
		else if(tecla == DOWN){
			config_minutero --;
			if(config_minutero <= 0){
				config_minutero = 59;
			}
		}
		else if(tecla  == LEFT){
			estados = CONFIG_HORA_CLOCK;
		}
		else if(tecla  == RIGHT || tecla  == BACK){
			estados = HORA_ACTUAL;
		}
		else if (tecla  == OK){
			rtc->SetMin(config_minutero);
			rtc->SetHour(config_hora);
			estados = HORA_ACTUAL;
		}
		break;
	case CONFIG_HORA_CLOCK:
		sprintf((char*)aux, "%u : %u : 0", config_hora, config_minutero);
		oled->OLED_Clear();
		oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 1", 2);
		oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"CONFIG: HOUR", 2);
		oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
		oled->OLED_Refresh();

		if(tecla == UP){
			config_hora ++;
			if(config_hora > 23){
				config_hora = 1;
			}
		}
		else if(tecla == DOWN){
			config_hora --;
			if(config_hora <= 0){
				config_hora = 23;
			}
		}
		else if(tecla  == OK ){
			rtc->SetHour(config_hora);
			rtc->SetMin(config_minutero);
			estados = HORA_ACTUAL;
		}
		else if(tecla  == BACK){
			estados = HORA_ACTUAL;
		}
		else if(tecla  == RIGHT ){
			estados = CONFIG_MINUTOS_CLOCK;
		}
		break;
	case CLIMA:
		checkPuerto();
		DisplayClima();
		if(tecla  == LEFT){
			estados = HORA_ACTUAL;
		}
		break;
	case TEMPORIZADOR:
		checkPuerto();
		if(tecla  == LEFT){
			config_temp_minuto = m_Tempo_minutos;
			config_temp_segundo = m_Tempo_segundos;
			estados = CONFIG_SEGUNDOS_TEMP;
		}
		else if(tecla  == OK){
			tiempoActual = (m_Tempo_minutos * 60 + m_Tempo_segundos);
			t_Temporizador = ((m_Tempo_minutos * 60 + m_Tempo_segundos) * 2) + (tiempoActual / 4); // Ajuste aproximado de tiempo real
			estados = TIMER_ON;
		}
		else if(tecla  == RIGHT){
			DisplayPomodoro();
			estados = POMODORO;
		}
		else if(tecla  == UP){
			DisplayAlarma();
			estados = ALARMA;
		}
		else if(tecla  == DOWN){
			DisplayHour();
			estados = HORA_ACTUAL;
		}
		break;
	case CONFIG_SEGUNDOS_TEMP:
		sprintf((char*)aux, "    %u : %u", config_temp_minuto, config_temp_segundo);
		oled->OLED_Clear();
		oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 2", 2);
		oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"TEMP: SEGS", 2);
		oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
		oled->OLED_Refresh();

		if (tecla  == UP) {
			config_temp_segundo++;
			if(config_temp_segundo > 59){
				config_temp_segundo = 1;
			}
		} else if (tecla  == DOWN) {
			config_temp_segundo--;
			if(config_temp_segundo <= 0){
				config_temp_segundo = 59;
			}
		} else if (tecla  == LEFT) {
			estados = CONFIG_MINUTOS_TEMP;
		} else if (tecla  == RIGHT || tecla  == BACK) {
			DisplayTemporizador();
			estados = TEMPORIZADOR;
		} else if (tecla  == OK) {
			m_Tempo_segundos = config_temp_segundo;
			DisplayTemporizador();
			estados = TEMPORIZADOR;
		}
		break;
	case CONFIG_MINUTOS_TEMP:
		sprintf((char*)aux, "    %u : %u", config_temp_minuto, config_temp_segundo);
		oled->OLED_Clear();
		oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 2", 2);
		oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"TEMP: MIN", 2);
		oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
		oled->OLED_Refresh();

		if (tecla  == UP) {
			config_temp_minuto++;
			if(config_temp_minuto > 59){
				config_temp_minuto = 1;
			}
		} else if (tecla  == DOWN) {
			config_temp_minuto--;
			if(config_temp_minuto <= 0){
				config_temp_minuto = 59;
			}
		} else if (tecla  == OK) {
			m_Tempo_segundos = config_temp_segundo;
			m_Tempo_minutos = config_temp_minuto;
			DisplayTemporizador();
			estados = TEMPORIZADOR;
		} else if (tecla  == BACK) {
			DisplayTemporizador();
			estados = TEMPORIZADOR;
		} else if (tecla  == RIGHT) {
			estados = CONFIG_SEGUNDOS_TEMP;
		}
		break;
	case TIMER_ON:
		rtc->Clock(clock);
		if(tiempoActual_aux != clock[2]){
			tiempoActual_aux = clock[2];
			tiempoActual--;
		}

		sprintf((char*)aux, "    %d : %d", (tiempoActual / 60), (tiempoActual % 60));
		oled->OLED_Clear();
		oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 2", 2);
		oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"TIMER ON", 2);
		oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
		oled->OLED_Refresh();

		if(t_Temporizador){
			buzzerPIP();
			DisplayTemporizador();
			estados = TEMPORIZADOR;
		}
		else if(tecla  == BACK){
			t_Temporizador.TimerStop();
			DisplayTemporizador();
			estados = TEMPORIZADOR;
		}
		else if(tecla  == OK){
			t_Temporizador.StandByTimer(Timer::PAUSE);
			oled->OLED_Clear();
			oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 2", 2);
			oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"TIMER PAUSE", 2);
			oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
			oled->OLED_Refresh();
			estados = TIMER_PAUSE;
		}
		break;
	case TIMER_PAUSE:
		if(tecla  == OK){
			t_Temporizador.StandByTimer(Timer::RUN);
			estados = TIMER_ON;
		}
		else if(tecla  == BACK){
			t_Temporizador.TimerStop();
			DisplayTemporizador();
			estados = TEMPORIZADOR;
		}
		break;
	case POMODORO:
		if(tecla  == OK){
			//Falta leer tiempos desde UART
			PomodoroActual = (m_Work_minutos* 60) + (m_Work_segundos);
			t_PomodoroWork = (((m_Work_minutos* 60) + (m_Work_segundos))* 2) + (PomodoroActual / 4); // Ajuste aproximado de tiempo real
			estados = WORK_ON;
		}
		else if(tecla  == LEFT){
			DisplayTemporizador();
			estados = TEMPORIZADOR;
		}
		break;
	case WORK_ON:
		rtc->Clock(clock);
		if(PomodoroActual_aux != clock[2]){
			PomodoroActual_aux = clock[2];
			PomodoroActual--;
		}

		sprintf((char*)aux, "    %d : %d", (PomodoroActual / 60), (PomodoroActual % 60));
		oled->OLED_Clear();
		oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 2", 2);
		oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"PMD WORK ON", 2);
		oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
		oled->OLED_Refresh();

		if(t_PomodoroWork){
			buzzerPIP();
			PomodoroActual = (m_Break_minutos * 60) + m_Break_segundos;
			t_PomodoroBreak = (((m_Break_minutos * 60) + m_Break_segundos)* 2) + (PomodoroActual / 4); // Ajuste aproximado de tiempo real
			estados = BREAK_ON;
		}
		else if(tecla  == OK){
			flagPomodoroWork = true;
			t_PomodoroWork.StandByTimer(Timer::PAUSE);
			estados = POMODORO_PAUSE;
		}
		else if (tecla == BACK){
			t_PomodoroBreak.TimerStop();
			t_PomodoroWork.TimerStop();
			DisplayPomodoro();
			estados = POMODORO;
		}
		break;
	case BREAK_ON:
		rtc->Clock(clock);
		if(PomodoroActual_aux != clock[2]){
			PomodoroActual_aux = clock[2];
			PomodoroActual--;
		}

		sprintf((char*)aux, "    %d : %d", (PomodoroActual / 60), (PomodoroActual % 60));
		oled->OLED_Clear();
		oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 2", 2);
		oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"PMD BREAK ON", 2);
		oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
		oled->OLED_Refresh();

		if(t_PomodoroBreak){
			buzzerPIP();
			PomodoroActual = (m_Work_minutos* 60) + (m_Work_segundos);
			t_PomodoroWork = (((m_Work_minutos* 60) + (m_Work_segundos))* 2) + (PomodoroActual / 4); // Ajuste aproximado de tiempo real
			estados = WORK_ON;
		}
		else if(tecla == OK){
			flagPomodoroWork = false;
			t_PomodoroBreak.StandByTimer(Timer::PAUSE);
			estados = POMODORO_PAUSE;
		}
		else if (tecla == BACK){
			t_PomodoroBreak.TimerStop();
			t_PomodoroWork.TimerStop();
			DisplayPomodoro();
			estados = POMODORO;
		}
		break;
	case POMODORO_PAUSE:
		if (tecla == OK){
			if(flagPomodoroWork){
				t_PomodoroWork.StandByTimer(Timer::RUN);
				estados = WORK_ON;
			}
			else{
				t_PomodoroBreak.StandByTimer(Timer::RUN);
				estados = BREAK_ON;
			}
		}
		break;
	case ALARMA:
		rtc->Clock(clock);
		if (tecla == LEFT) {
			config_alarma_hora = m_Alarma_Hora;
			config_alarma_minuto = m_Alarma_Minutos;
			estados = CONFIG_MINUTOS_ALARM;
		} else if (tecla == UP) {
			DisplayCronometro();
			estados = CRONOMETRO;
		}
		else if (tecla == OK){
			m_Alarma_Minutos = 0; // reseteo las variables por si m_Alarma_Activada = 0
			m_Alarma_Hora = 0;
			g_LED_AMANECER.clr(); // reseteo LED AMANECER
			g_BUZZER.clr(); // reseteo BUZZER
		}
		else if (tecla == DOWN){
			DisplayTemporizador();
			estados = TEMPORIZADOR;
		}
		else if(clock[1] == m_Alarma_Minutos && clock[0] == m_Alarma_Hora && m_Alarma_Activada){
			if(m_Buzzer_Activado){
				g_BUZZER.set();
			}
			if(m_Amanecer_Activado){
				g_LED_AMANECER.set();
			}
			m_Alarma_Minutos = 0; // reseteo las variables
			m_Alarma_Hora = 0;
		}
		break;
	case CONFIG_MINUTOS_ALARM:
		sprintf((char*)aux, "    %u : %u", config_alarma_hora, config_alarma_minuto);
		oled->OLED_Clear();
		oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 3", 2);
		oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"ALARMA: MIN", 2);
		oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
		oled->OLED_Refresh();

		if (tecla == UP) {
			config_alarma_minuto++;
			if(config_alarma_minuto > 59){
				config_alarma_minuto = 1;
			}
		}
		else if (tecla == DOWN) {
			config_alarma_minuto--;
			if(config_alarma_minuto <= 0){
				config_alarma_minuto = 59;
			}
		}
		else if (tecla == LEFT) {
			m_Alarma_Activada = 1;
			estados = CONFIG_HORA_ALARM;
		}
		else if (tecla == RIGHT || tecla == BACK) {
			DisplayAlarma();
			estados = ALARMA;
		} else if (tecla == OK) {
			m_Alarma_Minutos = config_alarma_minuto;
			m_Alarma_Hora = config_alarma_hora;

			DisplayAlarma();
			estados = ALARMA;
		}
		break;
	case CONFIG_HORA_ALARM:
		sprintf((char*)aux, "    %u : %u", config_alarma_hora, config_alarma_minuto);
		oled->OLED_Clear();
		oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 3", 2);
		oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"ALARMA: HOUR", 2);
		oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
		oled->OLED_Refresh();

		if(tecla == UP){
			config_alarma_hora ++;
			if(config_alarma_hora > 23){
				config_alarma_hora = 1;
			}
		}
		else if(tecla == DOWN){
			config_alarma_hora --;
			if(config_alarma_hora <= 0){
				config_alarma_hora = 23;
			}
		}
		else if(tecla == OK ){
			m_Alarma_Hora = config_alarma_hora;
			m_Alarma_Minutos = config_alarma_minuto;
			m_Alarma_Activada = 1;
			DisplayAlarma();
			estados = ALARMA;
		}
		else if(tecla == BACK){
			DisplayAlarma();
			estados = ALARMA;
		}
		else if(tecla == RIGHT ){
			estados = CONFIG_MINUTOS_ALARM;
		}
		break;
	case CRONOMETRO:
		if(tecla == OK){
			estados = CRONOMETRO_ON;
		}
		else if (tecla == DOWN) {
			DisplayAlarma();
			estados = ALARMA;
		}
		break;
	case CRONOMETRO_ON:
		rtc->Clock(clock);
		if(m_Cronometro_aux != clock[2]){
			m_Cronometro_aux = clock[2];
			m_Cronometro_segundos++;
			if(m_Cronometro_segundos > 59){
				m_Cronometro_segundos = 0;
				m_Cronometro_minutos++;
				if(m_Cronometro_minutos > 59){
					m_Cronometro_minutos = 0;
					m_Cronometro_horas++;
					if(m_Cronometro_horas > 23){
						m_Cronometro_horas = 0;
					}
				}
			}
		}

		sprintf((char*)aux, "%u : %u : %u", m_Cronometro_horas, m_Cronometro_minutos, m_Cronometro_segundos);
		oled->OLED_Clear();
		oled->OLED_Set_Text(0, 10, OLED::kOLED_Pixel_Set, (char *)"RELED MODO 4", 2);
		oled->OLED_Set_Text(0, 30, OLED::kOLED_Pixel_Set, (char *)"CRONOMETRO ON", 2);
		oled->OLED_Set_Text(0, 50, OLED::kOLED_Pixel_Set, (char*)aux, 2);
		oled->OLED_Refresh();

		if(tecla == OK){
			estados = CRONOMETRO_PAUSA;
		}
		else if(tecla == BACK){
			m_Cronometro_horas = 0;
			m_Cronometro_minutos = 0;
			m_Cronometro_segundos = 0;
			DisplayCronometro();
			estados = CRONOMETRO;
		}
		break;
	case CRONOMETRO_PAUSA:
		if(tecla == OK){
			estados = CRONOMETRO_ON;
		}
		else if(tecla == BACK){
			m_Cronometro_horas = 0;
			m_Cronometro_minutos = 0;
			m_Cronometro_segundos = 0;
			DisplayCronometro();
			estados = CRONOMETRO;
		}
		break;
	}
}

void Sheduller ( void )
{
	for ( uint8_t i = 0 ; i < PerifericoTemporizado::m_countPerifericosTemporizados  ; i ++ )
		g_perifericosTemporizados[i]->HandlerDelPeriferico();
}
