/*******************************************************************************************************************************//**
 *
 * @file		RTC.cpp
 * @brief		Metodos y constructor de la clase RTC
 * @date		5/12/2024
 * @author		Grupo 2
 *
 *
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include <RTC.h>

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPOS DE DATOS PRIVADOS AL MODULO
 **********************************************************************************************************************************/
static bool FlagInt = false;

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
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** IMPLEMENTACION DE LOS METODOS DE LA CLASE
 **********************************************************************************************************************************/

// Constructor del RTC, retorna void
RTC::RTC(I2C* i2c, uint8_t slaveAddress, Intext* alarmInt) : TimerWrite(Timer::MS ,nullptr)
{
	m_i2c = i2c;
	m_slaveAddress = slaveAddress;
	EnabledAlarm1 = true; //El estado de la alarma es desconocido, se supone activada.
	EnabledAlarm2 = true;
	EnabledInt = false;

	if(alarmInt != nullptr) //Puede utilizarse por interrupcion.
	{
		intextRTC = alarmInt;
		EnabledInt = true;

	}
}

// Funcion de lectura de datos del RTC, retorna bool de verificacion
bool RTC::Read(uint8_t wordAddress, uint8_t* data)
{
	static uint8_t state = DELAY;
	static uint8_t address = 0;
	uint8_t aux = 0;

	if(m_i2c->I2CIsIdle())
	{
		switch(state)
		{
		case DELAY:
			TimerWrite.TimerStart(1);
			state = WORDADDRESS;
			break;
		case WORDADDRESS:
			if(!TimerWrite)
			{
				address = wordAddress;
				m_i2c->WriteWordAddress(m_slaveAddress, address);
				TimerWrite.TimerStart(1);
				state = REQUESTFROM;
			}
			break;
		case REQUESTFROM:
			if(!TimerWrite)
			{
				m_i2c->requestFrom(m_slaveAddress, 1);
				state = READ;
			}
			break;
		case READ:
			if(m_i2c->Read(&aux))
			{
				*data = aux;
				state = DELAY;
				address = 0;
				return true;
			}
			break;
		}
	}
	return false;
}

// Lee la hora, minutos y segundos del RTC, usa puntero al buffer de datos del horario, retorna bool de verificacion
bool RTC::Clock(uint8_t* clock) //Lee el reloj del RTC.
{
	static uint8_t state = HOURS;
	uint8_t data = 0;

	switch(state)
	{
	case HOURS:
		if(Read(HOURS, &data))
		{
			clock[0] = BCDToHour(data);
			state = MINUTES;
		}
		break;
	case MINUTES:
		if(Read(MINUTES, &data))
		{
			clock[1] = BCDToMinutes(data);
			state = SECONDS;
		}
		break;
	case SECONDS:
		if(Read(SECONDS, &data))
		{
			clock[2] = BCDToSeconds(data);
			state = HOURS;
			return  true;
		}
		break;
	}
	return false;
}

// Lee la fecha del RTC (dia, mes y año), usa puntero al buffer de datos de la fecha, retorna bool de verificacion
bool RTC::Calendar(uint8_t* calendar) // Lee el calendario del RTC.
{
	static uint8_t state = DATE;
	uint8_t data = 0;

	switch(state)
	{
	case DATE:
		if(Read(DATE, &data))
		{
			calendar[0] = BCDToDate(data);
			state = MONTH;
		}
		break;
	case MONTH:
		if(Read(MONTH, &data))
		{
			calendar[1] = BCDToMonth(data);
			state = YEAR;
		}
		break;
	case YEAR:
		if(Read(YEAR, &data))
		{
			calendar[2] = BCDToYear(data); // + (m_century - 1)*100;
			state = DATE;
			return true;
		}
		break;
	}
	return false;
}

// Setea la hora en el RTC, usa hora, minutos y segundos a setear, retorna bool de verificacion
bool RTC::SetupHour(uint8_t h, uint8_t min, uint8_t sec) // Configuramos el reloj.
{

	bool salida = false;

	uint8_t seconds = SecondsToBCD(sec); // Lo pasamos a BCD.

	uint8_t minutes = MinutesToBCD(min);

	uint8_t hour = HourToBCD(h);

	uint8_t buffer_segundos[] = {SECONDS, seconds};

	uint8_t buffer_minutos[] = {MINUTES, minutes};

	uint8_t buffer_horas[] = {HOURS, hour};

	if(m_i2c->I2CIsIdle())
	{
		m_i2c->Write(RTC_ADDRESS, buffer_segundos, sizeof(buffer_segundos));
		for(int32_t i = 1000; i > 0 ; i--){}
		m_i2c->Write(RTC_ADDRESS, buffer_minutos, sizeof(buffer_minutos));
		for(int32_t i = 1000; i > 0 ; i--){}
		m_i2c->Write(RTC_ADDRESS, buffer_horas, sizeof(buffer_horas));

		salida = true;
	}
	return salida;
}

