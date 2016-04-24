
#include "project.h"
#include "drv_pwm.h"
#include "config.h"

void init_timer( TIM_TypeDef* TIMx , int period);

TIM_OCInitTypeDef  TIM_OCInitStructure;

// 16K
//#define PWMTOP 2999

// 32K not working
//#define PWMTOP 1499

// 8K
//#define PWMTOP 5999

//#define PWMFREQ 8000

#define PWMTOP ((48000000/PWMFREQ ) - 1)

#if ( PWMTOP< 1400 ) 
	#undef PWMTOP
	#define PWMTOP 6000
	#warning PWM FREQUENCY TOO HIGH
#endif

#if ( PWMTOP> 65535 ) 
	#undef PWMTOP
	#define PWMTOP 6000
	#warning PWM FREQUENCY TOO LOW
#endif


#ifdef PWM_PA0
#define PWM_PA0_PIN GPIO_Pin_0
#define PWM_PA0_PORT GPIOA
#define PWM_PA0_AF GPIO_AF_2
#define PWM_PA0_PINSOURCE GPIO_PinSource0
#define PWM_PA0_TIMER TIM2
#define PWM_PA0_CHANNEL CH1
#ifndef ENABLE_TIM2 
	#define ENABLE_TIM2
#endif
#endif

#ifdef PWM_PA1
#define PWM_PA1_PIN GPIO_Pin_1
#define PWM_PA1_PORT GPIOA
#define PWM_PA1_AF GPIO_AF_2
#define PWM_PA1_PINSOURCE GPIO_PinSource1
#define PWM_PA1_TIMER TIM2
#define PWM_PA1_CHANNEL CH2
#ifndef ENABLE_TIM2 
	#define ENABLE_TIM2
#endif
#endif


#ifdef PWM_PA2
#define PWM_PA2_PIN GPIO_Pin_2
#define PWM_PA2_PORT GPIOA
#define PWM_PA2_AF GPIO_AF_2
#define PWM_PA2_PINSOURCE GPIO_PinSource2
#define PWM_PA2_TIMER TIM2
#define PWM_PA2_CHANNEL CH3
#ifndef ENABLE_TIM2 
	#define ENABLE_TIM2
#endif
#endif


#ifdef PWM_PA3
#define PWM_PA3_PIN GPIO_Pin_3
#define PWM_PA3_PORT GPIOA
#define PWM_PA3_AF GPIO_AF_2
#define PWM_PA3_PINSOURCE GPIO_PinSource3
#define PWM_PA3_TIMER TIM2
#define PWM_PA3_CHANNEL CH4
#ifndef ENABLE_TIM2 
	#define ENABLE_TIM2
#endif
#endif


#ifdef PWM_PA5
#define PWM_PA5_PIN GPIO_Pin_5
#define PWM_PA5_PORT GPIOA
#define PWM_PA5_AF GPIO_AF_2
#define PWM_PA5_PINSOURCE GPIO_PinSource5
#define PWM_PA5_TIMER TIM2
#define PWM_PA5_CHANNEL CH1
#ifndef ENABLE_TIM2 
	#define ENABLE_TIM2
#endif
#endif


#ifdef PWM_PA8
#define PWM_PA8_PIN GPIO_Pin_8
#define PWM_PA8_PORT GPIOA
#define PWM_PA8_AF GPIO_AF_2
#define PWM_PA8_PINSOURCE GPIO_PinSource8
#define PWM_PA8_TIMER TIM1
#define PWM_PA8_CHANNEL CH1
#ifndef ENABLE_TIM1 
	#define ENABLE_TIM1
#endif
#endif

#ifdef PWM_PA9
#define PWM_PA9_PIN GPIO_Pin_9
#define PWM_PA9_PORT GPIOA
#define PWM_PA9_AF GPIO_AF_2
#define PWM_PA9_PINSOURCE GPIO_PinSource9
#define PWM_PA9_TIMER TIM1
#define PWM_PA9_CHANNEL CH2
#ifndef ENABLE_TIM1 
	#define ENABLE_TIM1
#endif
#endif


#ifdef PWM_PA10
#define PWM_PA10_PIN GPIO_Pin_10
#define PWM_PA10_PORT GPIOA
#define PWM_PA10_AF GPIO_AF_2
#define PWM_PA10_PINSOURCE GPIO_PinSource10
#define PWM_PA10_TIMER TIM1
#define PWM_PA10_CHANNEL CH3
#ifndef ENABLE_TIM1 
	#define ENABLE_TIM1
#endif
#endif

