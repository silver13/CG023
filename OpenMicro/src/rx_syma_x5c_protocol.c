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


#include "binary.h"
#include "drv_spi.h"

#include "project.h"
#include "xn297.h"
#include "drv_time.h"
#include <stdio.h>
#include "config.h"
#include "defines.h"

#include "rx_bayang.h"
#include "util.h"


#ifdef RX_SYMA_X5C_PROTOCOL

// compatibility with older version hardware.h
#if ( defined RADIO_XN297 || defined RADIO_XN297L)
    #error PROTOCOL needs NRF24 compatible radio
#endif


extern float rx[4];
// the last 2 are always on and off respectively
extern char aux[AUXNUMBER];
extern char lastaux[AUXNUMBER];
extern char auxchange[AUXNUMBER];


uint8_t chans[15] = {0x1d, 0x2f, 0x26, 0x3d, 0x15, 0x2b, 0x25, 0x24,
                           0x27, 0x2c, 0x1c, 0x3e, 0x39, 0x2d, 0x22};

void writeregs ( uint8_t data[] , uint8_t size )
{
	
spi_cson();
for ( uint8_t i = 0 ; i < size ; i++)
{
	spi_sendbyte( data[i]);
}
spi_csoff();
delay(1000);
}

void writeregs2 ( uint8_t *data , uint8_t size )
{
	
spi_cson();
for ( uint8_t i = 0 ; i < size ; i++)
{
	spi_sendbyte( data[i]);
}
spi_csoff();
delay(1000);
}

extern int xn_activate( int command);

int bkfound = 0;

void rx_init()
{

	


if (xn_readreg(0x07) & 0x80)
{
    xn_activate(0x53);
}

static int rxaddress[5] =  {0x6d,0x6a,0x73,0x73,0x73};
xn_writerxaddress( rxaddress);

    
	xn_writereg( EN_AA , 0 );	// aa disabled
	xn_writereg( EN_RXADDR , 1 ); // pipe 0 only
	xn_writereg( RF_SETUP , B00000000);  // 
	xn_writereg( RX_PW_P0 , 16 ); // payload size
	xn_writereg( SETUP_RETR , 0 ); // no retransmissions ( redundant?)
	xn_writereg( SETUP_AW , 3 ); // address size (5 bits)
	xn_command( FLUSH_RX);
    xn_writereg( RF_CH , 0x08 );  // bind  channel

    xn_activate(0x53); // BK24xx bank switch

    if ( xn_readreg(0x07) & 0x80 ) 
    {
      bkfound = 1;
    } 
    
    xn_activate(0x53); // switch bank back

  xn_writereg( 0 , B00001111 ); // power up, crc off


#ifdef RADIO_CHECK
	int temp = xn_readreg( 0x0f); // rx address pipe 5	
	// should be 0xc6
	extern void failloop( int);
	if ( temp != 0xc6) failloop(3);
#endif	
}



static int checkpacket()
{
	spi_cson();
	int status = spi_sendzerorecvbyte();
	spi_csoff();
	if( (status & B00001110) != B00001110 )
	{
		// rx fifo not empty		
		return 2;	
	}
	
  return 0;
}


int rxdata[16];

int rxmode = 0;


float syma_scale( int input)
{
  float result = 0;
  if ( input >= 0x80 )
		{
			result = (input - 0x80)  * 0.007874f ;
		}
		else  result = - input * 0.007874f ; 
return result;
}

uint8_t sum;

int decode_syma_x5c()
{
    if ( rxdata[7] == 0xae && rxdata[8] == 0xa9  )
    {
       sum = 0;
       for ( int i = 0 ; i < 15 ; i++) 
        sum+= rxdata[i];
        
       if ( sum == rxdata[15] )
       {
        // checksum passes
        
        // throttle
        rx[3] = (float) rxdata[0] * 0.00393f;
           
        rx[2] = syma_scale( rxdata[1] );
        rx[1] = syma_scale( rxdata[2] );
        rx[0] = syma_scale( rxdata[3] );
        
           aux[CH_VID] = ( rxdata[14] & 0x10 )? 1 : 0;//video
        aux[CH_PIC] = ( rxdata[14] & 0x08 )? 1 : 0 ;//pic
        aux[CH_FLIP] = ( rxdata[14] & 0x01 )? 1 : 0 ;//flip
        aux[CH_EXPERT] = ( rxdata[14] & 0x04 )? 1 : 0 ;//expert
           
        // update change flags
        for ( int i = 0 ; i < AUXNUMBER - 2 ; i++)
		{
			auxchange[i] = 0;
			if ( lastaux[i] != aux[i] ) auxchange[i] = 1;
			lastaux[i] = aux[i];
		}
           
        return 1;  
       }
       else return 0;
       
    }
    // data 7 or 8 different
    return 0;
}



//
static unsigned long failsafetime;

int failsafe = 0;


//#define RXDEBUG

#ifdef RXDEBUG	
struct rxdebug rxdebug;

int packetrx;
unsigned long lastrxtime;
unsigned long secondtimer;
#warning "RX debug enabled"
#endif

int rx_chan_count[16];
#include <stdlib.h>

int currentchannel = 0;
unsigned long lastrx;

void checkrx( void)
{
		if ( checkpacket() ) 
		{ 
			xn_readpayload( rxdata , 16);
			if ( rxmode == 0)
			{	// rx startup , bind mode	

             
				if (rxdata[7] == 0xae && rxdata[8] == 0xa9  ) 
				{// bind packet received			
					
					rxmode = 1;				

				  xn_writereg( RF_CH, chans[0] ); // Set channel frequency	
                    
				
					#ifdef SERIAL_INFO	
					printf( " BIND \n");
					#endif
				}
			}
			else
			{	// normal mode	
				#ifdef RXDEBUG	
				rxdebug.packettime = gettime() - lastrxtime;
				lastrxtime = gettime();
				#endif
				
				int pass = decode_syma_x5c();
			 
				if ( pass )
				{ 	
					failsafetime = gettime(); 
					failsafe = 0;
                    #ifdef RXDEBUG	
                    packetrx++;
                    #endif
                    lastrx = failsafetime;
                    if ( currentchannel < sizeof ( rx_chan_count )) rx_chan_count[currentchannel]++;
                    currentchannel++;
                    currentchannel%=sizeof(chans);
                    xn_writereg( RF_CH, chans[currentchannel] ); // Set channel frequency

				}	
				else
				{
				#ifdef RXDEBUG	
				rxdebug.failcount++;
				#endif	
				}
			
			}// end normal rx mode
				
		}// end packet received
        		
		unsigned long time = gettime();

if ( time - lastrx > 8000* (sizeof(chans) - 2) )
{
    currentchannel++;
    currentchannel%=sizeof(chans);
    xn_writereg( RF_CH, chans[currentchannel] ); // Set channel frequency

    lastrx = time;
}
		
		if( time - failsafetime > FAILSAFETIME )
		{//  failsafe
		  failsafe = 1;
			rx[0] = 0;
			rx[1] = 0;
			rx[2] = 0;
			rx[3] = 0;
		}
#ifdef RXDEBUG	
		// packets per second counter
			if ( time - secondtimer  > 1000000)
			{
				rxdebug.packetpersecond = packetrx;
				packetrx = 0;
				secondtimer = gettime();
			}
#endif

}
	


// end cg023 proto
#endif






