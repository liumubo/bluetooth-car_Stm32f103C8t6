#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "OLED.h"
#include <string.h>
#include "Delay.h"

uint8_t k;


char Serial_RxPacket[200];
uint8_t Serial_RxFlag;

char * StrArray[2];

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //????????
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //????????
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //????????
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //???????
	USART_InitStructure.USART_Parity = USART_Parity_No; //????????
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //???1????
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //?????,????8?
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //RXNE标志位一旦置1就会向NVIC申请中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE); //??USART1
	
	OLED_Init();
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	//???????????
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //??TDR????????????????????,??????
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for(i=0;String[i] != '\0';i++)
	{
		Serial_SendByte(String[i]);//将字符串中的每一个字符依次发送过去
	}
}

uint32_t Serial_Pow(uint32_t x, uint32_t y)
{
	uint32_t Result = 1;
	while(y--)
	{
		Result *= x;
	}
	return Result;
}

void Serial_SendNum(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String,format, arg);
	va_end(arg);
	Serial_SendString(String);
}





void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
	
	if(USART_GetFlagStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		
		
		if(RxState == 0)
		{
			if(RxData == '@' && Serial_RxFlag == 0)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if(RxState == 1)
		{
			if(RxData == '\r')
			{
				RxState = 2;
			}
			else
			{
				Serial_RxPacket[pRxPacket++] = RxData;
			}
		}
		else if(RxState == 2)
		{
			if(RxData == '\n')
			{
				RxState = 0;
				Serial_RxPacket[pRxPacket] = '\0';
 				Serial_RxFlag = 1;
			}
		}
		
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE); //手动清除标志位
	}
}