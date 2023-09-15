#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/**
  * @brief 初始化按键设�? 
  * @param  �?
  * @retval �?
  */
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief  按下按键获取对应的键�?
  * @param  �?
  * @retval 返回值是按键的键�?
  */

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0; //没有按键按下时返�?0
	
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
	{
//		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0);
//		Delay_ms(20);
		
		KeyNum = 1;
	}
	
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
	{
//		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0);
//		Delay_ms(20);
		
		KeyNum = 2;
	}
	
	return KeyNum;
}