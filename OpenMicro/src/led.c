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
#include "drv_time.h"
#include "led.h"
#include "config.h"

#define LEDALL 15

void ledon( uint8_t val )
{
if ( val&8)	GPIO_SetBits( LED1PORT, LED1PIN);
if ( val&4)	GPIO_SetBits( LED2PORT, LED2PIN);
if ( val&2)	GPIO_SetBits( LED3PORT, LED3PIN);
if ( val&1)	GPIO_SetBits( LED4PORT, LED4PIN);
			
}

void ledoff( uint8_t val )
{
	
if ( val&8) GPIO_ResetBits( LED1PORT, LED1PIN);
if ( val&4) GPIO_ResetBits( LED2PORT, LED2PIN);
if ( val&2) GPIO_ResetBits( LED3PORT, LED3PIN);
if ( val&1) GPIO_ResetBits( LED4PORT, LED4PIN);	
}

void ledflash( uint32_t period , int duty )
{
	if ( gettime() % period > (period*duty)/16 )
	{
		ledon(LEDALL);
	}
	else
	{
		ledoff(LEDALL);
	}
	
	
}












