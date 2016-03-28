
#include "defines.h"
#include "hardware.h"

// pids in pid.c

// rate in deg/sec
// for acro mode
#define MAX_RATE 360.0
#define MAX_RATEYAW 360.0

// max angle for level mode
#define MAX_ANGLE_HI 55.0f

// max rate used by level pid
#define LEVEL_MAX_RATE 2000.0f

// disable inbuilt expo functions
#define DISABLE_EXPO

// use if your tx has no expo function
// also comment out DISABLE_EXPO to use
// -1 to 1 , 0 = no exp
// positive = less sensitive near center 
#define EXPO_XY 0.3
#define EXPO_YAW 0.0



// failsafe time in uS
#define FAILSAFETIME 1000000  // one second


// battery saver
// do not start software if battery is too low
// flashes 2 times repeatedly at startup
#define STOP_LOWBATTERY

// under this voltage the software will not start 
// if STOP_LOWBATTERY is defined above
#define STOP_LOWBATTERY_TRESH 3.3

// voltage too start warning
// volts
#define VBATTLOW 3.5

// compensation for battery voltage vs throttle drop
// increase if battery low comes on at max throttle
// decrease if battery low warning goes away at high throttle
// in volts
#define VDROP_FACTOR 0.9

// voltage hysteresys
// in volts
#define HYST 0.10




// Gyro LPF filter frequency
// gyro filter 0 = 260hz
// gyro filter 1 = 184hz
// gyro filter 2 = 94hz
// gyro filter 3 = 42hz
// 4 , 5, 6
#define GYRO_LOW_PASS_FILTER 3


// software gyro lpf ( iir )
// set only one below
//#define SOFT_LPF_1ST_023HZ
//#define SOFT_LPF_1ST_043HZ
//#define SOFT_LPF_1ST_100HZ
//#define SOFT_LPF_2ND_043HZ
//#define SOFT_LPF_2ND_088HZ
//#define SOFT_LPF_4TH_088HZ
//#define SOFT_LPF_4TH_160HZ
//#define SOFT_LPF_4TH_250HZ
#define SOFT_LPF_NONE


// this works only on newer boards (non mpu-6050)
// on older boards the hw gyro setting controls the acc as well
#define ACC_LOW_PASS_FILTER 5


// channel for headless mode switch

// H8 protocol channels
// 0 - flip, 1 - expert, 2 - headfree, 3 - headingreturn

// cg023 protocol chanels
// 0 - headless, 
// 1 , 2 , 3 - video , still ,led

// 4 - on always ( all protocols)
// 5 - off always ( all protocols)
#define HEADLESSMODE 5

#define LEVELMODE 2



// enable motor filter
// hanning 3 sample fir filter
#define MOTOR_FILTER


// to do ( currently 8K)

// pwm frequency for motor control
// a higher frequency makes the motors more linear
//#define PWM_490HZ
//#define PWM_8KHZ
//#define PWM_16KHZ
//#define PWM_24KHZ
//#define PWM_32KHZ

// throttle angle compensation in level mode
// comment out to disable
#define AUTO_THROTTLE

// build acro only firmware
//#define ACRO_ONLY

// Radio protocol selection
// select only one
#define RX_CG023_PROTOCOL
//#define RX_BAYANG_PROTOCOL

// mode 1 to mode 3 conversion
// cg023 protocol
//#define RX_CG023_SWAP_YAWROLL

// Flash saving features
#define DISABLE_HEADLESS



//##################################
// debug / other things
// this should not be usually changed


// disable motors for testing
// #define NOMOTORS

// not available
// enable serial out
// 57600 default
// #define SERIAL

// invert yaw pid for hubsan motors
//#define INVERT_YAW_PID

// debug things
 #define DEBUG

// disable the check for known gyro that causes the 4 times flash
// #define DISABLE_GYRO_CHECK


// enable motors if pitch / roll controls off center (at zero throttle)
// possible values: 0 / 1
#define ENABLESTIX 0


#pragma diag_warning 1035 , 177 , 4017
#pragma diag_error 260

//--fpmode=fast























