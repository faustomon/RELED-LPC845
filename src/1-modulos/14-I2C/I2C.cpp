/*******************************************************************************************************************************//**
 *
 * @file		I2C.cpp
 * @brief		Metodos y constructor de la clase I2C
 * @date		5/12/2024
 * @author		Grupo 2
 *
 **********************************************************************************************************************************/
/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "I2C.h"
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
I2C* g_I2CVector[4] = { nullptr, nullptr, nullptr, nullptr };

/***********************************************************************************************************************************
 *** IMPLEMENTACION DE LOS METODOS DE LA CLASE
 **********************************************************************************************************************************/
// Constructor I2C que se encarga de configurarlo e inicializarlo, retorna void
I2C::I2C(i2c_t i2c, uint8_t portSCL, uint8_t pinSCL, uint8_t portSDA, uint8_t pinSDA)
: m_i2c(i2c), m_PortSCL(portSCL), m_PinSCL(pinSCL), m_PortSDA(portSDA), m_PinSDA(pinSDA)
{
	uint8_t Indice_PortPin1 ;
	uint8_t Indice_PortPin2 ;
	m_IndexReadIn =  0; // Indices en 0 de los buffer
	m_IndexReadOut = 0;
	m_IndexWriteIn = 0;
	m_IndexWriteOut = 0;
	m_FlagWrite = false;
	m_FlagRead = false;
	m_NACK = false;
	m_Bytes = 0;
	m_Size = 0;
	m_SlaveAddress = 0;

	// Chequeo de no sobreescritura, si se sobreescribio el i2c tira error
	m_Error = !((m_i2c == i2c0 && g_I2CVector[0] == nullptr) || (m_i2c == i2c1 && g_I2CVector[1] == nullptr)
			|| (m_i2c == i2c2 && g_I2CVector[2] == nullptr) || (m_i2c == i2c3 && g_I2CVector[3] == nullptr));

	if(!m_Error) // Si no hay error de sobreescritura procedemos a configurar
	{

		if(m_i2c == i2c0)
		{
			m_PortSCL = 0;
			m_PinSCL = 10;
			m_PortSDA = 0;
			m_PinSDA = 11;
		}

		switch(m_i2c)
		{
		case i2c0:
			m_I2C = I2C0; // I2C0
			break;
		case i2c1:
			m_I2C = I2C1; // I2C1
			break;
		case i2c2:
			m_I2C = I2C2; // I2C2
			break;
		case i2c3:
			m_I2C = I2C3; // I2C3
			break;
		}

		SYSCON->SYSAHBCLKCTRL0 |= (1 << 7); // SWM

		if(m_i2c == i2c0)
		{
			SYSCON->PRESETCTRL0 &= ~(1 << 5);
			SYSCON->PRESETCTRL0 &= ~(1 << 5);
			SYSCON->PRESETCTRL0 &= ~(1 << 5);
			SYSCON->PRESETCTRL0 |= (1 << 5);
			SYSCON->PRESETCTRL0 |= (1 << 5);
			SYSCON->PRESETCTRL0 |= (1 << 5);
		}
		else if(m_i2c == i2c1)
		{
			SYSCON->PRESETCTRL0 &= ~(1 << 21);
			SYSCON->PRESETCTRL0 &= ~(1 << 21);
			SYSCON->PRESETCTRL0 &= ~(1 << 21);
			SYSCON->PRESETCTRL0 |= (1 << 21);
			SYSCON->PRESETCTRL0 |= (1 << 21);
			SYSCON->PRESETCTRL0 |= (1 << 21);
		}
		else if(m_i2c == i2c2)
		{
			SYSCON->PRESETCTRL0 &= ~(1 << 22);
			SYSCON->PRESETCTRL0 &= ~(1 << 22);
			SYSCON->PRESETCTRL0 &= ~(1 << 22);
			SYSCON->PRESETCTRL0 |= (1 << 22);
			SYSCON->PRESETCTRL0 |= (1 << 22);
			SYSCON->PRESETCTRL0 |= (1 << 22);
		}
		else if(m_i2c == i2c3)
		{
			SYSCON->PRESETCTRL0 &= ~(1 << 23);
			SYSCON->PRESETCTRL0 &= ~(1 << 23);
			SYSCON->PRESETCTRL0 &= ~(1 << 23);
			SYSCON->PRESETCTRL0 |= (1 << 23);
			SYSCON->PRESETCTRL0 |= (1 << 23);
			SYSCON->PRESETCTRL0 |= (1 << 23);
		}

		if(m_i2c == i2c0)
		{
			SYSCON->FCLKSEL[5] = 1;
			SYSCON->SYSAHBCLKCTRL0 |= (1 << 5);
		}
		else if(m_i2c == i2c1)
		{
			SYSCON->FCLKSEL[6] = 1;
			SYSCON->SYSAHBCLKCTRL0 |= (1 << 21);
		}
		else if(m_i2c == i2c2)
		{
			SYSCON->FCLKSEL[7] = 1;
			SYSCON->SYSAHBCLKCTRL0 |= (1 << 22);
		}
		else if(m_i2c == i2c3)
		{
			SYSCON->FCLKSEL[8] = 1;
			SYSCON->SYSAHBCLKCTRL0 |= (1 << 23);
		}

		m_I2C->CLKDIV = CLK_DIV; // Pongo divisor de clock en 30
		m_I2C->MSTTIME = ((MSTSCLLOW << 0) & (0x7)) | ((MSTSCLHIGH << 4) & (0x70)); // Seteo SCL con ciclos de reloj de alto y bajo

		// Configuro los PINASSIGN y PINENABLE
		if(m_i2c == i2c0){
			PINENABLE_Config(PE_I2C0_SDA,1);
			PINENABLE_Config(PE_I2C0_SCL,1);
		}
		else if(m_i2c == i2c1){
			PINASSIGN_Config(PA_I2C1_SDA,m_PortSDA,m_PinSDA);
			PINASSIGN_Config(PA_I2C1_SCL,m_PortSCL,m_PinSCL);
		}
		else if(m_i2c == i2c2){
 			PINASSIGN_Config(PA_I2C2_SDA,m_PortSDA,m_PinSDA);
			PINASSIGN_Config(PA_I2C2_SCL,m_PortSCL,m_PinSCL);
		}
		else if(m_i2c == i2c3){
			PINASSIGN_Config(PA_I2C3_SDA,m_PortSDA,m_PinSDA);
			PINASSIGN_Config(PA_I2C3_SCL,m_PortSCL,m_PinSCL);
		}

		// Seteo OPENDRAIN en caso de no estar usando I2C0
		 if(m_i2c != i2c0)
		{
			 Indice_PortPin1 = IOCON_INDEX_PIO0[m_PinSCL];
			 Indice_PortPin2 = IOCON_INDEX_PIO0[m_PinSDA];
			 if ( m_PortSCL )
				 Indice_PortPin1 = IOCON_INDEX_PIO1[m_PinSCL];
			 if ( m_PortSDA )
				 Indice_PortPin2 = IOCON_INDEX_PIO1[m_PinSDA];

			 IOCON->PIO[Indice_PortPin1] &= ~(1<<10); // Hago clear
			 IOCON->PIO[Indice_PortPin1] |= 1 << 10; // Seteo OPENDRAIN

			 IOCON->PIO[Indice_PortPin2] &= ~(1<<10); // Hago clear
			 IOCON->PIO[Indice_PortPin2] |= 1 << 10; // Seteo OPENDRAIN
		}

		SYSCON->SYSAHBCLKCTRL0 &= ~(1 << 7);

		m_I2C->CFG = 1; // Seteo en modo MAESTRO
		g_I2CVector[m_i2c] = this;

		// Activo interrupciones en NVIC para el modulo I2C correspondiente
		if(m_i2c == i2c0)
			NVIC->ISER[0] |= (1 << 8);
		else if(m_i2c == i2c1)
			NVIC->ISER[0] |= (1 << 7);
		else if(m_i2c == i2c2)
			NVIC->ISER[0] |= (1 << 21);
		else if(m_i2c == i2c3)
			NVIC->ISER[0] |= (1 << 22);
	}

} // Fin de la configuracion del I2C


