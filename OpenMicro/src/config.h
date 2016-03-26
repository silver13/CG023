
#include "defines.h"

// pids in pid.c

// rate in deg/sec
// for acro mode
#define MAX_RATE 360.0
#define MAX_RATEYAW 360.0

// max angle for level mode
#define MAX_ANGLE_HI 35.0f

// max rate used by level pid for controlling the quad
#define LEVEL_MAX_RATE_HI 360.0f

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
#define VDROP_FACTOR 0.8

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


// HARDWARE PINS SETTING
//
// do not change hardware pins below
// make sure you don't set SWDIO or SWDCLK pins (programming pins)
// if you do, you lose board programmability without a reset pin
//
// example: pin "PB2" ( port b , pin 2 )
// pin: GPIO_Pin_2
// port: GPIOB

// to disable led pins set number to zero

#define LED_NUMBER 4

#define LED1PIN GPIO_Pin_4
#define LED1PORT GPIOA

#define LED2PIN GPIO_Pin_2
#define LED2PORT GPIOA

#define LED3PIN GPIO_Pin_12
#define LED3PORT GPIOA

#define LED4PIN GPIO_Pin_0
#define LED4PORT GPIOB

// aux leds

#define AUX_LED_NUMBER 1

#define AUX_LED1PIN GPIO_Pin_2
#define AUX_LED1PORT GPIOB

#define AUX_LED2PIN GPIO_Pin_x
#define AUX_LED2PORT GPIOx

// softi2c pins definitons:
// sda - out/in , sck - out

// disable softi2c pins 
//#define DISABLE_SOFTI2C_PINS

#define SOFTI2C_SDAPIN GPIO_Pin_7
#define SOFTI2C_SDAPORT GPIOB

#define SOFTI2C_SCLPIN GPIO_Pin_6
#define SOFTI2C_SCLPORT GPIOB

// Analog battery input pin and adc channel

#define BATTERYPIN GPIO_Pin_7
#define BATTERYPORT GPIOA
#define BATTERY_ADC_CHANNEL ADC_Channel_6


// SPI PINS DEFINITONS ( for radio ic )
// MOSI , CLK , SS - outputs , MISO input

//disable pins so they don't interfere with other pins 
//#define DISABLE_SPI_PINS

#define SPI_MOSI_PIN GPIO_Pin_3
#define SPI_MOSI_PORT GPIOB

#define SPI_MISO_PIN GPIO_Pin_15
#define SPI_MISO_PORT GPIOA

#define SPI_CLK_PIN GPIO_Pin_4
#define SPI_CLK_PORT GPIOB

#define SPI_SS_PIN GPIO_Pin_5
#define SPI_SS_PORT GPIOB


// PWM PINS DEFINITIONS 
// currently pins PA0 to PA3 , PA5 , PA8 to PA11 supported


// pwm pins disable
// disable all pwm pins / function
//#define DISABLE_PWM_PINS

// pin initialization
// enable the pins to be used here ( multiple pins ok)
#define PWM_PA0
#define PWM_PA1
//#define PWM_PA2
//#define PWM_PA3
//#define PWM_PA5
#define PWM_PA8
#define PWM_PA9
//#define PWM_PA10
//#define PWM_PA11


// Assingment of pin to motor
// Assign one pin to one motor

// back-left motor
// motor 0 pin

#define MOTOR0_PIN_PA0
//#define MOTOR0_PIN_PA1
//#define MOTOR0_PIN_PA2
//#define MOTOR0_PIN_PA3
//#define MOTOR0_PIN_PA5
//#define MOTOR0_PIN_PA8
//#define MOTOR0_PIN_PA9
//#define MOTOR0_PIN_PA10
//#define MOTOR0_PIN_PA11

// front-left motor
// motor 1 pin

//#define MOTOR1_PIN_PA0
#define MOTOR1_PIN_PA1
//#define MOTOR1_PIN_PA2
//#define MOTOR1_PIN_PA3
//#define MOTOR1_PIN_PA8
//#define MOTOR1_PIN_PA9
//#define MOTOR1_PIN_PA10
//#define MOTOR1_PIN_PA11

// front-right motor
// motor 2 pin

//#define MOTOR2_PIN_PA0
//#define MOTOR2_PIN_PA1
//#define MOTOR2_PIN_PA2
//#define MOTOR2_PIN_PA3
#define MOTOR2_PIN_PA8
//#define MOTOR2_PIN_PA9
//#define MOTOR2_PIN_PA10
//#define MOTOR2_PIN_PA11

// back-right motor
// motor 3 pin

//#define MOTOR3_PIN_PA0
//#define MOTOR3_PIN_PA1
//#define MOTOR3_PIN_PA2
//#define MOTOR3_PIN_PA3
//#define MOTOR3_PIN_PA8
#define MOTOR3_PIN_PA9
//#define MOTOR3_PIN_PA10
//#define MOTOR3_PIN_PA11


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
// #define DEBUG

// disable the check for known gyro that causes the 4 times flash
// #define DISABLE_GYRO_CHECK


// enable motors if pitch / roll controls off center (at zero throttle)
// possible values: 0 / 1
#define ENABLESTIX 0


#pragma diag_warning 1035 , 177 , 4017
#pragma diag_error 260

//--fpmode=fast