#ifdef PWM_PA11
#define PWM_PA11_PIN GPIO_Pin_11
#define PWM_PA11_PORT GPIOA
#define PWM_PA11_AF GPIO_AF_2
#define PWM_PA11_PINSOURCE GPIO_PinSource11
#define PWM_PA11_TIMER TIM1
#define PWM_PA11_CHANNEL CH4
#ifndef ENABLE_TIM1 
	#define ENABLE_TIM1
#endif
#endif




#ifndef DISABLE_PWM_PINS
void pwm_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;

	
// timer clock enable 
#ifdef ENABLE_TIM1	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);  // PA8 , PA9
	
	init_timer( TIM1 , PWMTOP);
	
#endif	
#ifdef ENABLE_TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);		// PA0 , PA1
		
	init_timer( TIM2 , PWMTOP);
#endif
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);

	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6 , ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7 , ENABLE);



  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;

  TIM_OCInitStructure.TIM_Pulse = 0;
	

#ifdef PWM_PA0
  GPIO_InitStructure.GPIO_Pin = PWM_PA0_PIN;
	GPIO_Init(PWM_PA0_PORT, &GPIO_InitStructure);

  GPIO_PinAFConfig(PWM_PA0_PORT, PWM_PA0_PINSOURCE, PWM_PA0_AF);
	
#if (PWM_PA0_CHANNEL==CH1) 
  TIM_OC1Init(PWM_PA0_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA0_CHANNEL==CH2) 
  TIM_OC2Init(PWM1_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA0_CHANNEL==CH3) 
  TIM_OC3Init(PWM_PA0_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA0_CHANNEL==CH4) 
  TIM_OC4Init(PWM_PA0_TIMER, &TIM_OCInitStructure);
#endif

#endif


#ifdef PWM_PA1
  GPIO_InitStructure.GPIO_Pin = PWM_PA1_PIN;
	GPIO_Init(PWM_PA1_PORT, &GPIO_InitStructure);

  GPIO_PinAFConfig(PWM_PA1_PORT, PWM_PA1_PINSOURCE, PWM_PA1_AF);
	
#if (PWM_PA1_CHANNEL==CH1) 
  TIM_OC1Init(PWM_PA1_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA1_CHANNEL==CH2) 
  TIM_OC2Init(PWM_PA1_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA1_CHANNEL==CH3) 
  TIM_OC3Init(PWM_PA1_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA1_CHANNEL==CH4) 
  TIM_OC4Init(PWM_PA1_TIMER, &TIM_OCInitStructure);
#endif
#endif


#ifdef PWM_PA2
  GPIO_InitStructure.GPIO_Pin = PWM_PA2_PIN;
	GPIO_Init(PWM_PA2_PORT, &GPIO_InitStructure);

  GPIO_PinAFConfig(PWM_PA2_PORT, PWM_PA2_PINSOURCE, PWM_PA2_AF);
	
#if (PWM_PA2_CHANNEL==CH1) 
  TIM_OC1Init(PWM_PA2_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA2_CHANNEL==CH2) 
  TIM_OC2Init(PWM_PA2_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA2_CHANNEL==CH3) 
  TIM_OC3Init(PWM_PA2_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA2_CHANNEL==CH4) 
  TIM_OC4Init(PWM_PA2_TIMER, &TIM_OCInitStructure);
#endif
#endif


#ifdef PWM_PA3
  GPIO_InitStructure.GPIO_Pin = PWM_PA3_PIN;
	GPIO_Init(PWM_PA3_PORT, &GPIO_InitStructure);

  GPIO_PinAFConfig(PWM_PA3_PORT, PWM_PA3_PINSOURCE, PWM_PA3_AF);
	
#if (PWM_PA3_CHANNEL==CH1) 
  TIM_OC1Init(PWM_PA3_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA3_CHANNEL==CH2) 
  TIM_OC2Init(PWM_PA3_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA3_CHANNEL==CH3) 
  TIM_OC3Init(PWM_PA3_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA3_CHANNEL==CH4) 
  TIM_OC4Init(PWM_PA3_TIMER, &TIM_OCInitStructure);
#endif
#endif


#ifdef PWM_PA5
  GPIO_InitStructure.GPIO_Pin = PWM_PA5_PIN;
	GPIO_Init(PWM_PA5_PORT, &GPIO_InitStructure);

  GPIO_PinAFConfig(PWM_PA5_PORT, PWM_PA5_PINSOURCE, PWM_PA5_AF);
	
