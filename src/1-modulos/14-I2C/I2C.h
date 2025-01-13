/*******************************************************************************************************************************//**
 *
 * @file		I2C.h
 * @brief		Clase I2C
 * @date		5/12/2024
 * @author		Grupo 2
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MODULO
 **********************************************************************************************************************************/
#ifndef SRC_I2C_I2C_H_
#define SRC_I2C_I2C_H_
/***********************************************************************************************************************************
 *** INCLUDES GLOBALES
 **********************************************************************************************************************************/
#include "LPC845.h"
#include "tipos.h"
#include "systick.h"
#include "Gpio.h"
#include "swm.h"


/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/
#if defined (__cplusplus)
	extern "C" {
	void I2C0_IRQHandler();
	void I2C1_IRQHandler();
	void I2C2_IRQHandler();
	void I2C3_IRQHandler();
	} // Hay que avisarle al C++ que esto viene del C porque si no se enoja
#endif

/***********************************************************************************************************************************
 *** DEFINES GLOBALES
 **********************************************************************************************************************************/
#define READ_BUFFER_SIZE_I2C 64
#define WRITE_BUFFER_SIZE_I2C 64

#define CLK_DIV 29
#define MSTSCLHIGH 6
#define MSTSCLLOW 6
/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** PROTOTIPOS DE FUNCIONES GLOBALES
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** IMPLEMENTACION DE LA CLASE
 **********************************************************************************************************************************/
class I2C
{
	public:
		enum i2c_t {i2c0 = 0, i2c1, i2c2, i2c3};

	private:
		// Flag para saber si esta escribiendo o leyendo
		bool m_FlagWrite;
		bool m_FlagRead;

		// Modo, Direccion y cual I2C es
		I2C_Type *m_I2C;
		i2c_t m_i2c;

		// Errores de configuracion o transmision
		bool m_Error;
		bool m_NACK;

		// Cantidad de Bytes que queremos leer
		uint8_t m_Bytes;
		uint8_t m_Size;
		uint8_t m_SlaveAddress;
		uint8_t m_regAddress;

		// Puerto y pin donde esta configurado el I2C
		uint8_t m_PortSCL;
		uint8_t m_PinSCL;
		uint8_t m_PortSDA;
		uint8_t m_PinSDA;

		// Buffer de Lectura
		uint8_t  m_BufferRead[READ_BUFFER_SIZE_I2C];
		uint16_t m_IndexReadIn;
		uint16_t m_IndexReadOut;

		// Buffer de Escritura
		uint8_t  m_BufferWrite[WRITE_BUFFER_SIZE_I2C];
		uint16_t m_IndexWriteIn;
		uint16_t m_IndexWriteOut;

		// Escritura o lectura
		void IntRead(bool last = false);
		void IntWrite();
		// Interrupcion
		void DisableI2CPendingInterrupt();
		void EnableI2CPendingInterrupt();

		// Metodos de buffers
		bool PopRead(uint8_t* data);
		void PushRead(uint8_t data);
		bool PopWrite(uint8_t* data);
		void PushWrite(uint8_t data);

		// Disparos de escritura o lectura
		void StartBitRead();
		void StartBitWrite();

	public:
		// Constructor
		I2C(i2c_t i2c = i2c0, uint8_t portSCL = 0, uint8_t pinSCL = 10, uint8_t portSDA = 0, uint8_t pinSDA = 11);
		virtual ~I2C();

		// Funcion de interrupcion
		void IRQHandler();

		// Funciones de escritura y lectura
		bool Write(uint8_t slaveAddress, uint8_t* message, uint16_t size);
		bool Write(uint8_t slaveAddress, uint8_t message);
		bool WriteWordAddress(uint8_t slaveAddress, uint8_t wordAddress);
		void WriteBlocking(uint8_t slaveAddress, uint8_t RegAddress,uint8_t *txBuff, uint32_t txSize);
		bool requestFrom(uint8_t slaveAddress, uint8_t bytes);
		bool requestFrom(uint8_t slaveAddress);
		bool Read(uint8_t* message);

		//Metodos que verifican el estado del I2C
		bool I2CIsIdle() { return (!m_FlagRead && !m_FlagWrite); }
		bool I2CIsReading() { return m_FlagRead; }
		bool I2CIsWriting() { return m_FlagWrite; }
		bool HadNACK() { return m_NACK; }

};

#endif

