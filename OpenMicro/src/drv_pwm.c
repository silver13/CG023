
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
#define PWMTOP 5999

#define PWM_PA0
#define PWM_PA1
#define PWM_PA8
#define PWM_PA9

#ifdef PWM_PA0
#define PWM1_PIN GPIO_Pin_0
#define PWM1_PORT GPIOA
#define PWM1_AF GPIO_AF_2
#define PWM1_PINSOURCE GPIO_PinSource0
#define PWM1_TIMER TIM2
#ifndef ENABLE_TIM2 
	#define ENABLE_TIM2
#endif
#endif

#ifdef PWM_PA1
#define PWM2_PIN GPIO_Pin_1
#define PWM2_PORT GPIOA
#define PWM2_AF GPIO_AF_2
#define PWM2_PINSOURCE GPIO_PinSource1
#define PWM2_TIMER TIM2
#ifndef ENABLE_TIM2 
	#define ENABLE_TIM2
#endif
#endif

#ifdef PWM_PA8
#define PWM3_PIN GPIO_Pin_8
#define PWM3_PORT GPIOA
#define PWM3_AF GPIO_AF_2
#define PWM3_PINSOURCE GPIO_PinSource8
#define PWM3_TIMER TIM1
#ifndef ENABLE_TIM1 
	#define ENABLE_TIM1
#endif
#endif

#ifdef PWM_PA9
#define PWM4_PIN GPIO_Pin_9
#define PWM4_PORT GPIOA
#define PWM4_AF GPIO_AF_2
#define PWM4_PINSOURCE GPIO_PinSource9
#define PWM4_TIMER TIM1
#ifndef ENABLE_TIM1 
	#define ENABLE_TIM1
#endif
#endif

void pwm_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;

	
	GPIO_InitStructure.GPIO_Pin = PWM1_PIN;
	GPIO_Init(PWM1_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PWM2_PIN;
	GPIO_Init(PWM2_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PWM3_PIN;
	GPIO_Init(PWM3_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PWM4_PIN;
	GPIO_Init(PWM4_PORT, &GPIO_InitStructure);
	
  GPIO_PinAFConfig(GPIOA, PWM1_PINSOURCE, PWM1_AF);
	
	GPIO_PinAFConfig(GPIOA, PWM2_PINSOURCE, PWM2_AF);
	
  GPIO_PinAFConfig(GPIOA, PWM3_PINSOURCE, PWM3_AF);
	
	GPIO_PinAFConfig(GPIOA, PWM4_PINSOURCE, PWM4_AF);
	

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
	
  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);

	
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
//int debugpwm[4];
#include  <math.h>

void pwm_set( uint8_t number , float pwm)
{
	pwm = pwm * PWMTOP ;
	
	if ( pwm < 0 ) pwm = 0;
  if ( pwm > PWMTOP ) pwm = PWMTOP;
	
	pwm = lroundf(pwm);
	
	TIM_OCInitStructure.TIM_Pulse = pwm;
//	debugpwm[number] = (uint32_t) pwm;
	
  switch( number)
	{
		case 0:
		  //TIM_OC1Init(TIM2, &TIM_OCInitStructure);
		  TIM2->CCR1 = (uint16_t) pwm;
		break;
		
		case 1:
		  //TIM_OC2Init(TIM2, &TIM_OCInitStructure);
		  TIM2->CCR2 = (uint16_t) pwm;
		break;
		
		case 2:
		  //TIM_OC1Init(TIM1, &TIM_OCInitStructure);
		  TIM1->CCR1 = (uint16_t) pwm;
		break;
		
		case 3: 
		  //TIM_OC2Init(TIM1, &TIM_OCInitStructure);
		  TIM1->CCR2 = (uint16_t) pwm;
		break;
		
		default:
			// handle error;
			//
		break;	
				
	}
	
}





