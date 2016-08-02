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

#ifdef RX_BAYANG_PROTOCOL_BLE_BEACON


// compatibility with older version hardware.h
#if ( !defined RADIO_XN297 && !defined RADIO_XN297L)
#define RADIO_XN297
#endif

#define BAYANG_LOWRATE_MULTIPLIER 0.5


extern float rx[4];
extern char aux[AUXNUMBER];
extern char lastaux[AUXNUMBER];
extern char auxchange[AUXNUMBER];


  char rfchannel[4];
	int rxaddress[5];
	int rxmode = 0;
	int rf_chan = 0;

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

#define BLE_INTERVAL 30000

// this allows different quads to show up at the same time.
// 0 - 255 select a different number for each quad if you need several simultaneous
#define BLE_QUAD_NUMBER 17


void rx_init()
{

/*
uint8_t bbcal[6] = { 0x3f , 0x4c , 0x84 , 0x6F , 0x9c , 0x20  };
writeregs( bbcal , sizeof(bbcal) );


uint8_t rfcal[8] = { 0x3e , 0xc9 , 0x9a , 0x80 , 0x61 , 0xbb , 0xab , 0x9c  };
writeregs( rfcal , sizeof(rfcal) );


uint8_t demodcal[6] = { 0x39 , 0x0b , 0xdf , 0xc4 , 0xa7 , 0x03};
writeregs( demodcal , sizeof(demodcal) );

*/

#ifdef XN297L
	
// demodcal	
// disable xn297L scrambling (nrf24 compatible)
// not working
//xn_writereg( 0x19 , B00001110 );
	
// default bbcal
//xn_writereg( 0x19 , B00001111 );

// this bbcal
// xn_writereg( 0x19 , B00000001 );

// 0 - 1 (0)
#define EN_CLK_OUT 0
// 0 - 7 (5)
#define DA_VREF_MB 5
// 0 - 7 (6)
#define DA_VREF_LB 6
// 0 - 1 (1)
// dac output amplitude control
// 0 = 0.5 ; 1 = 0.8
#define DA_LPF_CTRL 1

// 0 - 1 (0)
#define RSSI_EN 0
// 0 - 3 (1)
#define RSSI_GAIN_CTR 1
// Mixer gain
// 1 - 0 (1)
// 0 = 8 dB ; 1 = 14dB
#define MIXL_GC 1
// 0 - 3 (3)
// pa output stage current select
#define PA_BC 3
// 0 - 3 (3)
#define LNA_GC 3

// 0 - 7 (7)
#define VCO_BIAS 7
// 0 - 1 (0)
#define IB_BPF_TRIM 0
// rx mixer current selection 1 - high
#define MIXL_BC 1
// lna high current enable
#define LNA_HCURR_ 1
// bias current load 
// 0- 26k 1- 24k 2- 22k 3- 20k
#define RES_SEL 2

//xn297L rfcal - default settings
//uint8_t rfcal[4] = { 0x3e , 0xf6 , 0x3f , 0x5d };

uint8_t rfcal[4] = { 0x3e , (VCO_BIAS<<5)|((RES_SEL) << 3)|(LNA_HCURR_<<2)|(MIXL_BC << 1)|IB_BPF_TRIM
	, (RSSI_EN<<7)|(RSSI_GAIN_CTR<<5)|(MIXL_GC<<4)|(PA_BC<<2)|( LNA_GC ) 
	, (EN_CLK_OUT<<7)|(DA_VREF_MB<<4)|(DA_VREF_LB<<1)|(DA_LPF_CTRL) };

//uint8_t rfcal[4] = { 0x3e , 0xc0&((RES_SEL&3) << 4)&(LNA_HCURR_<<2)& (MIXL_BC << 1) & IB_BPF_TRIM, 0x3f , 0x5d };

writeregs( rfcal , sizeof(rfcal) );

//xn_writereg( 0x19 , B00001110 );
	
#define XN_TO_RX B10001111
#define XN_TO_TX B10000010
#define XN_POWER B00111111
	
#endif

	
#ifdef RADIO_XN297
static uint8_t bbcal[6] = { 0x3f , 0x4c , 0x84 , 0x6F , 0x9c , 0x20  };
writeregs( bbcal , sizeof(bbcal) );

static uint8_t rfcal[8] = { 0x3e , 0xc9 , 0x9a , 0x80 , 0x61 , 0xbb , 0xab , 0x9c  };
writeregs( rfcal , sizeof(rfcal) );

static uint8_t demodcal[6] = { 0x39 , 0x0b , 0xdf , 0xc4 , 0xa7 , 0x03};
writeregs( demodcal , sizeof(demodcal) );


#define XN_TO_RX B00001111
#define XN_TO_TX B00000010
#define XN_POWER B00000111
#endif
	
	

	
void bleinit();
bleinit();

//	return;

int rxaddress[5] = { 0 , 0 , 0 , 0 , 0  };
xn_writerxaddress( rxaddress);

	xn_writereg( EN_AA , 0 );	// aa disabled
	xn_writereg( EN_RXADDR , 1 ); // pipe 0 only
	xn_writereg( RF_SETUP , XN_POWER);  // lna high current on ( better performance )
	xn_writereg( RX_PW_P0 , 15 ); // payload size
	xn_writereg( SETUP_RETR , 0 ); // no retransmissions ( redundant?)
	xn_writereg( SETUP_AW , 3 ); // address size (5 bits)
	xn_command( FLUSH_RX);
  xn_writereg( RF_CH , 0 );  // bind on channel 0

  xn_writereg( 0 , XN_TO_RX ); // power up, crc enabled, rx mode


#ifdef RADIO_CHECK
void check_radio(void);
 check_radio();
#endif	
}