#if (PWM_PA5_CHANNEL==CH1) 
  TIM_OC1Init(PWM_PA5_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA5_CHANNEL==CH2) 
  TIM_OC2Init(PWM_PA5_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA5_CHANNEL==CH3) 
  TIM_OC3Init(PWM_PA5_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA5_CHANNEL==CH4) 
  TIM_OC4Init(PWM_PA5_TIMER, &TIM_OCInitStructure);
#endif
#endif


#ifdef PWM_PA8
  GPIO_InitStructure.GPIO_Pin = PWM_PA8_PIN;
	GPIO_Init(PWM_PA8_PORT, &GPIO_InitStructure);

  GPIO_PinAFConfig(PWM_PA8_PORT, PWM_PA8_PINSOURCE, PWM_PA8_AF);
	
#if (PWM_PA8_CHANNEL==CH1) 
  TIM_OC1Init(PWM_PA8_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA8_CHANNEL==CH2) 
  TIM_OC2Init(PWM_PA8_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA8_CHANNEL==CH3) 
  TIM_OC3Init(PWM_PA8_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA8_CHANNEL==CH4) 
  TIM_OC4Init(PWM_PA8_TIMER, &TIM_OCInitStructure);
#endif
#endif


#ifdef PWM_PA9
  GPIO_InitStructure.GPIO_Pin = PWM_PA9_PIN;
	GPIO_Init(PWM_PA9_PORT, &GPIO_InitStructure);

  GPIO_PinAFConfig(PWM_PA9_PORT, PWM_PA9_PINSOURCE, PWM_PA9_AF);
	
#if (PWM_PA9_CHANNEL==CH1) 
  TIM_OC1Init(PWM_PA9_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA9_CHANNEL==CH2) 
  TIM_OC2Init(PWM_PA9_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA9_CHANNEL==CH3) 
  TIM_OC3Init(PWM_PA9_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA9_CHANNEL==CH4) 
  TIM_OC4Init(PWM_PA9_TIMER, &TIM_OCInitStructure);
#endif
#endif



#ifdef PWM_PA10
  GPIO_InitStructure.GPIO_Pin = PWM_PA10_PIN;
	GPIO_Init(PWM_PA10_PORT, &GPIO_InitStructure);

  GPIO_PinAFConfig(PWM_PA10_PORT, PWM_PA10_PINSOURCE, PWM_PA10_AF);
	
#if (PWM_PA10_CHANNEL==CH1) 
  TIM_OC1Init(PWM_PA10_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA10_CHANNEL==CH2) 
  TIM_OC2Init(PWM_PA10_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA10_CHANNEL==CH3) 
  TIM_OC3Init(PWM_PA10_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA10_CHANNEL==CH4) 
  TIM_OC4Init(PWM_PA10_TIMER, &TIM_OCInitStructure);
#endif
#endif



#ifdef PWM_PA11
  GPIO_InitStructure.GPIO_Pin = PWM_PA11_PIN;
	GPIO_Init(PWM_PA11_PORT, &GPIO_InitStructure);

  GPIO_PinAFConfig(PWM_PA11_PORT, PWM_PA11_PINSOURCE, PWM_PA11_AF);
	
#if (PWM_PA11_CHANNEL==CH1) 
  TIM_OC1Init(PWM_PA11_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA11_CHANNEL==CH2) 
  TIM_OC2Init(PWM_PA11_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA11_CHANNEL==CH3) 
  TIM_OC3Init(PWM_PA11_TIMER, &TIM_OCInitStructure);
#elif (PWM_PA11_CHANNEL==CH4) 
  TIM_OC4Init(PWM_PA11_TIMER, &TIM_OCInitStructure);
#endif
#endif



	
#ifdef ENABLE_TIM1	
	TIM_Cmd(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
#endif 


#ifdef ENABLE_TIM2
  TIM_Cmd(TIM2, ENABLE);
  TIM_CtrlPWMOutputs(TIM2, ENABLE);
#endif

}

void init_timer( TIM_TypeDef* TIMx , int period)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = period;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit( TIMx, &TIM_TimeBaseStructure);
	
}

#include  <math.h>

