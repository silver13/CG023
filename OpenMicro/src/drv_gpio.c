#include "project.h"
#include "drv_gpio.h"
#include "config.h"

void gpio_init(void) {
// clocks on to all ports			
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);

#if ( LED_NUMBER > 0|| AUX_LED_NUMBER > 0 )
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

#endif

#if ( LED_NUMBER > 0 )
    GPIO_InitStructure.GPIO_Pin = LED1PIN;
    GPIO_Init(LED1PORT, &GPIO_InitStructure);
#if ( LED_NUMBER > 1 )	
    GPIO_InitStructure.GPIO_Pin = LED2PIN;
    GPIO_Init(LED2PORT, &GPIO_InitStructure);
#if ( LED_NUMBER > 2 )	
    GPIO_InitStructure.GPIO_Pin = LED3PIN;
    GPIO_Init(LED3PORT, &GPIO_InitStructure);
#if ( LED_NUMBER > 3 )	
    GPIO_InitStructure.GPIO_Pin = LED4PIN;
    GPIO_Init(LED4PORT, &GPIO_InitStructure);
#endif
#endif
#endif
#endif

#if ( AUX_LED_NUMBER > 0 )
    GPIO_InitStructure.GPIO_Pin = AUX_LED1PIN;
    GPIO_Init(AUX_LED1PORT, &GPIO_InitStructure);
#endif	
#if ( AUX_LED_NUMBER > 1 )
    GPIO_InitStructure.GPIO_Pin = AUX_LED2PIN;
    GPIO_Init(AUX_LED2PORT, &GPIO_InitStructure);
#endif	

}