// Escritura por interrupcion, retorna void
void I2C::IntWrite()
{
	if(m_FlagWrite)
	{
		uint8_t data;
		if(PopWrite(&data))
		{
			m_I2C->MSTDAT = data; // Escribe 8 bits de datos
			m_I2C->MSTCTL = 1; // Bit para ontinuar la transmision
		}
		else
		{
			m_I2C->MSTCTL =  (1<<2); // Para la transmision
			DisableI2CPendingInterrupt();
			m_FlagWrite = false;
		}
	}
} // Empieza a escribir los datos del buffer de escritura en el SDA


// Lectura por interrupcion, retorna void
void I2C::IntRead(bool last)
{

	uint8_t data = (m_I2C->MSTDAT & 0xFF) ;
	PushRead(data);
	m_Bytes--;

	if(!last && m_Bytes)
		m_I2C->MSTCTL = 1; // Bit para continuar la transmision
	else
	{
		m_Bytes = 0;
		m_I2C->MSTCTL = (1<<2); // Para la transmision
		DisableI2CPendingInterrupt();
		m_FlagRead = false;
	}
} // Empieza a guardar los datos recibidos en el buffer de lectura


// Inicia la comunicacion con el bit de lectura, retorna void
void I2C::StartBitRead()
{
	m_FlagRead = true;
	m_NACK = false;
	m_I2C->MSTDAT = ((m_SlaveAddress & 0x7F) << 1) | (1 << 0); // Escribe la direccion con el bit de lectura 1
	m_I2C->MSTCTL = (1<<1); // Empieza la transmision
} // Disparador de lectura


