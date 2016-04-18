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

#define CG023_LOWRATE_MULTIPLIER 0.2
#define CG023_MIDRATE_MULTIPLIER  0.6

#ifdef RX_CG023_PROTOCOL

extern float rx[4];
// the last 2 are always on and off respectively
extern char aux[AUXNUMBER];
extern char lastaux[AUXNUMBER];
extern char auxchange[AUXNUMBER];

void writeregs ( uint8_t data[] , uint8_t size )
{

    spi_cson();
    for ( uint8_t i = 0; i < size; i++)
    {
        spi_sendbyte( data[i]);
    }
    spi_csoff();
    delay(1000);
}

void rx_init()
{

    uint8_t bbcal[6] = {0x3f , 0x4c , 0x84 , 0x6F , 0x9c , 0x20};

    writeregs( bbcal , sizeof(bbcal) );

//spi_cson();
//for ( int i = 0 ; i < sizeof(bbcal) ; i++)
//{
//	spi_sendbyte( bbcal[i]);
//}
//spi_csoff();
//delay(1000);

    /*
     spi_cson();
     spi_sendbyte(0x3f);
     spi_sendbyte(0x4c);
     spi_sendbyte(0x84);
     spi_sendbyte(0x6F);
     spi_sendbyte(0x9c);
     spi_sendbyte(0x20);
     spi_csoff();
     delay(1000);
     */

    uint8_t rfcal[8] = {0x3e , 0xc9 , 220 , 0x80 , 0x61 , 0xbb , 0xab , 0x9c};

    writeregs( rfcal , sizeof(rfcal) );

//spi_cson();
//for ( int i = 0 ; i < sizeof(rfcal) ; i++)
//{
//	spi_sendbyte( rfcal[i]);
//}
//spi_csoff();
//delay(1000);

    /*
     // RF_CAL registers
     delay(1000);
     spi_cson();
     spi_sendbyte(0x3e);
     spi_sendbyte(0xc9);
     spi_sendbyte(220);
     spi_sendbyte(0x80);
     spi_sendbyte(0x61);
     spi_sendbyte(0xbb);
     spi_sendbyte(0xab);
     spi_sendbyte(0x9c);
     spi_csoff();
     delay(1000);
     */

    uint8_t demodcal[6] = {0x39 , 0x0b , 0xdf , 0xc4 , 0xa7 , 0x03};

    writeregs( demodcal , sizeof(demodcal) );

//spi_cson();
//for ( int i = 0 ; i < sizeof(demodcal) ; i++)
//{
//	spi_sendbyte( demodcal[i]);
//}
//spi_csoff();
//delay(1000);

// DEMOD_CAL registers
    /*
     spi_cson();
     spi_sendbyte(0x39);
     spi_sendbyte(0x0b);
     spi_sendbyte(0xdf);
     spi_sendbyte(0xc4);
     spi_sendbyte(0xa7);
     spi_sendbyte(0x03);
     spi_csoff();
     delay(1000);
     */
    int rxaddress[5] = {0x26, 0xA8, 0x67, 0x35, 0xCC};
    xn_writerxaddress( rxaddress);

    xn_writereg( EN_AA , 0 );	// aa disabled
    xn_writereg( EN_RXADDR , 1 );// pipe 0 only
    xn_writereg( RF_SETUP , B00000001);// lna high current on ( better performance )
    xn_writereg( RX_PW_P0 , 15 );// payload size
    xn_writereg( SETUP_RETR , 0 );// no retransmissions ( redundant?)
    xn_writereg( SETUP_AW , 3 );// address size (5 bits)
    xn_command( FLUSH_RX);
    xn_writereg( RF_CH , 0x2D );// bind  channel
    xn_writereg( 0 , B00001111 );// power up, crc enabled

}

static char checkpacket()
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

int rxdata[15];

int txid[2];
int rxmode = 0;

#define CG023_FLIP_MASK  0x01 // right shoulder (3D flip switch), resets after aileron or elevator has moved and came back to neutral
#define CG023_EASY_MASK  0x02 // left shoulder (headless mode)
#define CG023_VIDEO_MASK  0x04 // video camera 
#define CG023_STILL_MASK  0x08 // still camera 
#define CG023_LED_OFF_MASK  0x10
#define CG023_RATE_60_MASK  0x20
#define CG023_RATE_100_MASK 0x40

