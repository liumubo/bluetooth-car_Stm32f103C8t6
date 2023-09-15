#include "stm32f10x.h"                  // Device header


void PWM_Init(uint8_t Mode)
{
	//第一步 RCC开启时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	if(Mode == 1)
	{
		GPIO_InitTypeDef GPIOA_InitStructure;
		GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
		GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
		GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIOA_InitStructure);
		
		
		//第二步 选择时基单元时钟源
		TIM_InternalClockConfig(TIM2);
		
		//第三步 配置时基单元
		TIM_TimeBaseInitTypeDef TIMBase_InitStructure;
		TIMBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //不分频
		TIMBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIMBase_InitStructure.TIM_Period = 100-1; //ARR
		TIMBase_InitStructure.TIM_Prescaler = 720-1; //PSC  1000Hz频率周期为1ms，所以PWM的周期是1ms
		TIMBase_InitStructure.TIM_RepetitionCounter = 0; //重复计数器，用不到，直接给0即可
		TIM_TimeBaseInit(TIM2, &TIMBase_InitStructure);

		//第四步 配置输出比较单元
		TIM_OCInitTypeDef TIM2OC1_InitStructure;
		TIM_OCStructInit(&TIM2OC1_InitStructure); //初始化结构体所有成员，防止使用TIM1等其他高级定时器时，结构体因没有所有参数都初始化导致出现问题
		TIM2OC1_InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM2OC1_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //极性选择，高电平有效，即不翻转
		TIM2OC1_InitStructure.TIM_OutputState = TIM_OutputState_Enable; //输出使能
		TIM2OC1_InitStructure.TIM_Pulse = 0; //CCR
		TIM_OC1Init(TIM2, &TIM2OC1_InitStructure);

		TIM_OC2Init(TIM2, &TIM2OC1_InitStructure);
		TIM_OC3Init(TIM2, &TIM2OC1_InitStructure);
		TIM_OC4Init(TIM2, &TIM2OC1_InitStructure);
		
		
		// 运行控制
		TIM_Cmd(TIM2, ENABLE);
	}
	if(Mode == 2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		GPIO_InitTypeDef GPIOA_InitStructure;
		GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
		GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIOA_InitStructure);
		
		//第二步 选择时基单元时钟源
		TIM_InternalClockConfig(TIM3);
		
		
		//第三步 配置时基单元
		TIM_TimeBaseInitTypeDef TIMBase_InitStructure;
		TIMBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //不分频
		TIMBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIMBase_InitStructure.TIM_Period = 20000-1; //ARR
		TIMBase_InitStructure.TIM_Prescaler = 72-1; //PSC 50Hz频率周期为1ms，所以PWM的周期是1ms 1/20ms = (1/20*0.001) = 50Hz
		TIMBase_InitStructure.TIM_RepetitionCounter = 0; //重复计数器，用不到，直接给0即可
		TIM_TimeBaseInit(TIM3, &TIMBase_InitStructure);
		
		
		//第四步 配置输出比较单元
		TIM_OCInitTypeDef TIM3OC_InitStructure;
		TIM_OCStructInit(&TIM3OC_InitStructure); //初始化结构体所有成员，防止使用TIM1等其他高级定时器时，结构体因没有所有参数都初始化导致出现问题
		TIM3OC_InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM3OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //极性选择，高电平有效，即不翻转
		TIM3OC_InitStructure.TIM_OutputState = TIM_OutputState_Enable; //输出使能
		TIM3OC_InitStructure.TIM_Pulse = 500; //CCR
		TIM_OC1Init(TIM3, &TIM3OC_InitStructure); //通道1
		
		TIM_OC2Init(TIM3, &TIM3OC_InitStructure); //通道2
		
		

		
		
		// 运行控制
		TIM_Cmd(TIM3, ENABLE);
	}
	
	
	
	
	
	
}

void PWM_SetCompare1(uint16_t CompareNum)
{
	TIM_SetCompare1(TIM2, CompareNum); //这个函数是设置CCR的值
}

void PWM_SetCompare2(uint16_t CompareNum)
{
	TIM_SetCompare2(TIM2, CompareNum); //这个函数是设置CCR的值
}

void PWM_SetCompare3(uint16_t CompareNum)
{
	TIM_SetCompare3(TIM2, CompareNum); //这个函数是设置CCR的值
}

void PWM_SetCompare4(uint16_t CompareNum)
{
	TIM_SetCompare4(TIM2, CompareNum); //这个函数是设置CCR的值
}

void PWM_TIM3_SetCompare1(uint16_t CompareNum)
{
	TIM_SetCompare1(TIM3, CompareNum); //这个函数是设置CCR的值
}

void PWM_TIM3_SetCompare2(uint16_t CompareNum)
{
	TIM_SetCompare2(TIM3, CompareNum); //这个函数是设置CCR的值
}