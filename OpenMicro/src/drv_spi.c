

#include "project.h"
#include "drv_spi.h"
#include "binary.h"
#include "config.h"

void spi_init(void)
{    
	// spi port inits
	
		GPIO_InitTypeDef  GPIO_InitStructure;
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	
	spi_csoff();
}


#define gpioset( port , pin) port->BSRR = (0x0001 << pin)
#define gpioreset( port , pin) port->BRR = (0x0001 << pin)

#define MOSIHIGH gpioset( GPIOB, 3)
#define MOSILOW gpioreset( GPIOB, 3);
#define SCKHIGH gpioset( GPIOB, 4);
#define SCKLOW gpioreset( GPIOB, 4);

#define READMISO (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) )

#pragma push

#pragma Otime
#pragma O2

void spi_cson( )
{
	//GPIO_WriteBit(GPIOB, GPIO_PIN_5, Bit_RESET);
	//gpioreset( GPIOA , 15 );
	SPI_SS_PORT->BRR = SPI_SS_PIN;
}

void spi_csoff( )
{
	//gpioset( GPIOB, 5);
	//gpioset( GPIOA , 15 );
	SPI_SS_PORT->BSRR = SPI_SS_PIN;
}


void spi_sendbyte ( int data)
{
for ( int i =7 ; i >=0 ; i--)
	{
		if (  (data>>i)&1  ) 
		{
			MOSIHIGH;
		}
		else 
		{
			MOSILOW;
		}
	
		SCKHIGH;
		SCKLOW;
	}
}


int spi_sendrecvbyte2( int data)
{ 
	int recv = 0;
	for ( int i =7 ; i >=0 ; i--)
	{
		if ( (data) & (1<<7)  ) 
		{
			MOSIHIGH;
		}
		else 
		{
			MOSILOW;
		}
		SCKHIGH;
		data = data<<1;
		if ( READMISO ) recv= recv|(1<<7);
		recv = recv<<1;
		SCKLOW;
	}	
	  recv = recv>>8;
    return recv;
}


 int spi_sendrecvbyte( int data)
{ int recv = 0;

	for ( int i = 7 ; i >=0 ; i--)
	{
		recv = recv<<1;
		if ( (data) & (1<<7)  ) 
		{
			MOSIHIGH;
		}
		else 
		{
			MOSILOW;
		}
		
		data = data<<1;
		
		SCKHIGH;
		
		if ( READMISO ) recv= recv|1;

		SCKLOW;
		
	}	

    return recv;
}


 int spi_sendzerorecvbyte( )
{ int recv = 0;
	MOSILOW;

	for ( int i = 7 ; i >=0 ; i--)
	{
		recv = recv<<1;
		
		SCKHIGH;
		
		if ( READMISO ) recv= recv|1;

		SCKLOW;
		
	}	
    return recv;
}


#pragma pop