// Inicia la comunicacion con el bit de escritura, retorna void
void I2C::StartBitWrite()
{
	m_FlagWrite = true;
	m_NACK = false;
	m_I2C->MSTDAT = ((m_SlaveAddress & 0x7F) << 1) | (0 << 0); // Escribe la direccion con el bit de escritura 0
	m_I2C->MSTCTL =  (1<<1); // Empieza la transmision
} // Disparador de escritura


// Desactiva la interrupcion del BIT PENDING, retorna void
void I2C::DisableI2CPendingInterrupt() { m_I2C->INTENCLR |= 1; } // Desactiva la interrupcion


// Activa la interrupcion del BIT PENDING, retorna void
void I2C::EnableI2CPendingInterrupt() { m_I2C->INTENSET |= 1; } // Activa la interrupcion para que pueda leer o escribir


// Mete al buffer de lectura los datos recibidos, retorna void
void I2C::PushRead(uint8_t data)
{
	m_BufferRead[m_IndexReadIn] = data;
	m_IndexReadIn++;
	m_IndexReadIn %= READ_BUFFER_SIZE_I2C;
} // Mete el dato recibido en el buffer de lectura


// Saca del buffer de lectura un dato, retorna bool de verifiacion
bool I2C::PopRead(uint8_t* data)
{
	if (m_IndexReadIn != m_IndexReadOut)
	{
		*data = m_BufferRead[m_IndexReadOut];
		m_IndexReadOut++;
		m_IndexReadOut %= READ_BUFFER_SIZE_I2C;
		return true;
	}

	return false;
} // Saca el dato recibido en el buffer de lectura para mandarlo a otro buffer


//Mete al buffer de escritura los datos a transmitir, retorna void
void I2C::PushWrite(uint8_t data)
{
	m_BufferWrite[m_IndexWriteIn] = data;
	m_IndexWriteIn++;
	m_IndexWriteIn %= WRITE_BUFFER_SIZE_I2C;
} // Mete los datos que nos interesa escribir en el buffer de escritura


// Saca el dato del buffer de escritura para transmitirlo, retorna bool de verificacion
bool I2C::PopWrite(uint8_t* data)
{
	if (m_IndexWriteIn != m_IndexWriteOut)
	{
		*data = m_BufferWrite[m_IndexWriteOut];
		m_IndexWriteOut++;
		m_IndexWriteOut %= WRITE_BUFFER_SIZE_I2C;
		return true;
	}

	return false;
} // Saca los datos que deseamos escribir del buffer de escritura


// Transmite los cantidad de datos (size)(message) al esclavo (slaveAddress), retorna bool de verificacion
bool I2C::Write(uint8_t slaveAddress, uint8_t* message, uint16_t size)
{
	if(!m_Error && I2CIsIdle())
	{
		m_SlaveAddress = slaveAddress;
		for (uint32_t i = 0 ; i < size ; i++)
		{
				PushWrite(message[i]);
		}
		StartBitWrite();
		EnableI2CPendingInterrupt();
		return true;
	}
	return false;
}// Retorna true si se inicio la escritura
// Retorna false si hay alguna actividad pendiente ya sea lectura o escritura

bool I2C::Write(uint8_t slaveAddress, uint8_t message)
{
	if(!m_Error && I2CIsIdle())
	{
		m_SlaveAddress = slaveAddress;
		PushWrite(message);
		StartBitWrite();
		EnableI2CPendingInterrupt();
		return true;
	}
	return false;
}

