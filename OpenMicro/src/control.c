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

#include <inttypes.h>
#include <math.h>

#include "pid.h"
#include "config.h"
#include "util.h"
#include "drv_pwm.h"
#include "control.h"
#include "defines.h"
#include "drv_time.h"
#include "sixaxis.h"
#include "drv_fmc.h"


extern float rx[7];
extern float gyro[3];
extern int failsafe;
extern float pidoutput[PIDNUMBER];

extern float angleerror[3];
extern float attitude[3];

int onground = 1;
float thrsum;

float error[PIDNUMBER];
float motormap( float input);

float yawangle;

extern float looptime;

extern char auxchange[AUXNUMBER];
extern char aux[AUXNUMBER];

float motorfilter( float motorin ,int number);

extern float apid(int x);

#ifdef NOMOTORS
// to maintain timing or it will be optimized away
float tempx[4];
#endif

unsigned long timecommand = 0;

void control( void)
{	

	// check for accelerometer calibration command
	if ( onground )
	{
		if ( rx[1] < -0.8  )
		{
			if ( !timecommand) timecommand = gettime();
			if ( gettime() - timecommand > 3e6 )
			{
				// do command
					
			    gyro_cal();	// for flashing lights				  
			    acc_cal();
				  extern float accelcal[3];
				  #ifndef ACRO_ONLY
				  fmc_write( accelcal[0] + 127 , accelcal[1] + 127);
				  #endif
			    // reset loop time so max loop time is not exceeding
			    extern unsigned lastlooptime;
			    lastlooptime = gettime();
		      timecommand = 0;
			}		
		}
		else timecommand = 0;				
	}

// yaw angle for headless mode	
	yawangle = yawangle + gyro[YAW]*looptime;
	if ( auxchange[HEADLESSMODE] )
	{
		yawangle = 0;
	}
	
	if ( aux[HEADLESSMODE] ) 
	{
		float temp = rx[ROLL];
		rx[ROLL] = rx[ROLL] * cosf( yawangle) - rx[PITCH] * sinf(yawangle );
		rx[PITCH] = rx[PITCH] * cosf( yawangle) + temp * sinf(yawangle ) ;
	}
	
pid_precalc();	

#ifndef ACRO_ONLY
	// dual mode build
	if (aux[LEVELMODE])
	  {			// level mode

		  angleerror[0] = rx[0] * MAX_ANGLE_HI - attitude[0];
		  angleerror[1] = rx[1] * MAX_ANGLE_HI - attitude[1];

		  error[0] = apid(0) * LEVEL_MAX_RATE_HI * DEGTORAD - gyro[0];
		  error[1] = apid(1) * LEVEL_MAX_RATE_HI * DEGTORAD - gyro[1];

	  }
	else
	  {	// rate mode

		  error[0] = rx[0] * (float) MAX_RATE * DEGTORAD  - gyro[0];
		  error[1] = rx[1] * (float) MAX_RATE * DEGTORAD  - gyro[1];

		  // reduce angle Iterm towards zero
		  extern float aierror[3];
		  for (int i = 0; i <= 2; i++)
			  aierror[i] *= 0.8f;
	  }


	error[2] = rx[2] * (float) MAX_RATEYAW * DEGTORAD  - gyro[2];

	pid(0);
	pid(1);
	pid(2);
#else
// rate only build
	error[ROLL] = rx[ROLL] * (float) MAX_RATE * DEGTORAD  - gyro[ROLL];
	error[PITCH] = rx[PITCH] * (float) MAX_RATE * DEGTORAD  - gyro[PITCH];
	error[YAW] = rx[YAW] * (float) MAX_RATEYAW * DEGTORAD  - gyro[YAW];
	
pid_precalc();

	pid(ROLL);
	pid(PITCH);
	pid(YAW);
#endif

	
// map throttle so under 10% it is zero	
float	throttle = mapf(rx[3], 0 , 1 , -0.1 , 1 );
if ( throttle < 0   ) throttle = 0;

// turn motors off if throttle is off and pitch / roll sticks are centered
	if ( failsafe || (throttle < 0.001f && (!ENABLESTIX||  (fabs(rx[ROLL]) < 0.5f && fabs(rx[PITCH]) < 0.5f ) ) ) ) 

	{ // motors off
		for ( int i = 0 ; i <= 3 ; i++)
		{
			pwm_set( i , 0 );	
		}	
		onground = 1;
		thrsum = 0;
		#ifdef MOTOR_FILTER		
		// reset the motor filter
		for ( int i = 0 ; i <= 3 ; i++)
					{		
					motorfilter( 0 , i);
					}	
		#endif
	}
	else
	{
		onground = 0;
		float mix[4];	
		
#ifdef INVERT_YAW_PID
pidoutput[2] = -pidoutput[2];			
#endif
		
		mix[MOTOR_FR] = throttle - pidoutput[ROLL] - pidoutput[PITCH] + pidoutput[YAW];		// FR
		mix[MOTOR_FL] = throttle + pidoutput[ROLL] - pidoutput[PITCH] - pidoutput[YAW];		// FL	
		mix[MOTOR_BR] = throttle - pidoutput[ROLL] + pidoutput[PITCH] - pidoutput[YAW];		// BR
		mix[MOTOR_BL] = throttle + pidoutput[ROLL] + pidoutput[PITCH] + pidoutput[YAW];		// BL	
		
#ifdef INVERT_YAW_PID
// we invert again cause it's used by the pid internally (for limit)
pidoutput[2] = -pidoutput[2];			
#endif
					
		
#ifdef MOTOR_FILTER		
for ( int i = 0 ; i <= 3 ; i++)
			{
			mix[i] = motorfilter(  mix[i] , i);
			}	
#endif
			
		for ( int i = 0 ; i <= 3 ; i++)
		{
		#ifndef NOMOTORS
		pwm_set( i ,motormap( mix[i] ) );
		#else
		tempx[i] = motormap( mix[i] );
		#endif
		}	
	
		
		thrsum = 0;
		for ( int i = 0 ; i <= 3 ; i++)
		{
			if ( mix[i] < 0 ) mix[i] = 0;
			if ( mix[i] > 1 ) mix[i] = 1;
			thrsum+= mix[i];
		}	
		thrsum = thrsum / 4;
		
	}// end motors on
	
}


float motormap( float input)
{
	return input;
}

float motormapx( float input)
{ 
	// this is a thrust to pwm function
	//  float 0 to 1 input and output
	// output can go negative slightly
	// measured eachine motors and prop, stock battery
	// a*x^2 + b*x + c
	// a = 0.262 , b = 0.771 , c = -0.0258

if (input > 1.0f) input = 1.0f;
if (input < 0) input = 0;

input = input*input*0.262f  + input*(0.771f);
input += -0.0258f;

return input;   
}


float hann_lastsample[4];
float hann_lastsample2[4];

// hanning 3 sample filter
float motorfilter( float motorin ,int number)
{
 	float ans = motorin*0.25f + hann_lastsample[number] * 0.5f +   hann_lastsample2[number] * 0.25f ;
	
	hann_lastsample2[number] = hann_lastsample[number];
	hann_lastsample[number] = motorin;
	
	return ans;
}




