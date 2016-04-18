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

//#define RECTANGULAR_RULE_INTEGRAL
#define MIDPOINT_RULE_INTEGRAL
//#define SIMPSON_RULE_INTEGRAL

//#define NORMAL_DTERM
//#define SECOND_ORDER_DTERM
#define NEW_DTERM

#include <stdbool.h>
#include "pid.h"
#include "util.h"
#include "config.h"

#include "defines.h"

#define GAIN 0.8
// Kp											ROLL       PITCH     YAW
float pidkp[PIDNUMBER] = { 16.0e-2, 16.0e-2, 10e-1 };

// Ki											ROLL       PITCH     YAW
float pidki[PIDNUMBER] = { 8e-1, 8e-1, 5e-1 };

// Kd											ROLL       PITCH     YAW
float pidkd[PIDNUMBER] = { 8.8e-1, 8.8e-1, 5.0e-1 };

// "setpoint weighting" 0.0 - 1.0 where 0.0 = normal pid
float b[3] = { 0.3, 0.3, 0.0 };

/*
 float pidkp[PIDNUMBER] = { 7.0e-2 , 7.0e-2 , 5e-1 };
 float pidki[PIDNUMBER] = { 2e-1 , 2e-1 , 5e-1 };
 float pidkd[PIDNUMBER] = { 5.0e-1 , 5.0e-1 , 5.0e-1 };
 */

// output limit			
const float outlimit[PIDNUMBER] = { 0.8, 0.8, 0.4 };

// limit of integral term (abs)
const float integrallimit[PIDNUMBER] = { 0.8, 0.8, 0.4 };

// non changable things below
float ierror[PIDNUMBER] = { 0, 0, 0 };

float pidoutput[PIDNUMBER];

extern float error[PIDNUMBER];
static float lasterror[PIDNUMBER];

extern float looptime;
extern float gyro[3];
extern int onground;

extern float looptime;

#ifdef NORMAL_DTERM
static float lastrate[PIDNUMBER];
#endif

#ifdef NEW_DTERM
static float lastratexx[PIDNUMBER][2];
#endif

#ifdef SECOND_ORDER_DTERM
static float lastratexx[PIDNUMBER][4];
#endif

#ifdef SIMPSON_RULE_INTEGRAL
static float lasterror2[PIDNUMBER];
#endif

float timefactor;

void pid_precalc() {
    timefactor = 0.0032f / looptime;
}

float pid(int x) {

    if (onground) {
        ierror[x] *= 0.8f;
    }

    int iwindup = 0;
    if ((pidoutput[x] == outlimit[x]) && (error[x] > 0)) {
        iwindup = 1;
    }
    if ((pidoutput[x] == -outlimit[x]) && (error[x] < 0)) {
        iwindup = 1;
    }
    if (!iwindup) {
#ifdef MIDPOINT_RULE_INTEGRAL
        // trapezoidal rule instead of rectangular
        ierror[x] = ierror[x]
                + (error[x] + lasterror[x]) * 0.5f * pidki[x] * looptime;
        lasterror[x] = error[x];
#endif

#ifdef RECTANGULAR_RULE_INTEGRAL
        ierror[x] = ierror[x] + error[x] * pidki[x] * looptime;
        lasterror[x] = error[x];
#endif

#ifdef SIMPSON_RULE_INTEGRAL
        // assuming similar time intervals
        ierror[x] = ierror[x] + 0.166666f* (lasterror2[x] + 4*lasterror[x] + error[x]) * pidki[x] * looptime;
        lasterror2[x] = lasterror[x];
        lasterror[x] = error[x];
#endif

    }

    limitf(&ierror[x], integrallimit[x]);

    // P term
    pidoutput[x] = error[x] * (1 - b[x]) * pidkp[x];

    // b
    pidoutput[x] += -(b[x]) * pidkp[x] * gyro[x];

    // I term
    pidoutput[x] += ierror[x];

    // D term

#ifdef NORMAL_DTERM
    pidoutput[x] = pidoutput[x] - (gyro[x] - lastrate[x]) * pidkd[x] * timefactor;
    lastrate[x] = gyro[x];
#endif

#ifdef SECOND_ORDER_DTERM
    pidoutput[x] = pidoutput[x] - ( -( 0.083333333f) *gyro[x] + (0.666666f) * lastratexx[x][0]
            - (0.666666f) * lastratexx[x][2] + ( 0.083333333f) * lastratexx[x][3]) * pidkd[x] * timefactor;

    lastratexx[x][3] = lastratexx[x][2];
    lastratexx[x][2] = lastratexx[x][1];
    lastratexx[x][1] = lastratexx[x][0];
    lastratexx[x][0] = gyro[x];
#endif
#ifdef NEW_DTERM
    pidoutput[x] = pidoutput[x]
            - ((0.5f) * gyro[x] - (0.5f) * lastratexx[x][1]) * pidkd[x]
                    * timefactor;

    lastratexx[x][1] = lastratexx[x][0];
    lastratexx[x][0] = gyro[x];
#endif

    limitf(&pidoutput[x], outlimit[x]);

    return pidoutput[x];
}