// Setea segundos del RTC, retorna bool de verificacion
bool RTC::SetSegs(uint8_t sec){

	bool salida = false;
	uint8_t seconds = SecondsToBCD(sec);
	uint8_t buffer_segundos[] = {SECONDS, seconds};
	if(m_i2c->I2CIsIdle()){
		m_i2c->Write(RTC_ADDRESS, buffer_segundos, sizeof(buffer_segundos));
		for(int32_t i = 1000; i > 0 ; i--){}
		salida = true;
	}
	return salida;
}

// Setea minutos del RTC, retorna bool de verificacion
bool RTC::SetMin(uint8_t min){
	bool salida = false;
	uint8_t minutes = MinutesToBCD(min);
	uint8_t buffer_minutos[] = {MINUTES, minutes};
	if(m_i2c->I2CIsIdle()){
		m_i2c->Write(RTC_ADDRESS, buffer_minutos, sizeof(buffer_minutos));
		for(int32_t i = 1000; i > 0 ; i--){}
		salida = true;
	}
	return salida;
}

// Setea horas del RTC, retorna bool de verificacion
bool RTC::SetHour(uint8_t h){
	bool salida = false;
	uint8_t hour = HourToBCD(h);
	uint8_t buffer_horas[] = {HOURS, hour};
	if(m_i2c->I2CIsIdle()){
		m_i2c->Write(RTC_ADDRESS, buffer_horas, sizeof(buffer_horas));
		for(int32_t i = 1000; i > 0 ; i--){}
		salida = true;
	}
	return salida;
}

// Setea la fecha en el RTC, usa dia, mes y año a setear, retorna bool de verificacion
bool RTC::SetupCalendar(uint8_t d, uint8_t m, uint16_t y, day_t dayofweek, uint8_t century) //Configuramos el Calendario.
{
	static uint8_t state = DATE;
	m_century = century;
	bool setDow = false;
	uint8_t buffer[2];

	uint8_t date = 0;
	uint8_t month = 0;
	uint8_t year = 0;
	uint8_t day = 0;

	date = DateToBCD(d);
	month = MonthToBCD(m);
	year = YearToBCD(y);

	if(dayofweek != none)//Si enviamos el dia de la semana, tambien se configura.
	{
		setDow = true;
		day = DayToBCD(dayofweek);
	}

	if(m_i2c->I2CIsIdle())
	{
		switch(state)
		{
		case DATE:
			buffer[0] = DATE;// Primero el address, luego el dato.
			buffer[1] = date;
			m_i2c->Write(m_slaveAddress, buffer, 2);
			state = MONTH;
			break;
		case MONTH:
			buffer[0] = MONTH;
			buffer[1] = month;
			m_i2c->Write(m_slaveAddress, buffer, 2);
			state = YEAR;
			break;
		case YEAR:
			buffer[0] = YEAR;
			buffer[1] = year;
			m_i2c->Write(m_slaveAddress, buffer, 2);
			if(setDow)
			{
				state = DAY;
			}
			else
			{
				state = DATE;
				return true;
			}
			break;
		case DAY:
			buffer[0] = DAY;
			buffer[1] = day;
			m_i2c->Write(m_slaveAddress, buffer, 2);
			state = DATE;
			return true;
			break;
		}
	}
	return false;
}

// Desactiva las alarmas del RTC, retorna bool de verificacion
bool RTC::AlarmReset(void) // Se resetean ambas alarmas.
{
	static uint8_t state = 0;
	switch(state)
	{
	case 0:
		if(AlarmDisable(alarm_1))
			state = 1;
		break;
	case 1:
		if(AlarmDisable(alarm_2))
		{
			state = 0;
			return true;
		}
		break;
	}
	return false;
}

