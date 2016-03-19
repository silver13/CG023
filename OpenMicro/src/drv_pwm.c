
#include "project.h"
#include "drv_pwm.h"
#include "config.h"

void init_timer( TIM_TypeDef* TIMx , int period);

TIM_OCInitTypeDef  TIM_OCInitStructure;

#define PWMTOP 2999

void pwm_init(void)
{


	
	GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_2);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_2);

// timer clock enable 
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);  // PA8 , PA9
	
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);		// PA0 , PA1
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6 , ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7 , ENABLE);


	init_timer( TIM2 , 2999);
	
	init_timer( TIM1 , 2999);


  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;

  TIM_OCInitStructure.TIM_Pulse = 0;
	
  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);

  TIM_Cmd(TIM2, ENABLE);

  TIM_CtrlPWMOutputs(TIM2, ENABLE);
	
	TIM_Cmd(TIM1, ENABLE);

  TIM_CtrlPWMOutputs(TIM1, ENABLE);
 
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
int debugpwm[4];
#include  <math.h>

void pwm_set( uint8_t number , float pwm)
{
	pwm = pwm * PWMTOP ;
	
	if ( pwm < 0 ) pwm = 0;
  if ( pwm > PWMTOP ) pwm = PWMTOP;
	
	pwm = lroundf(pwm);
	
	TIM_OCInitStructure.TIM_Pulse = pwm;
	debugpwm[number] = (uint32_t) pwm;
	
  switch( number)
	{
		case 0:
		  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
		break;
		
		case 1:
		  TIM_OC2Init(TIM2, &TIM_OCInitStructure);
		break;
		
		case 2:
		  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
		break;
		
		case 3: 
		  TIM_OC2Init(TIM1, &TIM_OCInitStructure);
		break;
		
		default:
			// handle error;
			//
		break;	
				
	}
	
}