void pwm_set( uint8_t number , float pwmf)
{
	int pwm = pwmf * PWMTOP ;
	
	if ( pwm < 0 ) pwm = 0;
  if ( pwm > PWMTOP ) pwm = PWMTOP;
	
	TIM_OCInitStructure.TIM_Pulse = pwm;
	
  switch( number)
	{
		case 0:
			#ifdef MOTOR0_PIN_PA0
			TIM2->CCR1 = (uint16_t) pwm;
			#endif
			#ifdef MOTOR0_PIN_PA1
			TIM2->CCR2 = (uint16_t) pwm;
			#endif
		  #ifdef MOTOR0_PIN_PA2
			TIM2->CCR3 = (uint16_t) pwm;
			#endif
		  #ifdef MOTOR0_PIN_PA3
			TIM2->CCR4 = (uint16_t) pwm;
			#endif
		  #ifdef MOTOR0_PIN_PA5
			TIM2->CCR1 = (uint16_t) pwm;
			#endif
			#ifdef MOTOR0_PIN_PA8
			TIM1->CCR1 = (uint16_t) pwm;
			#endif
			#ifdef MOTOR0_PIN_PA9
			TIM1->CCR2 = (uint16_t) pwm;
			#endif	
			#ifdef MOTOR0_PIN_PA10
			TIM1->CCR3 = (uint16_t) pwm;
			#endif	
			#ifdef MOTOR0_PIN_PA11
			TIM1->CCR4 = (uint16_t) pwm;
			#endif			
		break;
		
		case 1:
			#ifdef MOTOR1_PIN_PA0
			TIM2->CCR1 = (uint16_t) pwm;
			#endif
			#ifdef MOTOR1_PIN_PA1
			TIM2->CCR2 = (uint16_t) pwm;
		  #endif
		  #ifdef MOTOR1_PIN_PA2
			TIM2->CCR3 = (uint16_t) pwm;
			#endif
		  #ifdef MOTOR1_PIN_PA3
			TIM2->CCR4 = (uint16_t) pwm;
			#endif
		  #ifdef MOTOR1_PIN_PA5
			TIM2->CCR1 = (uint16_t) pwm;
			#endif
			#ifdef MOTOR1_PIN_PA8
			TIM1->CCR1 = (uint16_t) pwm;
			#endif
			#ifdef MOTOR1_PIN_PA9
			TIM1->CCR2 = (uint16_t) pwm;
			#endif		
		  #ifdef MOTOR1_PIN_PA10
			TIM1->CCR3 = (uint16_t) pwm;
			#endif	
			#ifdef MOTOR1_PIN_PA11
			TIM1->CCR4 = (uint16_t) pwm;
			#endif	
		break;
		
		case 2:
			#ifdef MOTOR2_PIN_PA0
			TIM2->CCR1 = (uint16_t) pwm;
			#endif
			#ifdef MOTOR2_PIN_PA1
			TIM2->CCR2 = (uint16_t) pwm;
			#endif
		  #ifdef MOTOR2_PIN_PA2
			TIM2->CCR3 = (uint16_t) pwm;
			#endif
		  #ifdef MOTOR2_PIN_PA3
			TIM2->CCR4 = (uint16_t) pwm;
			#endif
			#ifdef MOTOR2_PIN_PA5
			TIM2->CCR1 = (uint16_t) pwm;
			#endif
			#ifdef MOTOR2_PIN_PA8
			TIM1->CCR1 = (uint16_t) pwm;
			#endif
			#ifdef MOTOR2_PIN_PA9
			TIM1->CCR2 = (uint16_t) pwm;
			#endif	
			#ifdef MOTOR2_PIN_PA10
			TIM1->CCR3 = (uint16_t) pwm;
			#endif	
			#ifdef MOTOR2_PIN_PA11
			TIM1->CCR4 = (uint16_t) pwm;
			#endif			
		break;
		
		case 3: 
			#ifdef MOTOR3_PIN_PA0
			TIM2->CCR1 = (uint16_t) pwm;
			#endif
			#ifdef MOTOR3_PIN_PA1
			TIM2->CCR2 = (uint16_t) pwm;
			#endif
		  #ifdef MOTOR3_PIN_PA2
			TIM2->CCR3 = (uint16_t) pwm;
			#endif
		  #ifdef MOTOR3_PIN_PA3
			TIM2->CCR4 = (uint16_t) pwm;
			#endif
		  #ifdef MOTOR3_PIN_PA5
			TIM2->CCR1 = (uint16_t) pwm;
			#endif
			#ifdef MOTOR3_PIN_PA8
			TIM1->CCR1 = (uint16_t) pwm;
			#endif
			#ifdef MOTOR3_PIN_PA9
			TIM1->CCR2 = (uint16_t) pwm;
			#endif		
		  #ifdef MOTOR3_PIN_PA10
			TIM1->CCR3 = (uint16_t) pwm;
			#endif	
			#ifdef MOTOR3_PIN_PA11
			TIM1->CCR4 = (uint16_t) pwm;
			#endif	
		break;
		
		default:
			// handle error;
			//
		break;	
				
	}
	
}







#else
// pwm pins disabled
void pwm_init(void)
{
}

void pwm_set( uint8_t number , float pwm)
{
}

#endif




