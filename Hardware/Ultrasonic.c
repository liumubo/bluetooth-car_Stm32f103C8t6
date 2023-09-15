#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Ultrasonic.h"
#include "Timer.h"
#include "OLED.h"

uint16_t Time;//������뼶����

void Ultrasonic_Init(void)
{
	//��һ�� RCC����ʱ�� ������ʱ��1��GPIO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//�ڶ��� ��ʼ��GPIO
	GPIO_InitTypeDef GPIOB_InitStructure;
	GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOB_InitStructure.GPIO_Pin = Trig_Pin;
	GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIOB_InitStructure);
	
	
	
	GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIOB_InitStructure.GPIO_Pin = Echo_Pin;
	GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIOB_InitStructure);
	
	GPIO_WriteBit(GPIOB, Trig_Pin, Bit_RESET);//��ֹ�ϵ�󴥷��ź�
	
}

void Ultrasonic_Start()
{
	GPIO_SetBits(GPIOB, Trig_Pin);
	Delay_us(45);
	GPIO_ResetBits(GPIOB, Trig_Pin);
	Timer_Init();
}

uint16_t Ultrasonic_GetValue()
{
	Ultrasonic_Start();
	Delay_ms(150); 
	// OLED_ShowNum(1,1,Time,5);
	return ( (Time*0.0001) *34000)/2; //10000��Time��Ӧ1.7cm
}


