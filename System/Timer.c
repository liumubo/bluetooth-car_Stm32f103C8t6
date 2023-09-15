#include "stm32f10x.h" // Device header
#include "Timer.h"


extern uint16_t Time;

void Timer_Init()
{
	Time = 0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); // 选择APB1总线下的定时器Timer4

	TIM_InternalClockConfig(TIM4); // TIM4使用内部时钟

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 计数模式，此处为向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 7200 - 1;				// ARR 1 = 0.0001S
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;					// PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			// 高级计时器特有，重复计数
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); // 使能中断

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; // 中断通道选择
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 优先级，同上

	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM4, ENABLE); // 打开定时器
}

void TIM4_IRQHandler() // 定时器4的中断函数
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{

		if (GPIO_ReadInputDataBit(Echo_Port, Echo_Pin) == 1) // 高电平的持续时间就是声波的传播时间
		{
			// 高电平一直持续Time就一直自增，一个Time代表100us
			Time++;
		}
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // 清空标志位
	}
}