// Lee la flag de alarma del RTC y segun cual o cuales esten activadas las desactiva, retorna bool de verificacion
bool RTC::AlarmDisable(alarm_t alarm)
{
	static uint8_t state = STATUS;
	bool* alarmSel = nullptr;
	uint8_t buffer[2];
	uint8_t data = 0;

	switch(alarm)
	{
	case alarm_1:
		alarmSel = &EnabledAlarm1;
		break;
	case alarm_2:
		alarmSel = &EnabledAlarm2;
		break;
	}

	if(m_i2c->I2CIsIdle() && *(alarmSel))
	{
		switch(state)
		{
		case CONTROL:
			if(Read(CONTROL, &data)) // Desactivamos la interrupcion de la alarma.
			{
				switch(alarm)
				{
				case alarm_1:
					data &= ~(1 << 0);
					break;
				case alarm_2:
					data &= ~(1 << 1);
					break;
				}
				buffer[0] = CONTROL;
				buffer[1] = data;
				m_i2c->Write(m_slaveAddress, buffer, 2);
				state = STATUS;
			}
			break;
		case STATUS:
			if(Read(STATUS, &data)) // Limpiamos la flag de la alarma.
			{
				switch(alarm)
				{
				case alarm_1:
					data &= ~(1 << 0);
					break;
				case alarm_2:
					data &= ~(1 << 1);
					break;
				}
				buffer[0] = STATUS;
				buffer[1] = data;
				m_i2c->Write(m_slaveAddress, buffer, 2);
				*alarmSel = false;
				state = CONTROL;
			}
			break;
		}
	}
	return !(*alarmSel);
}

// Manda en un buffer la hora en la que esta seteada la alarma, retorna bool de verificacion
bool RTC::ReadAlarm(alarm_t alarm, uint8_t *bufferalarm) // Lee el valor de la alarma.
{
	static uint8_t state = READALARM_H;

	if(m_i2c->I2CIsIdle())
	{
		if (alarm == alarm_1)
		{
			switch(state)
			{
			case READALARM_H:
				if(Read((uint8_t) ALARM_1_HOURS, &bufferalarm[0]))
				{
					bufferalarm[0] &= ~ (1<<7); //Elimino el seteo del bit 7
					bufferalarm[0] = BCDToHour(bufferalarm[0]);

					state = READALARM_M;
				}
				break;
			case READALARM_M:
				if(Read((uint8_t) ALARM_1_MINUTES, &bufferalarm[1]))
				{
					bufferalarm[1] &= ~(1<<7); //Elimino el seteo del bit 7
					bufferalarm[1] = BCDToMinutes(bufferalarm[1]);

					state = READALARM_S;
				}
				break;
			case READALARM_S:
				if(Read((uint8_t) ALARM_1_SECONDS, &bufferalarm[2]))
				{
					bufferalarm[2] &= ~(1<<7); //Elimino el seteo del bit 7
					bufferalarm[2] = BCDToSeconds(bufferalarm[2]);
					state = READALARM_H;
					return true;
				}
				break;
			}
		}

		if (alarm == alarm_2)
		{
			switch(state)
			{

			case READALARM_H:
				if(Read((uint8_t) ALARM_2_HOURS, &bufferalarm[0]))
				{
					bufferalarm[0] &= ~(1<<7); //Elimino el seteo del bit 7
					bufferalarm[0] = BCDToHour(bufferalarm[0]);

					state = READALARM_M;
				}
				break;
			case READALARM_M:
				if(Read((uint8_t) ALARM_2_MINUTES, &bufferalarm[1]))
				{
					bufferalarm[1] &= ~(1<<7); //Elimino el seteo del bit 7
					bufferalarm[1] = BCDToMinutes(bufferalarm[1]);

					state = READALARM_H;
					return true;
				}
				break;
			}
		}
	}
	return false;
}