void check_radio()
{	
int	rxcheck = xn_readreg( 0x0f); // rx address pipe 5	
	// should be 0xc6
	extern void failloop( int);
	if ( rxcheck != 0xc6) failloop(3);
}

///////////////////
// BLE FUNCTIONS
//  https://github.com/lijunhw/nRF24_BLE/blob/master/Arduino/nRF24_BLE_advertizer_demo/nRF24_BLE_advertizer_demo.ino


void btLeCrc(const uint8_t* data, uint8_t len, uint8_t* dst){
// implementing CRC with LFSR
uint8_t v, t, d;
while(len--){
d = *data++;
for(v = 0; v < 8; v++, d >>= 1){
t = dst[0] >> 7;
dst[0] <<= 1;
if(dst[1] & 0x80) dst[0] |= 1;
dst[1] <<= 1;
if(dst[2] & 0x80) dst[1] |= 1;
dst[2] <<= 1;
if(t != (d & 1)){
dst[2] ^= 0x5B;
dst[1] ^= 0x06;
}
}
}
}



// scrambling sequence for xn297
const uint8_t xn297_scramble[] = {
    0xe3, 0xb1, 0x4b, 0xea, 0x85, 0xbc, 0xe5, 0x66,
    0x0d, 0xae, 0x8c, 0x88, 0x12, 0x69, 0xee, 0x1f,
    0xc7, 0x62, 0x97, 0xd5, 0x0b, 0x79, 0xca, 0xcc,
    0x1b, 0x5d, 0x19, 0x10, 0x24, 0xd3, 0xdc, 0x3f,
    0x8e, 0xc5, 0x2f};

// bit reversed of above
const uint8_t xn297_scramble_rev[] = { 
		0xc7 , 0x8d , 0xd2 , 0x57 , 0xa1 , 0x3d , 0xa7 , 0x66 ,
		0xb0 , 0x75 , 0x31 , 0x11 , 0x48 , 0x96 , 0x77 , 0xf8 , 
		0xe3 , 0x46 , 0xe9 , 0xab , 0xd0 , 0x9e , 0x53 , 0x33 , 
		0xd8 , 0xba , 0x98 , 0x8 , 0x24 , 0xcb , 0x3b , 0xfc , 
		0x71 , 0xa3 , 0xf4 , 85  , 104  , 207  , 169  , 25   ,
		108  ,  93  , 76   , 4   , 146  , 229  , 29  };

		// whitening sequence for adv channel 37 (rf chan 2402)
		// for speed
const uint8 ble_whiten_37[] = { 
	0x8D , 0xd2 , 0x57 , 0xa1 , 0x3d , 0xa7 , 0x66 , 0xb0 ,
	0x75 , 0x31 , 0x11 , 0x48 , 0x96 , 0x77 , 0xf8 , 0xe3 ,
	0x46 , 0xe9 , 0xab , 0xd0 , 0x9e , 0x53 , 0x33 , 0xd8 ,
	0xba , 0x98 , 0x08 , 0x24 , 0xcb , 0x3b , 0xfc , 0x71 ,
	0xa3 , 0xf4 , 0x55 , 0x68 , 0xCF , 0xA9 , 0x19 , 0x6C ,
	0x5D , 0x4C
}; // whitening sequence channel 37 ( 0 - index ; 2 - rf channel; 37 - ble spec)

