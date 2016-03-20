
#include "defines.h"

// pids in pid.c

// rate in deg/sec
// for low rates
#define MAX_RATE 180.0
#define MAX_RATEYAW 180.0

// disable inbuilt expo functions
#define DISABLE_EXPO

// use if your tx has no expo function
// also comment out DISABLE_EXPO to use
// -1 to 1 , 0 = no exp
// positive = less sensitive near center 
#define EXPO_XY 0.3
#define EXPO_YAW 0.0


// multiplier for high rates
// devo/module uses high rates only
#define HIRATEMULTI 2.0
#define HIRATEMULTIYAW 2.0


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
#define VDROP_FACTOR 0.65

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


// to do

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
//#define SOFT_LPF_NONE



// channel for headless mode switch

// H8 protocol channels
// 0 - flip, 1 - expert, 2 - headfree, 3 - headingreturn

// cg023 protocol chanels
// 0 - headless, 
// 1 , 2 , 3 - video , still ,led

// 4 - on always
// 5 - off always
#define HEADLESSMODE 5

// channel for rates switch

// channel for headless mode switch
// H8 protocol channels
// 0 - flip, 1 - expert, 2 - headfree, 3 - headingreturn

// cg023 protocol chanels
// 0 - headless, 
// 1 , 2 , 3 - video , still ,led

// 4 - on always
// 5 - off always
#define RATES 5



// enable motors if pitch / roll controls off center (at zero throttle)
// possible values: 0 / 1
#define ENABLESTIX 0




// enable motor filter
// hanning 3 sample fir filter
#define MOTOR_FILTER


// to do

// pwm frequency for motor control
// a higher frequency makes the motors more linear
//#define PWM_490HZ
//#define PWM_8KHZ
//#define PWM_16KHZ
//#define PWM_24KHZ
//#define PWM_32KHZ



//##################################
// debug / other things
// this should not be usually changed

// Radio protocol selection

#define RX_CG023_PROTOCOL
//#define RX_BAYANG_PROTOCOL

// mode 1 to mode 3 conversion
// cg023 protocol
//#define RX_CG023_SWAP_YAWROLL


// do not change hardware pins below

#define LED1PIN GPIO_Pin_4
#define LED1PORT GPIOA

#define LED2PIN GPIO_Pin_2
#define LED2PORT GPIOA

#define LED3PIN GPIO_Pin_12
#define LED3PORT GPIOA

#define LED4PIN GPIO_Pin_0
#define LED4PORT GPIOB



#define SOFTI2C_SDAPIN GPIO_Pin_7
#define SOFTI2C_SDAPORT GPIOB

#define SOFTI2C_SCLPIN GPIO_Pin_6
#define SOFTI2C_SCLPORT GPIOB


#define BATTERYPIN GPIO_Pin_7
#define BATTERYPORT GPIOA
#define BATTERY_ADC_CHANNEL ADC_Channel_6

// spi defines not finished
#define SPI_MOSI_PIN
#define SPI_MOSI_PORT

#define SPI_MISO_PIN GPIO_Pin_15
#define SPI_MISO_PORT GPIOA

#define SPI_CLK_PIN

#define SPI_SS_PIN GPIO_Pin_5
#define SPI_SS_PORT GPIOB




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




#pragma diag_warning 1035 , 177 , 4017
#pragma diag_error 260

