// Actualiza la hora y la alarma_1 del RTC, retorna bool de verificacion
bool RTC::UpdateClockAndAlarm(uint8_t h, uint8_t m, uint8_t s) // Actualiza el reloj y la alarma considerando el tiempo restante.
{
	static uint8_t state = READCLOCK;
	static uint8_t bufferpastclock[3];
	static uint8_t bufferpastalarm[3];
	static uint16_t alarm;

	if(m_i2c->I2CIsIdle())
	{
		switch(state)
		{
		case READCLOCK:
			if(Clock(bufferpastclock))
			{
				state = READALARM;
			}
			break;
		case READALARM:
			if(ReadAlarm(alarm_1, bufferpastalarm))
			{
				state = MISSINGTIME;
			}
			break;
		case MISSINGTIME: // Lo pasamos a segundos.
			if((bufferpastalarm[0]*60*60 + bufferpastalarm[1]*60 + bufferpastalarm[2]) >= (bufferpastclock[0]*60*60 + bufferpastclock[1]*60 + bufferpastclock[0]))
			{
				alarm = (bufferpastalarm[0]*60*60 + bufferpastalarm[1]*60 + bufferpastalarm[2])  - (bufferpastclock[0]*60*60 + bufferpastclock[1]*60 + bufferpastclock[0]) ;
			}
			else
			{
				alarm = (24*60*60 - (bufferpastclock[0]*60*60 + bufferpastclock[1]*60 + bufferpastclock[0])) + (bufferpastalarm[0]*60*60 + bufferpastalarm[1]*60 + bufferpastalarm[2]);
			}
			state = SETCLOCK;
			break;
		case SETCLOCK:
			if(SetupHour(h, m, s))
			{
				state = SETALARM;
			}
			break;
		case SETALARM:
			if(SetAlarmTo(alarm, true)) // La alarma sonara dentro de alarms segundos.
			{
				state = READCLOCK;
				return true;
			}
			break;
		}
	}
	return false;
}

// Setea la alarma_2 en esa hora y minuto, retorna bool de verificacion
bool RTC::SetAlarmClock(uint8_t h, uint8_t min) //Seteamos la alarma 2 para que "suene" a determinada hora.
{
	static uint8_t state = MINUTES;
	uint8_t data = 0;
	static uint8_t buffer[2];

	static uint8_t minutes = 0;
	static 	uint8_t hour = 0;

	if(AlarmDisable(alarm_2))
	{
		minutes = MinutesToBCD(min);
		hour = HourToBCD(h);

		if(m_i2c->I2CIsIdle())
		{
			switch(state)
			{
			case MINUTES:// Los A2Mx son para setear para que suene cuando las horas y minutos coinciden.
				buffer[0] = ALARM_2_MINUTES;
				buffer[1] = (minutes & ~(1 << 7)); // A2M2 0
				m_i2c->Write(m_slaveAddress, buffer, 2);
				state = HOURS;
				break;
			case HOURS:
				buffer[0] = ALARM_2_HOURS;
				buffer[1] = (hour & ~(1 << 7)); // A2M3 0
				m_i2c->Write(m_slaveAddress, buffer, 2);
				state = DATE;
				break;
			case DATE:
				buffer[0] = ALARM_2_DATE;
				buffer[1] = (1 << 7); // A2M4 1
				m_i2c->Write(m_slaveAddress, buffer, 2);
				state = CONTROL;
				break;
			case CONTROL:
				if(Read(CONTROL, &data))
				{
					buffer[0] = CONTROL;
					buffer[1] = (data | (1 << 1) | (1 << 2)); // Se activa la alarma.
					m_i2c->Write(m_slaveAddress, buffer, 2);
					EnabledAlarm2 = true;
					state = MINUTES;
					return true;
				}
				break;
			}
		}
	}
	return false;
}

