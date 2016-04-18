
// HARDWARE PINS SETTING
//
// do not change hardware pins below
// make sure you don't set SWDIO or SWDCLK pins (programming pins)
// if you do, you lose board programmability without a reset pin
//
// example: pin "PB2" ( port b , pin 2 )
// pin: GPIO_Pin_2
// port: GPIOB


// setting procedure:
// set led number zero, led aux number zero
// uncomment DISABLE_SOFTI2C_PINS , DISABLE_SPI_PINS and DISABLE_PWM_PINS
// this will prevent any pins to be used, so that there are no conflicts
// set pins starting with leds, in order to see working status
// do not set PA13 , PA14 (stm32f031) as this will break the programming interface
// to disable led pins set number to zero

#define LED_NUMBER 3

#define LED1PIN GPIO_Pin_6
#define LED1PORT GPIOB

#define LED2PIN GPIO_Pin_1
#define LED2PORT GPIOB

#define LED3PIN GPIO_Pin_4
#define LED3PORT GPIOB

// ??
//#define LED4PIN GPIO_Pin_0
//#define LED4PORT GPIOA

// aux leds

#define AUX_LED_NUMBER 0

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

#define SOFTI2C_SCLPIN GPIO_Pin_8
#define SOFTI2C_SCLPORT GPIOB

//#define SOFTI2C_GYRO_ADDRESS 0x68
#define SOFTI2C_GYRO_ADDRESS 0x69

// Analog battery input pin and adc channel

#define BATTERYPIN GPIO_Pin_2
#define BATTERYPORT GPIOA
#define BATTERY_ADC_CHANNEL ADC_Channel_2


// SPI PINS DEFINITONS ( for radio ic )
// MOSI , CLK , SS - outputs , MISO input

//disable pins so they don't interfere with other pins 
//#define DISABLE_SPI_PINS

#define SPI_MISO_PIN GPIO_Pin_6
#define SPI_MISO_PORT GPIOA

#define SPI_MOSI_PIN GPIO_Pin_7
#define SPI_MOSI_PORT GPIOA

#define SPI_CLK_PIN GPIO_Pin_5
#define SPI_CLK_PORT GPIOA

#define SPI_SS_PIN GPIO_Pin_4
#define SPI_SS_PORT GPIOA


// PWM PINS DEFINITIONS 
// currently pins PA0 to PA3 , PA5 , PA8 to PA11 supported


// pwm pins disable
// disable all pwm pins / function
//#define DISABLE_PWM_PINS

// pwm pin initialization
// enable the pwm pins to be used here ( multiple pins ok)
//#define PWM_PA0
//#define PWM_PA1
//#define PWM_PA2
//#define PWM_PA3
//#define PWM_PA5
#define PWM_PA8
#define PWM_PA9
#define PWM_PA10
#define PWM_PA11


// Assingment of pin to motor
// Assign one pin to one motor

// back-left motor
// motor 0 pin

//#define MOTOR0_PIN_PA0
//#define MOTOR0_PIN_PA1
//#define MOTOR0_PIN_PA2
//#define MOTOR0_PIN_PA3
//#define MOTOR0_PIN_PA5
//#define MOTOR0_PIN_PA8
#define MOTOR0_PIN_PA9
//#define MOTOR0_PIN_PA10
//#define MOTOR0_PIN_PA11

// front-left motor
// motor 1 pin

//#define MOTOR1_PIN_PA0
//#define MOTOR1_PIN_PA1
//#define MOTOR1_PIN_PA2
//#define MOTOR1_PIN_PA3
#define MOTOR1_PIN_PA8
//#define MOTOR1_PIN_PA9
//#define MOTOR1_PIN_PA10
//#define MOTOR1_PIN_PA11

// front-right motor
// motor 2 pin

//#define MOTOR2_PIN_PA0
//#define MOTOR2_PIN_PA1
//#define MOTOR2_PIN_PA2
//#define MOTOR2_PIN_PA3
//#define MOTOR2_PIN_PA8
//#define MOTOR2_PIN_PA9
//#define MOTOR2_PIN_PA10
#define MOTOR2_PIN_PA11

// back-right motor
// motor 3 pin

//#define MOTOR3_PIN_PA0
//#define MOTOR3_PIN_PA1
//#define MOTOR3_PIN_PA2
//#define MOTOR3_PIN_PA3
//#define MOTOR3_PIN_PA8
//#define MOTOR3_PIN_PA9
#define MOTOR3_PIN_PA10
//#define MOTOR3_PIN_PA11


// gyro orientation
// the expected orientation is with the dot in the front-left corner
// use this to rotate to the correct orientation 
//#define SENSOR_ROTATE_90_CW
//#define SENSOR_ROTATE_90_CCW
#define SENSOR_ROTATE_180




