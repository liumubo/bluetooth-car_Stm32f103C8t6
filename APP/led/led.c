#include "led.h"

/*******************************************************************************
* 函 数 名         : LED_Init
* 函数功能		   : LED初始化函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
	
	RCC_APB2PeriphClockCmd(LED1_PORT_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=LED1_PIN;  //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //设置推挽输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
	GPIO_Init(LED1_PORT,&GPIO_InitStructure); 	   /* 初始化GPIO */
	GPIO_SetBits(LED1_PORT,LED1_PIN);   //将LED端口拉高，熄灭所有LED
	
	GPIO_InitStructure.GPIO_Pin=LED2_PIN;  //选择你要设置的IO口
	GPIO_Init(LED2_PORT,&GPIO_InitStructure); 	   /* 初始化GPIO */
	GPIO_SetBits(LED2_PORT,LED2_PIN);   //将LED端口拉高，熄灭所有LED
}