// Setea la alarma_1 minutos o segundos despues del actual horario, retorna bool de verificacion
bool RTC::SetAlarmTo(uint8_t minsec, bool sec)
{
	static uint8_t data = 0;
	static uint8_t state = READ;
	uint8_t buffer[2];
	static  uint8_t clock[3] = {0, 0, 0};
	static	uint8_t seconds = 0;
	static	uint8_t minutes = 0;
	static	uint8_t hour = 0;

	if(AlarmDisable(alarm_1))
	{

		if(m_i2c->I2CIsIdle())
		{
			switch(state)
			{
			case READ:
				if(Clock(clock))//Lee la hora para setear la alarma
				{
					if(sec)// Depende si seteamos para minutos o segundos.
					{
						hour = minsec / 3600;
						minsec %= 3600;
						minutes = minsec / 60;
						minsec %= 60;
						seconds = minsec;
					}
					else
					{
						hour = (unsigned int) minsec / 60;
						minsec %= 60;
						minutes = minsec;
						seconds = 0;
					}

					clock[2] += seconds; // Calculamos que hora seria dentro de "minsec" segundos/minutos.
					if(clock[2] >= 60)
					{
						minutes++;
						clock[2] %= 60;
					}
					clock[1] += minutes;
					if(clock[1] >= 60)
					{
						hour++;
						clock[1] %= 60;
						clock[0] += hour;
						clock[0] %= 24; //00 a 23 horas
					}

					minutes = MinutesToBCD(clock[1]);
					hour = HourToBCD(clock[0]);
					seconds = SecondsToBCD(clock[2]);

					state = HOURS;
				}
				break;
			case SECONDS:
				buffer[0] = ALARM_1_SECONDS;
				buffer[1] = (seconds & ~(1 << 7)); //A1M1
				m_i2c->Write(m_slaveAddress, buffer, 2);
				state = DATE;
				break;
			case MINUTES:
				buffer[0] = ALARM_1_MINUTES;
				buffer[1] = (minutes & ~(1 << 7)); //A1M2
				m_i2c->Write(m_slaveAddress, buffer, 2);
				state = SECONDS;
				break;
			case HOURS:
				buffer[0] = ALARM_1_HOURS;
				buffer[1] = (hour & ~(1 << 7));//A1M3
				m_i2c->Write(m_slaveAddress, buffer, 2);
				state = MINUTES;
				break;
			case DATE:
				buffer[0] = ALARM_1_DATE;
				buffer[1] = (1 << 7); //A1M4
				m_i2c->Write(m_slaveAddress, buffer, 2);
				state = CONTROL;
				break;
			case CONTROL:
				if(Read(CONTROL, &data))
				{
					buffer[0] = CONTROL;
					buffer[1] = (data | (1 << 0) | (1 << 2));
					m_i2c->Write(m_slaveAddress, buffer, 2);
					EnabledAlarm1 = true;
					state = READ;
					return true;

				}
				break;

			}
		}
	}

	return false;
}

// Lee las flags de alarma, retorna bool de verificacion
bool RTC::ReadFlags(uint8_t& flag)
{
	uint8_t data = 0;
	flag = 0x0;

	if(m_i2c->I2CIsIdle())
	{
		if(Read(STATUS, &data))
		{
			data &= 0x3;
			flag = data;
			if(flag == 0x0)
			{
				flag = 0x4;
			}
			return true;
		}
	}
	return false;
}

// Si saltaron las alarmas se encarga de desactivarlas, si no, no hace nada, retorna que alarma salto o si no hay interrupcion
uint8_t RTC::Alarm(uint8_t& alarms)
{
	static uint8_t state = READ_FLAG;
	static uint8_t flag = 0x0;

	if(EnabledInt && !FlagInt) // Si estamos usando la interrupcion externa y no hay flag retorna rapidamente.
	{
		alarms = NO_INT;
		return NO_INT;
	}

	switch(state)
	{
	case READ_FLAG:
		if(ReadFlags(flag))
			state = flag;
		break;
	case ALARM1_FLAG:
		if(AlarmDisable(alarm_1))
		{
			alarms = ALARM1_FLAG;
			if(EnabledInt) // Desactivamos la flag de interrupcion externa.
				FlagInt = false;
			state = READ_FLAG;
			return ALARM1_FLAG;
		}
		break;
	case ALARM2_FLAG:
		if(AlarmDisable(alarm_2))
		{
			alarms = ALARM2_FLAG;
			if(EnabledInt)
				FlagInt = false;
			state = READ_FLAG;
			return ALARM2_FLAG;
		}
		break;
	case ALARM12_FLAG: // Si suenan ambas alarmas al mismo tiempo.
		if(AlarmDisable(alarm_1) && AlarmDisable(alarm_2))
		{
			alarms = ALARM12_FLAG;
			if(EnabledInt)
				FlagInt = false;
			state = READ_FLAG;
			return ALARM12_FLAG;
		}
		break;
	case NO_FLAG:
		if(EnabledInt)
			FlagInt = false;
		state = READ_FLAG;
		alarms = NO_FLAG;
		return NO_FLAG;
		break;
	}
	return 0x0;
}

// Si salto la alarma activa la flag de interrupcion, retorna void
void RTC::AlarmInterrupt(void)
{
	FlagInt = true;
}

// Pasa de segundos a codigo BCD, retorna los segundos (uint8_t)
uint8_t RTC::SecondsToBCD(uint8_t seconds)
{
	uint8_t aux = 0;
	uint8_t ret = 0;

	if(seconds <= 59)
	{
		aux = seconds / 10;
		ret |= (aux << 4) & 0x70;
		aux = seconds % 10;
		ret |= (aux) & 0xF;
	}
	return ret;
}

