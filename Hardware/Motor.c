#include "stm32f10x.h"                  // Device header
#include "PWM.h"

#define MOTOR_MODE			1

void Motor_Init(void)
{
	PWM_Init(MOTOR_MODE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIOA_InitStructure;
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�������
	GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_4;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIOA_InitStructure);
	
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�������
	GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_11;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIOA_InitStructure);
	
	
	GPIO_InitTypeDef GPIOB_InitStructure;
	GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�������
	GPIOB_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIOB_InitStructure);
	
	GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�������
	GPIOB_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIOB_InitStructure);
	
}

void Motor_SetSpeed(int8_t Speed)
{
	if(Speed >= 0)
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
		GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);
		PWM_SetCompare1(Speed);
	}
	else
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
		GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);
		PWM_SetCompare1(-Speed);
	}
}

void Motor_SetSpeed2(int8_t Speed)
{
	if(Speed >= 0)
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_RESET);
		GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
		PWM_SetCompare2(Speed);
	}
	else
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_11, Bit_SET);
		GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
		PWM_SetCompare2(-Speed);
	}
}

void Motor_SetSpeed3(int8_t Speed)
{
	if(Speed >= 0)
	{
		GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_RESET);
		GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET);
		PWM_SetCompare3(Speed);
	}
	else
	{
		GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_SET);
		GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET);
		PWM_SetCompare3(-Speed);
	}
}

void Motor_SetSpeed4(int8_t Speed)
{
	if(Speed >= 0)
	{
		GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
		GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
		PWM_SetCompare4(Speed);
	}
	else
	{
		GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
		GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
		PWM_SetCompare4(-Speed);
	}
}