bool I2C::WriteWordAddress(uint8_t slaveAddress, uint8_t wordAddress){return Write(slaveAddress, wordAddress);}


// Le pide al esclavo (slaveAddres) leer determinada cantidad de datos (bytes), retorna bool de verificacion
bool I2C::requestFrom(uint8_t slaveAddress, uint8_t bytes)
{

	if(I2CIsIdle() && !m_Error)
	{
		m_SlaveAddress = slaveAddress;
		m_Bytes = bytes; // Cantidad de bytes que queremos leer y que queremos que guarde en el buffer
		m_Size = bytes;
		StartBitRead();
		StartBitRead();
		EnableI2CPendingInterrupt();
		EnableI2CPendingInterrupt();
		return true;
	}

	return false;
}// Retorna true si se inicio la lectura
// Returns falso si hay alguna actividad pendiente ya sea lectura o escritura

bool I2C::requestFrom(uint8_t slaveAddress) {return requestFrom(slaveAddress, 1);}


// Lee los datos recibidos y los guarda en el buffer, retorna bool de verificacion
bool I2C::Read(uint8_t* message)
{
	static uint32_t index = 0;
	uint8_t data = 0;
	uint8_t *buffer = message;

	if(!m_Error && !(I2CIsReading()) && m_Size != 0)
	{
		if (PopRead(&data))
		{
			buffer[index] = data;
			index++;

			if (index >= m_Size) // size es la cantidad de bytes que queremos guardar en nuestro buffer
			{
				m_Size = 0;
				index = 0;
				return true;
			}
		}
	}

	return false;
} // Guardamos los datos recibidos en el buffer de lectura en un buffer que nos interesa a nosotros, tambien guardamos la cantidad que queremos


// Funcion de interrupcion que se encarga de escribir o leer y ademas
// detecta el ultimo byte a leer, tambien se encarga. Checkeamos el status, retorna void
void I2C::IRQHandler()
{
	if(!m_Error)
	{

			uint8_t stat = (m_I2C->STAT & 0xE) >> 1; // Usamos MSTSTATE, no usamos el MSTPENDING

			if(stat == 0x1 && m_FlagRead) // Listo para recibir datos
				IntRead();

			else if(stat == 0x2 && m_FlagWrite) // Listo para escribir datos
				IntWrite();

			else if(stat == 0x0 || stat == 0x3 || stat == 0x4) // NACK address o NACK data o Idle
			{
				if(m_FlagRead)
					IntRead(true); // Ultimo byte por leer
				else // Si hay problemas en la comunicacion como en la direccion del esclavo o la transmision de datos
				{
					DisableI2CPendingInterrupt();
					m_I2C->MSTCTL = (1<<2); // Para la transmision
					m_NACK = true;
					m_IndexWriteIn = 0;
					m_IndexWriteOut = 0;// Reinicia el buffer de escritura.
					m_FlagWrite = false;
					m_FlagRead = false;
				}
			}


	}
} // Metodo de interrupcion


// Envia los command o data, con el adress del SSD1306, los registros a configurar del mismo, el buffer y su size
void I2C::WriteBlocking(uint8_t addr, uint8_t sub_addr,uint8_t *txBuff, uint32_t txSize){

	while (!(m_I2C->STAT & (1 << 0))) {}
	m_I2C->MSTDAT = ((addr<<1) | (0&1u) );
	m_I2C->MSTCTL = (1 << 1);
	while (!(m_I2C->STAT & (1 << 0))) {}

	// Envio lo que tiene que recibir el display comando o dato
	m_I2C->MSTDAT = sub_addr;
	m_I2C->MSTCTL = (1 << 0);
	while (!(m_I2C->STAT & (1 << 0))) {}

	for( uint32_t i=0; i<txSize; i++){
		m_I2C->MSTDAT = txBuff[i];
		m_I2C->MSTCTL = (1 << 0);
		while (!(m_I2C->STAT & (1 << 0)));
	}
	// Para la transmision
	m_I2C->MSTCTL = (1 << 2);
}


// Destructor I2C, vacio
I2C::~I2C() {}

// Asignacion de las funciones de interrupcion
void I2C0_IRQHandler() { g_I2CVector[0]->IRQHandler(); }
void I2C1_IRQHandler() { g_I2CVector[1]->IRQHandler(); }
void I2C2_IRQHandler() { g_I2CVector[2]->IRQHandler(); }
void I2C3_IRQHandler() { g_I2CVector[3]->IRQHandler(); }