// Pasa de minutos a codigo BCD, retorna los minutos (uint8_t)
uint8_t RTC::MinutesToBCD(uint8_t minutes)
{
	uint8_t aux = 0;
	uint8_t ret = 0;

	if(minutes <= 59)
	{
		aux = minutes / 10;
		ret |= (aux << 4) & 0x70;
		aux = minutes % 10;
		ret |= (aux) & 0xF;
	}
	return ret;
}

// Pasa de horas a codigo BCD, retorna los horas (uint8_t)
uint8_t RTC::HourToBCD(uint8_t hour)
{
	uint8_t aux = 0;
	uint8_t ret = 0;

	if(hour < 24)
	{
		aux = hour / 10;
		ret |= (aux << 4);

		switch(aux)
		{
			case 0:
			case 1:
				aux = hour % 10;
				break;
			case 2:
				aux = hour % 20;
				break;
		}
		ret |= (aux) & 0xF;
	}
	return ret;
}

// Pasa de dias a codigo BCD, retorna los dias (uint8_t)
uint8_t RTC::DayToBCD(day_t day){return day;}

// Pasa de fecha a codigo BCD, retorna los fecha (uint8_t)
uint8_t RTC::DateToBCD(uint8_t date)
{
	uint8_t aux = 0;
	uint8_t ret = 0;

	if(date <= 31 && date >= 1)
	{
		aux = date / 10;
		ret |= (aux << 4) & 0x30;
		aux = date % 10;
		ret |= (aux) & 0xF;
	}
	return ret;
}

// Pasa de meses a codigo BCD, retorna los meses (uint8_t)
uint8_t RTC::MonthToBCD(uint8_t month)
{

	uint8_t aux = 0;
	uint8_t ret = 0;

	if(month <= 12 && month >= 1)
	{
		aux = month / 10;
		ret |= (aux << 4) & 0x10;
		aux = month % 10;
		ret |= (aux) & 0xF;
	}
	return ret;
}

// Pasa de años a codigo BCD, retorna los años (uint8_t)
uint8_t RTC::YearToBCD(uint16_t year)
{
	uint8_t aux = 0;
	uint8_t ret = 0;

	year %= 100;

	aux = year / 10;
	ret |= (aux << 4) & 0xF0;
	aux = year % 10;
	ret |= (aux) & 0xF;

	return ret;
}

// Pasa el BCD a segundos, retorna (uint8_t)
uint8_t RTC::BCDToSeconds(uint8_t seconds) {return ((seconds & 0x70) >> 4)*10 + (seconds &  0xF);}

// Pasa el BCD a minutos, retorna (uint8_t)
uint8_t RTC::BCDToMinutes(uint8_t minutes) {return ((minutes & 0x70) >> 4)*10 + (minutes &  0xF);}

// Pasa el BCD a horas, retorna (uint8_t)
uint8_t RTC::BCDToHour(uint8_t hour) {return ((hour & 0x30) >> 4)*10 + (hour & 0xF);}

// Pasa el BCD a dias, retorna (uint8_t)
RTC::day_t RTC::BCDToDay(uint8_t day)
{
	day &= 0x7;
	switch(day)
	{
	case 0:
		return none;
		break;
	case 1:
		return Sunday;
		break;
	case 2:
		return Monday;
		break;
	case 3:
		return Tuesday;
		break;
	case 4:
		return Wednesday;
		break;
	case 5:
		return Thursday;
		break;
	case 6:
		return Friday;
		break;
	case 7:
		return Saturday;
		break;
	}
	return none;
}

// // Pasa el BCD a fecha, retorna (uint8_t)
uint8_t RTC::BCDToDate(uint8_t date) {return ((date & 0x30) >> 4)*10 + (date & 0xF);}

// Pasa el BCD a mes, retorna (uint8_t)
uint8_t RTC::BCDToMonth(uint8_t month) {return ((month & 0x10) >> 4)*10 + (month & 0xF);}

// Pasa el BCD a año, retorna (uint8_t)
uint8_t RTC::BCDToYear(uint8_t year) {return ((year & 0xF0) >> 4)*10 + (year & 0xF);}

// Destructor de RTC
RTC::~RTC() {}
