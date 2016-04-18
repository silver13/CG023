/*
 The MIT License (MIT)

 Copyright (c) 2016 stawel

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


#ifdef RX_H7_PROTOCOL

extern float rx[4];

extern char aux[AUXNUMBER];
extern char lastaux[AUXNUMBER];
extern char auxchange[AUXNUMBER];

#define H7_FLIP_MASK  0x80 // right shoulder (3D flip switch), resets after aileron or elevator has moved and came back to neutral
#define H7_F_S_MASK  0x01


#define PACKET_SIZE 9   // packets have 9-byte payload

void writeregs(const uint8_t data[], uint8_t size) {

	spi_cson();
	for (uint8_t i = 0; i < size; i++) {
		spi_sendbyte(data[i]);
	}
	spi_csoff();
	delay(1000);
}

static const uint8_t bbcal[6] = { 0x3f, 0x4c, 0x84, 0x6F, 0x9c, 0x20 };
static const uint8_t rfcal[8] = { 0x3e, 0xc9, 220, 0x80, 0x61, 0xbb, 0xab, 0x9c };
static const uint8_t demodcal[6] = { 0x39, 0x0b, 0xdf, 0xc4, 0xa7, 0x03 };
static int rxaddress[5] = {0xcc, 0xcc, 0xcc, 0xcc, 0xcc};
//{0xb2, 0xbe, 0x00, 0xcc, 0xcc};


static uint8_t rf_channel_idx = 0;
static uint8_t rx_channel = 0;
static uint8_t rf_channel_shift = 0;
static const uint8_t rf_channel_list[] = {
		 2, 12, 22, 32, 42, 52, 62, 72,
};

static uint8_t getChannel()
{
	uint8_t ch = rf_channel_list[rf_channel_idx] + rf_channel_shift;
    return ch;
}

static void newChannelIdx(uint8_t shift) {
	uint8_t rx = rx_channel;
	uint8_t backward = rx & 0x10;
	rx += shift;
	rx &= 7;
	if(backward) {
		rf_channel_idx = 7 - rx;
	} else {
		rf_channel_idx = rx;
	}
}


void rx_init() {

	writeregs(bbcal, sizeof(bbcal));
	writeregs(rfcal, sizeof(rfcal));
	writeregs(demodcal, sizeof(demodcal));

	xn_writerxaddress(rxaddress);

	xn_writereg( EN_AA, 0);	// aa disabled
	xn_writereg( EN_RXADDR, 1); // pipe 0 only
	xn_writereg( RF_SETUP, B00000001); // lna high current on ( better performance )
	xn_writereg( RX_PW_P0, PACKET_SIZE); // payload size
	xn_writereg( SETUP_RETR, 0); // no retransmissions ( redundant?)
	xn_writereg( SETUP_AW, 3); // address size (5 bits)
	xn_command( FLUSH_RX);
	xn_writereg( RF_CH, 22);  // bind  channel
	xn_writereg(0, B00001111); // power up, crc enabled

}

static char checkpacket() {
	spi_cson();
	int status = spi_sendzerorecvbyte();
	spi_csoff();
	if ((status & B00001110) != B00001110) {
		// rx fifo not empty		
		return 2;
	}

	return 0;
}

int rxdata[PACKET_SIZE];

int txid[2];
int rxmode = 0;
#define PACKET_PERIOD    2625

int decode_cg023(void) {
// throttle		 
		rx[3] = 0.00390625f * (225 - rxdata[0]);

		rx[1] = ( ((int)rxdata[3]) - 112) * 0.00166666f;

		rx[0] = ( ((int)rxdata[2]) - 112) * 0.00166666f; // roll

		rx[2] = (-((int)rxdata[1]) + 112) * 0.00166666f;

		//rxdata[4] L-R: default:32, (63..1)
		//rxdata[5] F-B: default:32, (1..63)
		//rxdata[6] default:0, 1: F/S, 128: flip

		aux[0] = (rxdata[6] & H7_FLIP_MASK)?1:0;
		aux[2] = (rxdata[6] & H7_F_S_MASK)?1:0; //??

		rx_channel = rxdata[7]; //next channel

		//rxdata[8]; //checksum

		return 1;
}

static unsigned long failsafetime;

int failsafe = 0;

void checkrx(void) {
	if (checkpacket()) {
		xn_readpayload(rxdata, PACKET_SIZE);
		if (rxmode == RXMODE_BIND) {	// rx startup , bind mode
			if (rxdata[0] == 0x20) {	// bind packet received

				rxaddress[0] = rxdata[4];
				rxaddress[1] = rxdata[5];
				rxaddress[2] = 0;
				rxmode = RXMODE_NORMAL;
				xn_writerxaddress(rxaddress);

				rf_channel_shift = 7;
			}
		} else {	// normal mode
			decode_cg023();
			failsafetime = gettime();
			failsafe = 0;
		}	// end normal rx mode

	}	// end packet received

	unsigned long timediff = gettime() - failsafetime;
	unsigned long packages = timediff / (PACKET_PERIOD * 2);

	if(packages > 8) {
		//we are lost
		packages /= 8;
	}

	newChannelIdx(packages);
	xn_writereg(0x25, getChannel()); // Set channel frequency

	if (timediff > FAILSAFETIME) {	//  failsafe
		failsafe = 1;
		rx[0] = 0;
		rx[1] = 0;
		rx[2] = 0;
		rx[3] = 0;
	}
}

// end H7 proto
#endif

