#include "project.h"
#include "drv_gpio.h"
#include "config.h"

void gpio_init(void)
{
// clocks on to all ports		


  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB , ENABLE);
  

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_InitStructure.GPIO_Pin = LED1PIN;	
  GPIO_Init(LED1PORT, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = LED2PIN;	
  GPIO_Init(LED2PORT, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = LED3PIN;	
  GPIO_Init(LED3PORT, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = LED4PIN;	
  GPIO_Init(LED4PORT, &GPIO_InitStructure); 

	
}


