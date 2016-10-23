#include "config.h"
#include "drv_time.h"
#include "util.h"

extern int lowbatt;
extern int rxmode;
extern int failsafe;
extern int ledcommand;
extern char aux[];
int rgb_led_value;

int rgb_loopcount = 0;

// normal flight rgb colour - LED switch ON
#define RGB_VALUE_INFLIGHT_ON RGB( 255 , 255 , 255 )

// normal flight rgb colour - LED switch OFF
#define RGB_VALUE_INFLIGHT_OFF RGB( 0 , 0 , 0 )

//  colour before bind
#define RGB_VALUE_BEFORE_BIND RGB( 0 , 128 , 128 )

// fade from one color to another when changed
#define RGB_FILTER_ENABLE
#define RGB_FILTER_TIME_MICROSECONDS 50e3

// runs the update once every 16 loop times ( 16 mS )
#define DOWNSAMPLE 16

#define RGB_FILTER_TIME FILTERCALC( 1000*DOWNSAMPLE , RGB_FILTER_TIME_MICROSECONDS)
#define RGB( r , g , b ) ( ( (g&0xff)<<16)|( (r&0xff)<<8)|( b&0xff )) 

extern	void rgb_send( int data);

float r_filt, g_filt, b_filt;

#if ( RGB_LED_NUMBER > 0)

void rgb_led_set( int rgb )
{
#ifdef RGB_FILTER_ENABLE	
int g = rgb>>16;
int r = (rgb&0x0000FF00)>>8;
int b = rgb & 0xff;


lpf( &r_filt, r , RGB_FILTER_TIME);
lpf( &g_filt, g , RGB_FILTER_TIME);
lpf( &b_filt, b , RGB_FILTER_TIME);

rgb_led_value = RGB( (int)r_filt , (int)g_filt , (int)b_filt );
#else
rgb_led_value = rgb;
#endif
}


void rgb_ledflash( int color1 , int color2 , uint32_t period , int duty )
{
	if ( gettime() % period > (period*duty)>>4 )
	{
		rgb_led_set( color1 );
	}
	else
	{
		rgb_led_set( color2 );
	}

}


void rgb_led_senddata( void)
{
	for (int i = 0 ; i < RGB_LED_NUMBER ; i++)
	{
		rgb_send( rgb_led_value );
	}
}


void rgb_led_lvc( void)
{
rgb_loopcount++;
if ( rgb_loopcount > DOWNSAMPLE )
{
	rgb_loopcount = 0;
// led flash logic	
if ( lowbatt )
	//rgb_led_set( RGB( 255 , 0 , 0 ) );
	rgb_ledflash ( RGB( 255 , 0 , 0 ), RGB( 255 , 32 , 0 ) ,500000 , 8);
else
{
		if ( rxmode == RXMODE_BIND)
		{// bind mode
		//rgb_ledflash ( RGB( 0 , 0 , 255 ), RGB( 0 , 128 , 0 ), 1000000, 12);
			rgb_led_set( RGB_VALUE_BEFORE_BIND );
		}else
		{// non bind
			if ( failsafe) 
				{
					rgb_ledflash ( RGB( 0 , 128 , 0 ) , RGB( 0 , 0 , 128 ) ,500000, 8);	
					//rgb_led_set( RGB( 0 , 128 , 128 ) );					
				}
			else 
			{
/*			
				#ifdef GESTURES2_ENABLE
				if (ledcommand)
						  {
							  if (!ledcommandtime)
								  ledcommandtime = gettime();
							  if (gettime() - ledcommandtime > 500000)
							    {
								    ledcommand = 0;
								    ledcommandtime = 0;
							    }
							  ledflash(100000, 8);
						  }
						else
				
					#endif // end gesture led flash
*/
				if ( aux[LEDS_ON] )
			
				rgb_led_set( RGB_VALUE_INFLIGHT_ON );
				else 			
				rgb_led_set( RGB_VALUE_INFLIGHT_OFF );
			}
		} 		
		
	}

	rgb_led_senddata();

}	
}

#endif
