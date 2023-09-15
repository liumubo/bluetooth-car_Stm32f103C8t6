#include "stm32f10x.h"                  // Device header


void PWM_Init(uint8_t Mode)
{
	//��һ�� RCC����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	if(Mode == 1)
	{
		GPIO_InitTypeDef GPIOA_InitStructure;
		GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
		GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
		GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIOA_InitStructure);
		
		
		//�ڶ��� ѡ��ʱ����Ԫʱ��Դ
		TIM_InternalClockConfig(TIM2);
		
		//������ ����ʱ����Ԫ
		TIM_TimeBaseInitTypeDef TIMBase_InitStructure;
		TIMBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����Ƶ
		TIMBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIMBase_InitStructure.TIM_Period = 100-1; //ARR
		TIMBase_InitStructure.TIM_Prescaler = 720-1; //PSC  1000HzƵ������Ϊ1ms������PWM��������1ms
		TIMBase_InitStructure.TIM_RepetitionCounter = 0; //�ظ����������ò�����ֱ�Ӹ�0����
		TIM_TimeBaseInit(TIM2, &TIMBase_InitStructure);

		//���Ĳ� ��������Ƚϵ�Ԫ
		TIM_OCInitTypeDef TIM2OC1_InitStructure;
		TIM_OCStructInit(&TIM2OC1_InitStructure); //��ʼ���ṹ�����г�Ա����ֹʹ��TIM1�������߼���ʱ��ʱ���ṹ����û�����в�������ʼ�����³�������
		TIM2OC1_InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM2OC1_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //����ѡ�񣬸ߵ�ƽ��Ч��������ת
		TIM2OC1_InitStructure.TIM_OutputState = TIM_OutputState_Enable; //���ʹ��
		TIM2OC1_InitStructure.TIM_Pulse = 0; //CCR
		TIM_OC1Init(TIM2, &TIM2OC1_InitStructure);

		TIM_OC2Init(TIM2, &TIM2OC1_InitStructure);
		TIM_OC3Init(TIM2, &TIM2OC1_InitStructure);
		TIM_OC4Init(TIM2, &TIM2OC1_InitStructure);
		
		
		// ���п���
		TIM_Cmd(TIM2, ENABLE);
	}
	if(Mode == 2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		GPIO_InitTypeDef GPIOA_InitStructure;
		GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
		GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIOA_InitStructure);
		
		//�ڶ��� ѡ��ʱ����Ԫʱ��Դ
		TIM_InternalClockConfig(TIM3);
		
		
		//������ ����ʱ����Ԫ
		TIM_TimeBaseInitTypeDef TIMBase_InitStructure;
		TIMBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����Ƶ
		TIMBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIMBase_InitStructure.TIM_Period = 20000-1; //ARR
		TIMBase_InitStructure.TIM_Prescaler = 72-1; //PSC 50HzƵ������Ϊ1ms������PWM��������1ms 1/20ms = (1/20*0.001) = 50Hz
		TIMBase_InitStructure.TIM_RepetitionCounter = 0; //�ظ����������ò�����ֱ�Ӹ�0����
		TIM_TimeBaseInit(TIM3, &TIMBase_InitStructure);
		
		
		//���Ĳ� ��������Ƚϵ�Ԫ
		TIM_OCInitTypeDef TIM3OC_InitStructure;
		TIM_OCStructInit(&TIM3OC_InitStructure); //��ʼ���ṹ�����г�Ա����ֹʹ��TIM1�������߼���ʱ��ʱ���ṹ����û�����в�������ʼ�����³�������
		TIM3OC_InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM3OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //����ѡ�񣬸ߵ�ƽ��Ч��������ת
		TIM3OC_InitStructure.TIM_OutputState = TIM_OutputState_Enable; //���ʹ��
		TIM3OC_InitStructure.TIM_Pulse = 500; //CCR
		TIM_OC1Init(TIM3, &TIM3OC_InitStructure); //ͨ��1
		
		TIM_OC2Init(TIM3, &TIM3OC_InitStructure); //ͨ��2
		
		

		
		
		// ���п���
		TIM_Cmd(TIM3, ENABLE);
	}
	
	
	
	
	
	
}

void PWM_SetCompare1(uint16_t CompareNum)
{
	TIM_SetCompare1(TIM2, CompareNum); //�������������CCR��ֵ
}

void PWM_SetCompare2(uint16_t CompareNum)
{
	TIM_SetCompare2(TIM2, CompareNum); //�������������CCR��ֵ
}

void PWM_SetCompare3(uint16_t CompareNum)
{
	TIM_SetCompare3(TIM2, CompareNum); //�������������CCR��ֵ
}

void PWM_SetCompare4(uint16_t CompareNum)
{
	TIM_SetCompare4(TIM2, CompareNum); //�������������CCR��ֵ
}

void PWM_TIM3_SetCompare1(uint16_t CompareNum)
{
	TIM_SetCompare1(TIM3, CompareNum); //�������������CCR��ֵ
}

void PWM_TIM3_SetCompare2(uint16_t CompareNum)
{
	TIM_SetCompare2(TIM3, CompareNum); //�������������CCR��ֵ
}