uint8_t swapbits_old(uint8_t a){
// reverse the bit order in a single byte
uint8_t v = 0;
if(a & 0x80) v |= 0x01;
if(a & 0x40) v |= 0x02;
if(a & 0x20) v |= 0x04;
if(a & 0x10) v |= 0x08;
if(a & 0x08) v |= 0x10;
if(a & 0x04) v |= 0x20;
if(a & 0x02) v |= 0x40;
if(a & 0x01) v |= 0x80;
return v;
}

// from https://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith32Bits
// reverse the bit order in a single byte
uint8_t swapbits(uint8_t a){
unsigned int b = a;
b = ((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
return b;
}

void btLeWhiten(uint8_t* data, uint8_t len, uint8_t whitenCoeff)
{
	// Implementing whitening with LFSR
	uint8_t m;
	while(len--)
		{
		for(m = 1; m; m <<= 1)
			{
			if(whitenCoeff & 0x80)
				{
				whitenCoeff ^= 0x11;
				(*data) ^= m;
				}
			whitenCoeff <<= 1;
			}
		data++;
	}
}
static inline uint8_t btLeWhitenStart(uint8_t chan){
//the value we actually use is what BT'd use left shifted one...makes our life easier
return swapbits(chan) | 2;
}



void btLePacketEncode(uint8_t* packet, uint8_t len, uint8_t chan){
// Assemble the packet to be transmitted
// Length is of packet, including crc. pre-populate crc in packet with initial crc value!
uint8_t i, dataLen = len - 3;

packet[len - 3] = 0x55; //CRC start value: 0x555555
packet[len - 2] = 0x55;
packet[len - 1] = 0x55;

btLeCrc(packet, dataLen, packet + dataLen);
	TS();
for(i = 0; i < 3; i++, dataLen++)
	packet[dataLen] = swapbits(packet[dataLen]);
TE();
if (1)
{	
// faster array based whitening
for(i = 0; i < len; i++) 
	packet[i] ^=ble_whiten_37[i];
}
else // lfsr based
btLeWhiten(packet, len, btLeWhitenStart(chan));
	
}

#define RXDEBUG

#ifdef RXDEBUG
unsigned long packettime;
int channelcount[4];
int failcount;
int packetrx;
int packetpersecond;


int skipstats[12];
int afterskip[12];
//#warning "RX debug enabled"
#endif

// works with 4 and 5 addresses byte
#define XN297_ADDRESS_SIZE_BLE 5


// The MAC address of BLE advertizer -- just make one up
#define MY_MAC_0	BLE_QUAD_NUMBER
#define MY_MAC_1	0x22
#define MY_MAC_2	0x33
#define MY_MAC_3	0x44
#define MY_MAC_4	0x55
#define MY_MAC_5	0xF6

uint8_t buf[48];
int buffint[48];
static const uint8_t chRf[] = {2, 26,80};
static const uint8_t chLe[] = {37,38,39};
uint8_t ch = 0; // RF channel for frequency hopping

int payloadsize;

//uint8_t testarray[42];

void bleinit()
{
// Set access addresses (TX address in nRF24L01) to BLE advertising 0x8E89BED6
// Remember that both bit and byte orders are reversed for BLE packet format
	
int txaddr[5];

/*	
	// 4 byte address
txaddr[0] = swapbits(0x8E)^xn297_scramble[3];
txaddr[1] = swapbits(0x89)^xn297_scramble[2];
txaddr[2] = swapbits(0xBE)^xn297_scramble[1];
txaddr[3] = swapbits(0xD6)^xn297_scramble[0];
txaddr[4] = 0;
*/

/*	
		// 4 byte address - optimized
txaddr[0] = 0x71^0xea;
txaddr[1] = 0x91^0x4b;
txaddr[2] = 0x7d^0xb1;
txaddr[3] = 0x6b^0xe3;
txaddr[4] = 0;
*/

// using 5 byte address because it's shared with rx (rx protocol has 5 bytes)
// saves changing the address size everytime we send
txaddr[0] = 0x71^0x85;
txaddr[1] = 0x91^0xea;
txaddr[2] = 0x7d^0x4b;
txaddr[3] = 0x6b^0xb1;
txaddr[4] = 0xaa^0xe3;	// preamble

	
xn_writetxaddress( txaddr );	


//for( int i = 0 ; i <42; i++) testarray[i] = 0;
//
//	btLeWhiten( testarray , 42, btLeWhitenStart(chLe[0]));
	



//	xn_writereg( EN_AA , 0 );	// aa disabled -- duplicated
//	xn_writereg( RF_SETUP , B00111011);  // high power xn297L only
//	xn_writereg( SETUP_RETR , 0 ); // no retransmissions  -- duplicated
// -- duplicated
//	xn_writereg( SETUP_AW , XN297_ADDRESS_SIZE_BLE - 2 ); // address size (4 bytes for ble)

  xn_writereg( 0x1d, B00011000 ); // 64 bit payload

}

void send_beacon(void);

unsigned int beacon_time;
int loopcounter = 0;
unsigned int ble_txtime;
int ble_send = 0;

#define BLE_TX_TIMEOUT 10000

void beacon_sequence()
{
static int beacon_seq_state = 0;
	
 switch ( beacon_seq_state )
 {
	 case 0:
		 // send data if enough time passed since last send
	   if ( gettime() - ble_txtime > BLE_INTERVAL )
		 {
		 ble_send = 1;
		 send_beacon();
	   beacon_seq_state++;
		 }
	 break;
	 
	 case 1:
		 // wait for data to finish transmitting
			if( (xn_readreg(0x17)&B00010000)  ) 
			{
			// xn_writereg( 0 , B10000011 ); 
				xn_writereg( 0 , XN_TO_RX ); 
			 beacon_seq_state++;

			}
			else
			{// if it takes too long we get rid of it
				if ( gettime() - ble_txtime > BLE_TX_TIMEOUT )
				{
				 xn_command( FLUSH_TX);
				 //xn_writereg( 0 , B10000011 );
					xn_writereg( 0 , XN_TO_RX ); 
				 beacon_seq_state++;
				 //ble_send = 0;
				}
			}
	 break;
	 
		
	 case 2:
		 // restore radio settings to protocol compatible
	   // mainly channel here
		 ble_send = 0;	
		if ( rxmode == 0 )
		{
			xn_writereg(0x25, 0 ); // Set channel frequency	, bind
		}
		 beacon_seq_state++;
	 break;
	 
	 default:
		 beacon_seq_state = 0;		 
	 break;
	
 
	 
 }

}

int interleave = 0;

void send_beacon()
{

	TS();
	
// Channel hopping
ch++;
if (ch>2 ) 
{
  ch = 0;
}

ch = 0;




xn_writereg(RF_CH, chRf[ch]);

uint8_t L=0;
// max len 27 with 5 byte address = 37 total payload bytes

//  xn_writereg( RF_CH , chRf[ch] ); 
L=0;
//4C00 02 15 58 5C DE931B0142CC9A1325009BEDC65E 0000 0000 C5
buf[L++] = B00100010; //PDU type, given address is random; 0x42 for Android and 0x40 for iPhone
//buf[L++] = 0x42; //PDU type, given address is random; 0x42 for Android and 0x40 for iPhone
buf[L++] = 10+ 21; // length of payload
buf[L++] = MY_MAC_0;
buf[L++] = MY_MAC_1;
buf[L++] = MY_MAC_2;
buf[L++] = MY_MAC_3;
buf[L++] = MY_MAC_4;
buf[L++] = MY_MAC_5;

extern float vbattfilt;

int vbatt = vbattfilt *1000.0f;

// packet data unit
buf[L++] = 2; //flags (LE-only, limited discovery mode)
buf[L++] = 0x01;
buf[L++] = 0x06;

unsigned int time = gettime();

time = time>>20; // divide by 1024*1024, no time for accuracy here
time = time * 10;

buf[L++] =  0x03;  // Length
buf[L++] =  0x03;  // Param: Service List
buf[L++] =  0xAA;
buf[L++] =  0xFE;  // Eddystone ID
buf[L++] =  0x11;  // Length
buf[L++] =  0x16;  // Service Data
buf[L++] =  0xAA;
buf[L++] =  0xFE; // Eddystone ID
buf[L++] =  0x20;  // TLM flag
buf[L++] =  0x00; // TLM version
buf[L++] =  vbatt>>8;  // Battery voltage
buf[L++] =  vbatt;  // Battery voltage
buf[L++] =  0x80;  // temp 8.8 fixed point
buf[L++] =  0x00;  // temperature
buf[L++] =  0x00;  // adv count 0
buf[L++] =  0x00;  // adv count 1
buf[L++] =  packetpersecond>>8&0xff;  // adv count 2
buf[L++] =  packetpersecond&0xff;  // adv count 3
buf[L++] =  time>>24;  // powerup time 0 
buf[L++] =  time>>16;  // powerup time 1
buf[L++] =  time>>8;  // powerup time 2
buf[L++] =  time;  // powerup time 3 0.1 sec resolution



L=L+3; //crc


btLePacketEncode(buf, L, chLe[ch]);


for (uint8_t i = 0; i < L; ++i) 
{
buf[i] = buf[i] ^ xn297_scramble_rev[i+ XN297_ADDRESS_SIZE_BLE] ;
}
 

for( int i = 0 ; i < L ; i++) buffint[i] = buf[i];

// 409 uS

//374

//348

xn_command( FLUSH_TX);
//xn_writereg( 0 , B10000010 ); 
xn_writereg( 0 , XN_TO_TX );

payloadsize = L;
xn_writepayload( buffint , L );

// 150uS
/*
while( !(status&B00010000)  ) 
					{
						status = xn_readreg(0x17);
						delay(10);
					}
*/

//xn_writereg( 0 , B10000010 ); 
//delay(4000);


ble_txtime = gettime();

return;

	
}


int statusdebug;
static char checkpacket()
{
	spi_cson();
	int status = spi_sendzerorecvbyte();
statusdebug = status;
	spi_csoff();
	if ( status&(1<<MASK_RX_DR) )
	{	 // rx clear bit
		// this is not working well
	 // xn_writereg( STATUS , (1<<MASK_RX_DR) );
		//RX packet received
		//return 1;
	}
	if( (status & B00001110) != B00001110 )
	{
		// rx fifo not empty		
		return 2;	
	}
	
  return 0;
}


int rxdata[15];

#define CHANOFFSET 512

float packettodata( int *  data)
{
	return ( ( ( data[0]&0x0003) * 256 + data[1] ) - CHANOFFSET ) * 0.001953125 ;	
}


static int decodepacket( void)
{
	if ( rxdata[0] == 165 )
	{
		 int sum = 0;
		 for(int i=0; i<14; i++) 
		 {
			sum += rxdata[i];
		 }	
		if ( (sum&0xFF) == rxdata[14] )
		{
			rx[0] = packettodata( &rxdata[4] );
			rx[1] = packettodata( &rxdata[6] );
			rx[2] = packettodata( &rxdata[10] );
		// throttle		
			rx[3] = ( (rxdata[8]&0x0003) * 256 + rxdata[9] ) * 0.000976562;
		
#ifndef DISABLE_EXPO
	rx[0] = rcexpo ( rx[0] , EXPO_XY );
	rx[1] = rcexpo ( rx[1] , EXPO_XY ); 
	rx[2] = rcexpo ( rx[2] , EXPO_YAW ); 	
#endif
		if  (rxdata[1] != 0xfa) 
		{// low rates
			for ( int i = 0 ; i <3; i++)
			{
				rx[i] = rx[i] * (float) BAYANG_LOWRATE_MULTIPLIER;
			}
		}
		// trims are 50% of controls at max		
	// trims are not used because they interfere with dynamic trims feature of devo firmware
			
//			rx[0] = rx[0] + 0.03225 * 0.5 * (float)(((rxdata[4])>>2) - 31);
//			rx[1] = rx[1] + 0.03225 * 0.5 * (float)(((rxdata[6])>>2) - 31);
//			rx[2] = rx[2] + 0.03225 * 0.5 * (float)(((rxdata[10])>>2) - 31);
	
		  // flip channel
			aux[CH_FLIP] = (rxdata[2] &  0x08)?1:0;
	
		  // expert mode
			aux[CH_EXPERT] = (rxdata[1] == 0xfa)?1:0;
	
		  // headless channel
		  aux[CH_HEADFREE] = (rxdata[2] &  0x02)?1:0;

			// rth channel
			aux[CH_RTH] = (rxdata[2] &  0x01)?1:0;

			aux[CH_INV] = (rxdata[1] & 0x80)?1:0; // inverted flag
						
			aux[CH_VID] = (rxdata[2] & 0x10) ? 1 : 0;
												
			aux[CH_HEADFREE] = (rxdata[2] & 0x20) ? 1 : 0;			


			for ( int i = 0 ; i < AUXNUMBER - 2 ; i++)
			{
				auxchange[i] = 0;
				if ( lastaux[i] != aux[i] ) auxchange[i] = 1;
				lastaux[i] = aux[i];
			}
			
			return 1;	// valid packet	
		}
	 return 0; // sum fail
	}
return 0; // first byte different
}



void nextchannel()
{
	rf_chan++;
	if (rf_chan > 3 ) rf_chan = 0;
	xn_writereg(0x25, rfchannel[rf_chan] );
}



unsigned long lastrxtime;
unsigned long failsafetime;
unsigned long secondtimer;

int failsafe = 0;


unsigned int skipchannel = 0;
int lastrxchan;
int timingfail = 0;
float packettimefilt = 3000;



#define FILTERCALC( sampleperiod, filtertime) (1.0f - ((float)sampleperiod) / ((float)filtertime))


void checkrx(void)
{
	int packetreceived = checkpacket();
	int pass = 0;
	if (packetreceived)
	  {
		  if (rxmode == RXMODE_BIND)
		    {		// rx startup , bind mode
			    xn_readpayload(rxdata, 15);

			    if (rxdata[0] == 164)
			      {	// bind packet
				      rfchannel[0] = rxdata[6];
				      rfchannel[1] = rxdata[7];
				      rfchannel[2] = rxdata[8];
				      rfchannel[3] = rxdata[9];
							
							int rxaddress[5];
				      rxaddress[0] = rxdata[1];
				      rxaddress[1] = rxdata[2];
				      rxaddress[2] = rxdata[3];
				      rxaddress[3] = rxdata[4];
				      rxaddress[4] = rxdata[5];
				      
				      xn_writerxaddress(rxaddress);
				      xn_writereg(0x25, rfchannel[rf_chan]);	// Set channel frequency 
							rxmode = RXMODE_NORMAL;

#ifdef SERIAL
				      printf(" BIND \n");
#endif
			      }
		    }
		  else
		    {		// normal mode  
#ifdef RXDEBUG
			    channelcount[rf_chan]++;
			    packettime = gettime() - lastrxtime;
					
					//if ( packettime > 1500&& packettime < 4500 && !timingfail ) lpf( &packettimefilt , packettime , FILTERCALC( 1 , 1000));
					//limitf(&packettimefilt , 4500);
					if ( skipchannel&& !timingfail ) afterskip[skipchannel]++;
					if ( timingfail ) afterskip[0]++;

#endif

unsigned long temptime = gettime();
	
			    nextchannel();

			    xn_readpayload(rxdata, 15);
			    pass = decodepacket();

			    if (pass)
			      {
#ifdef RXDEBUG
				      packetrx++;
#endif
							skipchannel = 0;
							timingfail = 0;
							lastrxchan = rf_chan;
							lastrxtime = temptime;
				      failsafetime = temptime;
				      failsafe = 0;
			      }
			    else
			      {
#ifdef RXDEBUG
				      failcount++;
#endif
			      }

		    }		// end normal rx mode

	  }			// end packet received

	beacon_sequence();
		
	unsigned long time = gettime();


		if ( skipchannel <5 && rxmode != RXMODE_BIND && !ble_send)
		{
			unsigned int temp = time - lastrxtime ;

			if (!timingfail&& temp > 500 && ( temp - 250 )/(int)packettimefilt >= (skipchannel + 1) ) 
			{
				nextchannel();
#ifdef RXDEBUG				
				skipstats[skipchannel]++;
#endif				
				skipchannel++;
			}
			
		}
	// sequence period 12000
	if (time - lastrxtime > 13000 && rxmode != RXMODE_BIND)
	  {			
			//  channel with no reception   
		  lastrxtime = time;
			// set channel to last with reception
			if (!timingfail) rf_chan = lastrxchan;
			// advance to next channel
		  nextchannel();
			// set flag to discard packet timing
			timingfail = 1;

	  }
	
	if (time - failsafetime > FAILSAFETIME)
	  {	//  failsafe
		  failsafe = 1;
		  rx[0] = 0;
		  rx[1] = 0;
		  rx[2] = 0;
		  rx[3] = 0;
	  }
#ifdef RXDEBUG
	if (gettime() - secondtimer > 1000000)
	  {
		  packetpersecond = packetrx;
		  packetrx = 0;
		  secondtimer = gettime();
	  }
#endif

}

#endif