int decode_cg023( void)
{
    if ( rxdata[0] == 0x55 )
    {
        // maybe corrupt packet
        if ( rxdata[3] != 0 || rxdata[4] != 0 ) return 0;
        if ( rxdata[1] != txid[0] || rxdata[2] != txid[1] ) return 0;

// throttle		 
        rx[3] = 0.00390625f * rxdata[5];

#ifndef RX_CG023_SWAP_YAWROLL		 
// normal yaw - roll
        if ( rxdata[6] >= 0x80 )
        {
            rx[2] = -rxdata[6] * 0.0166666f + 2.1166582f; // yaw
        }
        else if ( rxdata[6] <= 0x3C ) rx[2] = (1.0f + ( rxdata[6] - 60) * 0.0166666f); // yaw
        else rx[2] = 0.0;
        rx[0] = - rxdata[8] * 0.0166666f + 2.1166582f;// roll
#else
        // swapped yaw - roll (mode 3)
        if ( rxdata[6] >= 0x80 )
        {
            rx[0] = -rxdata[6] * 0.0166666f + 2.1166582f; // yaw
        }
        else if ( rxdata[6] <= 0x3C ) rx[0] = (1.0f + ( rxdata[6] - 60) * 0.0166666f); // yaw
        else rx[0] = 0.0;
        rx[2] = - rxdata[8] * 0.0166666f + 2.1166582f;// roll
#endif

        rx[1] = - rxdata[7] * 0.0166666f + 2.1166582f;

#ifndef DISABLE_EXPO
        rx[0] = rcexpo ( rx[0] , EXPO_XY );
        rx[1] = rcexpo ( rx[1] , EXPO_XY );
        rx[2] = rcexpo ( rx[2] , EXPO_YAW );
#endif

        // switch flags

        aux[0] = (rxdata[13] & CG023_FLIP_MASK)?1:0;
        aux[1] = (rxdata[13] & CG023_VIDEO_MASK)?1:0;
        aux[2] = (rxdata[13] & CG023_STILL_MASK)?1:0;
        aux[3] = (rxdata[13] & CG023_LED_OFF_MASK)?1:0;

        float ratemulti = 1.0;
        if ( rxdata[13] & CG023_RATE_100_MASK )
        {
            goto skip;
        }
        else if ( rxdata[13] & CG023_RATE_60_MASK )
        {
            ratemulti = CG023_MIDRATE_MULTIPLIER;
        }
        else
        {
            ratemulti = CG023_LOWRATE_MULTIPLIER;
        }

        for ( int i = 0; i <= 2; i++)
        {
            rx[i] = rx[i] * ratemulti;
        }
        skip:
        for ( int i = 0; i < AUXNUMBER - 2; i++)
        {
            auxchange[i] = 0;
            if ( lastaux[i] != aux[i] ) auxchange[i] = 1;
            lastaux[i] = aux[i];
        }

        return 1;
    }
    else
    {
        // non data packet
        return 0;
    }
//
}

//
static unsigned long failsafetime;

int failsafe = 0;

//#define RXDEBUG

#ifdef RXDEBUG	
struct rxdebug
{
    unsigned long packettime;
    int failcount;
    int packetpersecond;
}
rxdebug;
int packetrx;
unsigned long lastrxtime;
unsigned long secondtimer;
#warning "RX debug enabled"
#endif

void checkrx( void)
{
    if ( checkpacket() )
    {
        xn_readpayload( rxdata , 15);
        if ( rxmode == RXMODE_BIND)
        {	// rx startup , bind mode
            if ( rxdata[0] == 0xAA )
            {	// bind packet received

                txid[0] = rxdata[1];
                txid[1] = rxdata[2];

                rxmode = RXMODE_NORMAL;

                xn_writereg(0x25, (uint8_t)(rxdata[1] - 0x7D) );// Set channel frequency

#ifdef SERIAL
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

            int pass = decode_cg023();

            if ( pass )
            {
                failsafetime = gettime();
                failsafe = 0;

            }
            else
            {
#ifdef RXDEBUG
                rxdebug.failcount++;
#endif
            }

        }	// end normal rx mode

    }	// end packet received

    unsigned long time = gettime();

    if( time - failsafetime > FAILSAFETIME )
    {	//  failsafe
        failsafe = 1;
        rx[0] = 0;
        rx[1] = 0;
        rx[2] = 0;
        rx[3] = 0;
    }
#ifdef RXDEBUG
    // packets per second counter
    if ( time - secondtimer > 1000000)
    {
        rxdebug.packetpersecond = packetrx;
        packetrx = 0;
        secondtimer = gettime();
    }
#endif

}

// end cg023 proto
#endif

