/*******************************************************************************************************************************//**
 *
 * @file		RTC.h
 * @brief		Clase RTC
 * @date		5/12/2024
 * @author		Grupo 2
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MODULO
 **********************************************************************************************************************************/
#ifndef SRC_RTC_RTC_H_
#define SRC_RTC_RTC_H_
/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "tipos.h"
#include "I2C.h"
#include "timer.h"
#include "intext.h"
#include "Gpio.h"

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/
#define RTC_ADDRESS 0x68
#define SECONDS 0x00
#define MINUTES 0x01
#define HOURS   0x02
#define DAY 	0x03
#define DATE	0x04
#define MONTH 	0x05
#define YEAR 	0x06

#define READCLOCK 0
#define READALARM 1
#define READALARM_H 1
#define READALARM_M 2
#define READALARM_S 3
#define MISSINGTIME 4
#define SETCLOCK 5
#define SETALARM 6


#define ALARM_1_SECONDS 0x07
#define ALARM_1_MINUTES 0x08
#define ALARM_1_HOURS   0x09
#define ALARM_1_DATE    0x0A

#define ALARM_2_MINUTES 0x0B
#define ALARM_2_HOURS   0x0C
#define ALARM_2_DATE    0x0D

#define CONTROL 	0x0E
#define STATUS 	    0x0F



#define WORDADDRESS  16
#define REQUESTFROM  17
#define READ         18
#define DELAY 	19

#define READ_FLAG 0x10
#define ALARM1_FLAG 0x1
#define ALARM2_FLAG 0x2
#define ALARM12_FLAG 0x3
#define NO_FLAG 0x4
#define NO_INT 0x5

#define ALARM_STATUS_REGISTER 0x0F


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
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** IMPLEMENTACION DE LA CLASE
 **********************************************************************************************************************************/
class RTC {
public:
	enum day_t {none, Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday};
	enum month_t {January, February, March, April, May, June, July, August, September, October, November, December};
private:
	enum error_t {m_error, m_ok};
	Intext* intextRTC;
	I2C* m_i2c;
	uint8_t m_slaveAddress;
	uint8_t m_century;
	Timer TimerWrite;
	bool EnabledAlarm1;
	bool EnabledAlarm2;
	bool EnabledInt;
public:
	enum alarm_t {alarm_1 = 1, alarm_2 = 2};
	RTC(I2C* i2c, uint8_t slaveAddress = RTC_ADDRESS, Intext* alarmInt = nullptr);
	bool Clock(uint8_t* clock);
	bool Calendar(uint8_t* calendar);

	bool SetupHour(uint8_t hour, uint8_t minute, uint8_t second);
	bool SetupCalendar(uint8_t date, uint8_t month, uint16_t year, day_t dayofweek = none, uint8_t century = 21);
	bool SetSegs(uint8_t sec);
	bool SetMin(uint8_t min);
	bool SetHour(uint8_t h);

	bool AlarmDisable(alarm_t alarm);
	bool AlarmReset(void);
	bool ReadAlarm(alarm_t alarm, uint8_t *bufferalarm);
	bool UpdateClockAndAlarm(uint8_t hour, uint8_t minute, uint8_t second);
	bool SetAlarmClock(uint8_t hour, uint8_t minute); //ALARM 2
	bool SetAlarmTo(uint8_t minutes, bool sec = false); //ALARM 1
	uint8_t Alarm(uint8_t& alarms); // 0x0 none, 0x1 Alarm1, 0x2 Alarm2, 0x3 Both

	void AlarmInterrupt(void);
	bool ReadFlags(uint8_t& flag);
	virtual ~RTC();
private:
	bool Read(uint8_t wordAdress, uint8_t* data);


	uint8_t SecondsToBCD(uint8_t seconds);
	uint8_t MinutesToBCD(uint8_t minutes);
	uint8_t HourToBCD(uint8_t hour);

	uint8_t DayToBCD(day_t day);
	uint8_t DateToBCD(uint8_t date);
	uint8_t MonthToBCD(uint8_t month);
	uint8_t YearToBCD(uint16_t year);

	uint8_t BCDToSeconds(uint8_t seconds);
	uint8_t BCDToMinutes(uint8_t minutes);
	uint8_t BCDToHour(uint8_t hour);

	day_t BCDToDay(uint8_t day);
	uint8_t BCDToDate(uint8_t date);
	uint8_t BCDToMonth(uint8_t month);
	uint8_t BCDToYear(uint8_t year);
};

#endif /* SRC_RTC_RTC_H_ */
