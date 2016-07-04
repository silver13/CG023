/*
The MIT License (MIT)

Copyright (c) 2016 silverx

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include "project.h"
#include <stdint.h>
#include <stdio.h>

#include "drv_softi2c.h"
#include "config.h"



//static GPIO_InitTypeDef  sdainit;

void delay(int);

// inter-version fix
#ifndef SOFTI2C_SPEED_SLOW1
#ifndef SOFTI2C_SPEED_SLOW2
#ifndef SOFTI2C_SPEED_FAST
#define SOFTI2C_SPEED_FAST
#endif
#endif
#endif

#ifdef SOFTI2C_SPEED_FAST
#define _delay  //
#define _delay2 //
#endif

#ifdef SOFTI2C_SPEED_SLOW1

#ifdef __GNUC__
void delayraw()
{
	volatile uint8_t count = 1;
	while (count--);
}
#else
void delayraw()
{
	uint8_t count = 1;
	while (count--);
}
#endif

#define _delay  delayraw()
#define _delay2 //delay(1)
#endif

#ifdef SOFTI2C_SPEED_SLOW2
#define _delay  delay(1)
#define _delay2 delay(1)
#endif

	
	int sda;
	int scl;	
	
	void _sendstart(void);
	void _sendstop(void);
	void sdalow(void);
	void sdahigh(void);
	void scllow(void);
	void sclhigh(void);
	void _restart(void);	
	int _readbyte( int); 
	int _sendbyte( int);
	int _readsda(void);
	
void setoutput(void);

////////////////////////////////
/////////I2C Routines//////////


 void sdalow()
{	
  SOFTI2C_SDAPORT->BRR = SOFTI2C_SDAPIN;
  sda = 0;
	//_delay;
	_delay2;
}


  void sdahigh()
{
	SOFTI2C_SDAPORT->BSRR = SOFTI2C_SDAPIN;
	_delay;
  sda = 1; 
}


  void scllow()
{
 SOFTI2C_SCLPORT->BRR = SOFTI2C_SCLPIN;
 scl = 0;
 _delay2;
}

  void sclhigh()
{
  SOFTI2C_SCLPORT->BSRR = SOFTI2C_SCLPIN;
	_delay;
 scl = 1; 
}

  void sclhighlow()
{
 SOFTI2C_SCLPORT->BSRR = SOFTI2C_SCLPIN;
	_delay;
 SOFTI2C_SCLPORT->BRR = SOFTI2C_SCLPIN;
//_delay;
	_delay2;
 scl = 0;
}


int _readsda()
{
//if ( sdaout) setinput();	
//return ( GPIO_ReadInputDataBit(SOFTI2C_SDAPORT, SOFTI2C_SDAPIN) ); 
return	SOFTI2C_SDAPORT->IDR & SOFTI2C_SDAPIN;
}


void _sendstart()
{
  sdalow();
}


void _restart()
{
 if (sda == 0) 
 {
   sdahigh();
 }
 sclhigh();
 sdalow();
}

void _sendstop()
{ 
  if (sda == 1) 
  {
    sdalow(); 
  }
  sclhigh();
  sdahigh();
}



int _sendbyte( int value )
{
int i;

 scllow();
 
 for ( i = 7; i >= 0 ;i--)
 {
	 if ((value >> i)&1) 
	 {
		sdahigh();
	 }
	 else 
	 {
		sdalow();
	 }
	 //sclhigh();
	 //scllow();
	sclhighlow();
 }
 
 if (!sda) sdahigh(); // release the line
 //get ack

 sclhigh();
// skip ack since it is not used here
 uint8_t ack;// = _readsda();
	#ifdef i2cdebug
  if (ack)
	{
	if (debug) Serial.println("NOT RECEIVED"); 
	}
	#endif
 scllow();
return ack; 
}

int _readbyte(int ack)  //ACK 1 single byte ACK 0 multiple bytes
{
 int data=0;

 if ( sda == 0) 
 {
   sdahigh();
 }
 int i;
 for( i = 7; i>=0;i--)
 {
  sclhigh(); 
 if (_readsda() ) data|=(1<<i);
  scllow();
 }

if (ack)  
{
  sdahigh();
} 
else 
{
  sdalow();
}
  sclhigh();
  scllow();
if (sda) sdalow(); 

return data;
}


int softi2c_write( int device_address , int address, int value)
{
 _sendstart();
 _sendbyte((device_address<<1));
 _sendbyte(address);
  uint8_t ack = _sendbyte(value);
  _sendstop();
  return ack;
}


int softi2c_read(int device_address , int register_address)  
{
 _sendstart();
 _sendbyte((device_address<<1));
 _sendbyte(register_address);
 _restart(); 
 _sendbyte((device_address<<1) + 1);
 uint8_t x = _readbyte(1); 
 _sendstop();
 return x; 
}


void softi2c_writedata(int device_address ,int register_address , int *data, int size ) 
{
	int index = 0;
 _sendstart();
 _sendbyte(device_address<<1);
 _sendbyte(register_address);
  
  _sendstop();

	while(index<size)
	{
	_sendbyte(data[index]);
	index++;	
	}
 _sendstop();

}


void softi2c_readdata(int device_address ,int register_address , int *data, int size ) 
{
	int index = 0;
 _sendstart();
 _sendbyte(device_address<<1);
 _sendbyte(register_address);
 _restart();
 _sendbyte( (device_address<<1) + 1);
	while(index<size-1)
	{
	data[index] = _readbyte(0);
	index++;	
	}
  data[index] = _readbyte(1);
 _sendstop();
 
}



void softi2c_init()
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	
  GPIO_InitStructure.GPIO_Pin = SOFTI2C_SCLPIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SOFTI2C_SCLPORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SOFTI2C_SDAPIN;
	GPIO_Init(SOFTI2C_SDAPORT, &GPIO_InitStructure);

//sda = 0;
//scl = 0;

sdahigh();
sclhigh();
	
}




///////////////////////////////END I2C///////
