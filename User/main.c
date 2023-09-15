#include "stm32f10x.h" // Device header
#include "LED.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Key.h"
#include "Serial.h"
#include "queue.h"
#include "usart.h"
#include "SysTick.h"
#include "OLED.h"
#include "Delay.h"
#include "Servo.h"
#include "Motor.h"
#include "Ultrasonic.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
// #include "handleReset.h"


// 任务优先级
#define START_TASK_PRIO 1
// 任务堆栈大小
#define START_STK_SIZE 128
// 任务句柄
TaskHandle_t StartTask_Handler;
// 任务函数
void start_task(void *pvParameters);

// 任务优先级
#define LED1_TASK_PRIO 2
// 任务堆栈大小
#define LED1_STK_SIZE 50
// 任务句柄
TaskHandle_t LED1Task_Handler;
// 任务函数
void led1_task(void *pvParameters);

// 任务优先级
#define RECEIVE_TASK_PRIO 3
// 任务堆栈大小
#define RECEIVE_STK_SIZE 512
// 任务句柄
TaskHandle_t ReceiveTask_Handler;
// 任务函数
void receive_task(void *pvParameters);

// 任务优先级
#define SEND_TASK_PRIO 4
// 任务堆栈大小
#define SEND_STK_SIZE 512
// 任务句柄
TaskHandle_t SendTask_Handler;
// 任务函数
void send_task(void *pvParameters);

QueueHandle_t Test_Queue = NULL; // 队列句柄

#define QUEUE_LEN 4	 /* 队列的长度，最大可包含多少个消息 */
#define QUEUE_SIZE 4 /* 队列中每个消息大小（字节） */

uint8_t i;

float Angle1;
float Angle2;

int8_t Speed;

uint8_t Key1Flag = 0;
uint8_t Key2Flag = 0;


char *strArray[2];

uint16_t Length;

uint8_t flagMid, flagLeft, flagRight;

static uint8_t distNum = 50;

uint8_t KeyNum;

uint16_t sum = 0;

uint8_t z = 0;

uint8_t cmdLeft, cmdAuto, cmdRight;

uint8_t isActive = 0;

uint8_t deg;

int main()
{
	// 硬件（外设）初始化
	//	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 设置系统中断优先级分组4
	Serial_Init();
	OLED_Init();
	LED_Init();
	Servo_Init();
	Motor_Init();
	Ultrasonic_Init();
	// handleReset_Init();
	OLED_ShowString(1,1, "Car");
	


	Servo1_SetAngle(110);
	

	// printf("FreeRTOS消息队列实验\r\n");
	// printf("按下KEY_UP或者KEY1发送队列消息\r\n");
	// printf("Receive任务接收到消息在串口回显\r\n");

	// 创建开始任务
	xTaskCreate((TaskFunction_t)start_task,			 // 任务函数
				(const char *)"start_task",			 // 任务名称
				(uint16_t)START_STK_SIZE,			 // 任务堆栈大小
				(void *)NULL,						 // 传递给任务函数的参数
				(UBaseType_t)START_TASK_PRIO,		 // 任务优先级
				(TaskHandle_t *)&StartTask_Handler); // 任务句柄

	vTaskStartScheduler(); // 开启任务调度
}

// 开始任务任务函数
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL(); // 进入临界区

	/* 创建Test_Queue */
	Test_Queue = xQueueCreate((UBaseType_t)QUEUE_LEN,	/* 消息队列的长度 */
							  (UBaseType_t)QUEUE_SIZE); /* 消息的大小 */

	// 创建LED1任务
	xTaskCreate((TaskFunction_t)led1_task,
				(const char *)"led1_task",
				(uint16_t)LED1_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)LED1_TASK_PRIO,
				(TaskHandle_t *)&LED1Task_Handler);

	// 创建接收任务
	xTaskCreate((TaskFunction_t)receive_task,
				(const char *)"receive_task",
				(uint16_t)RECEIVE_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)RECEIVE_TASK_PRIO,
				(TaskHandle_t *)&ReceiveTask_Handler);

	// 创建发送任务
	xTaskCreate((TaskFunction_t)send_task,
				(const char *)"send_task",
				(uint16_t)SEND_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)SEND_TASK_PRIO,
				(TaskHandle_t *)&SendTask_Handler);

	vTaskDelete(StartTask_Handler); // 删除开始任务
	taskEXIT_CRITICAL();			// 退出临界区
}

// LED1任务函数
void led1_task(void *pvParameters)
{
	while (1)
	{
		//        LED1=0;
		GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
		vTaskDelay(200);
		//        LED1=1;
		GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET);
		vTaskDelay(800);
	}
}

// 接收任务函数
void receive_task(void *pvParameters)
{
	BaseType_t xReturn = pdTRUE; /* 定义一个创建信息返回值，默认为pdTRUE */
	uint32_t r_queue;			 /* 定义一个接收消息的变量 */
	uint32_t sum1 = 0;
	uint32_t sum2 = 0;
	uint32_t send_data1 = 1;

	while (1)
	{
		xReturn = xQueueReceive(Test_Queue,		/* 消息队列的句柄 */
								&r_queue,		/* 保存消息的缓冲区 */
								portMAX_DELAY); /* 等待时间 一直等 */
		if (pdTRUE == xReturn)
		{
			OLED_ShowNum(1, 1, r_queue, 2);
			if (r_queue == 0)
			{
				NVIC_SystemReset();
			}
			if (r_queue == 1)
			{
				Serial_SendString("left!\r\n");
				OLED_ShowString(3, 5, "                ");
				OLED_ShowString(3, 5, "left!");

				Speed = (0 - 60);
				Motor_SetSpeed2(-Speed);
				Motor_SetSpeed3(Speed);

				Motor_SetSpeed(Speed);
				Motor_SetSpeed4(-Speed);
			}
			if (r_queue == 2)
			{
				Serial_SendString("right!\r\n");
				OLED_ShowString(3, 5, "                ");
				OLED_ShowString(3, 5, "right!");

				Speed = 60;
				Motor_SetSpeed2(-Speed);
				Motor_SetSpeed3(Speed);

				Motor_SetSpeed(Speed);
				Motor_SetSpeed4(-Speed);
			}
			if (r_queue == 3)
			{
				Serial_SendString("forward!\r\n");
				OLED_ShowString(3, 5, "                ");
				OLED_ShowString(3, 5, "forward!");

				Speed = 0;
				

				if (strcmp(strArray[1], "speed1") == 0)
				{
					Speed += 40;

				}
				else if (strcmp(strArray[1], "speed2") == 0)
				{
					Speed += 60;
				}
				else if (strcmp(strArray[1], "speed3") == 0)
				{
					Speed += 80;
				}
				else if (strcmp(strArray[1], "speed4") == 0)
				{
					Speed += 100;
				}

				Motor_SetSpeed(Speed);
				Motor_SetSpeed2(Speed);
				Motor_SetSpeed3(Speed);
				Motor_SetSpeed4(Speed);

				
				OLED_ShowString(1, 1, strArray[0]);
				OLED_ShowSignedNum(2, 7, Speed, 3);
			}
			if (r_queue == 4)
			{
				Serial_SendString("back!\r\n");
				OLED_ShowString(3, 5, "                ");
				OLED_ShowString(3, 5, "back!");


				Speed = 0;
				
				OLED_ShowString(3, 1, strArray[1]);

				if (strcmp(strArray[1], "speed1") == 0)
				{
					Speed = (0 - 40);

					OLED_ShowString(4, 1, "hello2");
				}
				else if (strcmp(strArray[1], "speed2") == 0)
				{
					Speed = (0 - 60);
				}
				else if (strcmp(strArray[1], "speed3") == 0)
				{
					Speed = (0 - 80);
				}
				else if (strcmp(strArray[1], "speed4") == 0)
				{
					Speed = (0 - 100);
				}

				Motor_SetSpeed(Speed);
				Motor_SetSpeed2(Speed);
				Motor_SetSpeed3(Speed);
				Motor_SetSpeed4(Speed);

			

				
				OLED_ShowSignedNum(2, 7, Speed, 3);
			}
			if (r_queue == 5)
			{
				Speed = 0;
				Motor_SetSpeed(Speed);
				Motor_SetSpeed2(Speed);
				Motor_SetSpeed3(Speed);
				Motor_SetSpeed4(Speed);
			}
			while (r_queue == 6)
			{
				OLED_ShowNum(3, 1, distNum, 2);
				
				if (Serial_RxFlag == 1)
				{

					NVIC_SystemReset();

					Serial_RxFlag = 0;
				}
				OLED_ShowString(2, 1, "Auto??");

				OLED_ShowNum(4, 5, distNum, 2);

				Servo1_SetAngle(110);
				
				sum = 0;
				for (i = 0; i < 5; i++)
				{
					Length = Ultrasonic_GetValue();
					sum += Length;
				}
				OLED_ShowNum(3, 10, sum, 4);

				// 前进
				Speed = 30;
				Motor_SetSpeed(Speed);
				Motor_SetSpeed2(Speed);
				Motor_SetSpeed3(Speed);
				Motor_SetSpeed4(Speed);

				// OLED_ShowNum(4, 1, distNum, 2);
				if (sum / 5 < distNum)
				{
					flagMid = 1;
				}

				if (flagMid == 1)
				{
					Speed = (0 - 25);
					Motor_SetSpeed(Speed);
					Motor_SetSpeed2(Speed);
					Motor_SetSpeed3(Speed);
					Motor_SetSpeed4(Speed);

					Delay_ms(50);

					Speed = (0);
					Motor_SetSpeed(Speed);
					Motor_SetSpeed2(Speed);
					Motor_SetSpeed3(Speed);
					Motor_SetSpeed4(Speed);

					sum = 0;
					for (deg = 45; deg <= 110; deg++)
					{
						Servo1_SetAngle(155 - deg);
						Delay_ms(10);

						if ((155 - deg) >= 45 && (155 - deg) <= 50)
						{
							Length = Ultrasonic_GetValue();
							sum += Length;
							Delay_ms(20);
						}
					}

					// for (i = 0; i < 5; i++)
					// {

					// }
					OLED_ShowNum(3, 10, sum, 4);

					if (sum / 5 < distNum)
					{
						flagRight = 1;
					}
				}
				OLED_ShowNum(2, 13, flagRight, 2);

				if (flagRight == 1 && flagMid == 1)
				{
					// 左转
					Speed = (0 - 50);
					Motor_SetSpeed2(-Speed);
					Motor_SetSpeed3(Speed);

					Motor_SetSpeed(Speed);
					Motor_SetSpeed4(-Speed);
					Delay_ms(30);
					OLED_ShowString(4, 1, "turnLeft");
					Servo1_SetAngle(110);
				}
				else if (flagMid == 1 && flagRight != 1)
				{
					sum = 0;
					for (deg = 45; deg <= 150; deg++)
					{
						Servo1_SetAngle(deg);
						Delay_ms(10);
						if (deg <= 150 && deg >= 145)
						{
							Length = Ultrasonic_GetValue();
							sum += Length;
							Delay_ms(20);
						}
					}

					// Servo1_SetAngle(150);

					// for (i = 0; i < 5; i++)
					// {

					// }
					OLED_ShowNum(3, 10, sum, 4);

					if (sum / 5 < distNum)
					{
						flagLeft = 1;
					}

					if (flagLeft == 1)
					{
						// 右转
						Speed = 50;
						Motor_SetSpeed2(-Speed);
						Motor_SetSpeed3(Speed);

						Motor_SetSpeed(Speed);
						Motor_SetSpeed4(-Speed);
						Delay_ms(30);
						
					}
					else
					{

						Servo1_SetAngle(110);
						sum = 0;
						for (i = 0; i < 5; i++)
						{
							Length = Ultrasonic_GetValue();
							sum += Length;
							Delay_ms(20);
						}
						OLED_ShowNum(3, 10, sum, 4);
						// 后退
						Speed = (0 - 25);
						Motor_SetSpeed(Speed);
						Motor_SetSpeed2(Speed);
						Motor_SetSpeed3(Speed);
						Motor_SetSpeed4(Speed);
						Delay_ms(20);

						if (sum / 5 < distNum)
						{
							flagMid = 1;
						}

						if (flagMid == 1)
						{
							// 前进
							Speed = 30;
							Motor_SetSpeed(Speed);
							Motor_SetSpeed2(Speed);
							Motor_SetSpeed3(Speed);
							Motor_SetSpeed4(Speed);
						
						}
						else
						{
							// 左转
							Speed = (0 - 50);
							Motor_SetSpeed2(-Speed);
							Motor_SetSpeed3(Speed);

							Motor_SetSpeed(Speed);
							Motor_SetSpeed4(-Speed);
							Delay_ms(30);
							
						}
					}
				}

				OLED_ShowNum(4, 11, flagLeft, 1);
				OLED_ShowNum(4, 13, flagMid, 1);
				OLED_ShowNum(4, 15, flagRight, 1);
				flagMid = 0;
				flagLeft = 0;
				flagRight = 0;
			}

			while (r_queue == 7)
			{
				

				OLED_ShowNum(3, 1, distNum, 2);
				if (Serial_RxFlag == 1)
				{
					NVIC_SystemReset();

					Serial_RxFlag = 0;
				}
				OLED_ShowString(2, 1, "Left??");
				OLED_ShowNum(4, 5, distNum, 2);

				Servo1_SetAngle(110);
				
				sum = 0;
				for (i = 0; i < 5; i++)
				{
					Length = Ultrasonic_GetValue();
					sum += Length;
					Delay_ms(20);
				}

				// 前进
				Speed = 30;
				Motor_SetSpeed(Speed);
				Motor_SetSpeed2(Speed);
				Motor_SetSpeed3(Speed);
				Motor_SetSpeed4(Speed);

				OLED_ShowNum(3, 10, sum, 4);
				if (sum / 5 < distNum)
				{
					flagMid = 1;
				}

				if (flagMid == 1)
				{
					Speed = (0 - 25);
					Motor_SetSpeed(Speed);
					Motor_SetSpeed2(Speed);
					Motor_SetSpeed3(Speed);
					Motor_SetSpeed4(Speed);

					Delay_ms(50);

					Speed = (0);
					Motor_SetSpeed(Speed);
					Motor_SetSpeed2(Speed);
					Motor_SetSpeed3(Speed);
					Motor_SetSpeed4(Speed);

					// OLED_ShowString(4, 1, "flagMid=1");

					sum = 0;
					for (deg = 45; deg <= 110; deg++)
					{
						Servo1_SetAngle(155 - deg);
						Delay_ms(10);
						if ((155 - deg) <= 50 && (155 - deg) >= 45)
						{
							Length = Ultrasonic_GetValue();
							sum += Length;
							Delay_ms(20);
						}
					}

					// Servo1_SetAngle(45);

					// for (i = 0; i < 5; i++)
					// {

					// }
					OLED_ShowNum(3, 10, sum, 4);

					if (sum / 5 < distNum)
					{
						flagRight = 1;
					}

					if (flagMid == 1 || flagRight == 1)
					{
						// 先后退
						Speed = (0 - 70);
						Motor_SetSpeed(Speed);
						Motor_SetSpeed2(Speed);
						Motor_SetSpeed3(Speed);
						Motor_SetSpeed4(Speed);

						Delay_ms(200);

						// 往左转
						Speed = (0 - 60);
						Motor_SetSpeed2(-Speed);
						Motor_SetSpeed3(Speed);

						Motor_SetSpeed(Speed);
						Motor_SetSpeed4(-Speed);
						Delay_ms(30);
						OLED_ShowString(4, 1, "turnLeft");
						Servo1_SetAngle(110);
					}

					OLED_ShowNum(4, 11, flagLeft, 1);
					OLED_ShowNum(4, 13, flagMid, 1);
					OLED_ShowNum(4, 15, flagRight, 1);
					flagMid = 0;
					flagRight = 0;
				}
			}

			while (r_queue == 8)
			{

				// OLED_ShowNum(3, 1, distNum, 2);
				if (Serial_RxFlag == 1)
				{
					NVIC_SystemReset();

					Serial_RxFlag = 0;
				}

				Servo1_SetAngle(110);
			
				sum = 0;
				for (i = 0; i < 5; i++)
				{
					Length = Ultrasonic_GetValue();
					sum += Length;
					Delay_ms(20);
				}

				// 前进
				Speed = 30;
				Motor_SetSpeed(Speed);
				Motor_SetSpeed2(Speed);
				Motor_SetSpeed3(Speed);
				Motor_SetSpeed4(Speed);

				OLED_ShowNum(3, 10, sum, 4);
				if (sum / 5 < distNum)
				{
					flagMid = 1;
				}

				if (flagMid == 1)
				{
					Speed = (0 - 25);
					Motor_SetSpeed(Speed);
					Motor_SetSpeed2(Speed);
					Motor_SetSpeed3(Speed);
					Motor_SetSpeed4(Speed);

					Delay_ms(50);

					Speed = (0);
					Motor_SetSpeed(Speed);
					Motor_SetSpeed2(Speed);
					Motor_SetSpeed3(Speed);
					Motor_SetSpeed4(Speed);

					
					sum = 0;
					for (deg = 110; deg <= 150; deg++)
					{
						Servo1_SetAngle(deg);
						Delay_ms(10);
						if (deg >= 145 && deg <= 150)
						{
							Length = Ultrasonic_GetValue();
							sum += Length;
							Delay_ms(10);
						}
					}


					OLED_ShowNum(3, 10, sum, 4);
					if (sum / 5 < distNum)
					{
						flagLeft = 1;
					}

					if (flagMid == 1 || flagLeft == 1)
					{
						// 先后退
						Speed = (0 - 70);
						Motor_SetSpeed(Speed);
						Motor_SetSpeed2(Speed);
						Motor_SetSpeed3(Speed);
						Motor_SetSpeed4(Speed);

						Delay_ms(200);

						// 往右转
						Speed = 60;
						Motor_SetSpeed2(-Speed);
						Motor_SetSpeed3(Speed);

						Motor_SetSpeed(Speed);
						Motor_SetSpeed4(-Speed);
						Delay_ms(30);
						OLED_ShowString(4, 1, "turnRight");
						Servo1_SetAngle(110);
					}

					OLED_ShowNum(4, 11, flagLeft, 1);
					OLED_ShowNum(4, 13, flagMid, 1);
					OLED_ShowNum(4, 15, flagRight, 1);
					flagMid = 0;
					flagRight = 0;
				}
			}

			
		}

		vTaskDelay(20);
	}
}

// 发送任务函数
void send_task(void *pvParameters)
{
	BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值，默认为pdPASS */
	uint32_t send_data1 = 1;
	uint32_t send_data2 = 2;
	uint32_t send_data3 = 3;
	uint32_t send_data4 = 4;
	uint32_t send_data5 = 5;
	uint32_t send_data6 = 6;
	uint32_t send_data7 = 7;
	uint32_t send_data8 = 8;
	uint32_t send_data0 = 0;
	u8 keyNum = 0;

	while (1)
	{

		i = 0;
		isActive = 0;

		if (Serial_RxFlag == 1)
		{

			char *str = strtok(Serial_RxPacket, " ");

			while (str != NULL)
			{
				// OLED_ShowNum(2, 5, i, 1);
				strArray[i++] = str;
				// OLED_ShowNum(2, 7, i, 1);

				str = strtok(NULL, " ");
			}
			if (strlen(strArray[1]) <= 2)
			{
				distNum = atoi(strArray[1]);
				// OLED_ShowNum(2, 5, distNum, 2);
				// OLED_ShowString(1, 10, strArray[1]);
			}

			// OLED_ShowString(1, 8, strArray[0]);

			if (strcmp(strArray[0], "reset") == 0)
			{

				xReturn = xQueueSend(Test_Queue,  /* 消息队列的句柄 */
									 &send_data0, /* 发送的消息内容 */
									 0);		  /* 等待时间 0 */
				if (pdPASS == xReturn)
				{
					// printf("消息send_data1发送成功!\r\n");
					// OLED_ShowString(4, 1, "send0 success");
				}
			}

			if (strcmp(strArray[0], "left") == 0)
			{

				xReturn = xQueueSend(Test_Queue,  /* 消息队列的句柄 */
									 &send_data1, /* 发送的消息内容 */
									 0);		  /* 等待时间 0 */
				if (pdPASS == xReturn)
				{
					// printf("消息send_data1发送成功!\r\n");
					// OLED_ShowString(4, 1, "send1 success");
				}
			}
			else if (strcmp(strArray[0], "right") == 0)
			{
				xReturn = xQueueSend(Test_Queue,  /* 消息队列的句柄 */
									 &send_data2, /* 发送的消息内容 */
									 0);		  /* 等待时间 0 */
				if (pdPASS == xReturn)
				{
					// printf("消息send_data1发送成功!\r\n");
					// OLED_ShowString(4, 1, "send2 success");
				}
			}
			else if (strcmp(strArray[0], "forward") == 0)
			{

				xReturn = xQueueSend(Test_Queue,  /* 消息队列的句柄 */
									 &send_data3, /* 发送的消息内容 */
									 0);		  /* 等待时间 0 */
				if (pdPASS == xReturn)
				{
					// printf("消息send_data1发送成功!\r\n");
					// OLED_ShowString(4, 1, "send3 success");
				}
			}
			else if (strcmp(strArray[0], "back") == 0)
			{
				xReturn = xQueueSend(Test_Queue,  /* 消息队列的句柄 */
									 &send_data4, /* 发送的消息内容 */
									 0);		  /* 等待时间 0 */
				if (pdPASS == xReturn)
				{
					// printf("消息send_data1发送成功!\r\n");
					// OLED_ShowString(4, 1, "send4 success");
				}
			}
			else if (strcmp(strArray[0], "stop") == 0)
			{
				xReturn = xQueueSend(Test_Queue,  /* 消息队列的句柄 */
									 &send_data5, /* 发送的消息内容 */
									 0);		  /* 等待时间 0 */
				if (pdPASS == xReturn)
				{
					// printf("消息send_data1发送成功!\r\n");
					// OLED_ShowString(4, 1, "send5 success");
				}
			}

			if (strcmp(strArray[0], "Obstacle") == 0)
			{

				xReturn = xQueueSend(Test_Queue,  /* 消息队列的句柄 */
									 &send_data6, /* 发送的消息内容 */
									 0);		  /* 等待时间 0 */
				if (pdPASS == xReturn)
				{
					// printf("消息send_data2发送成功!\r\n");
					// OLED_ShowString(4, 1, "send6 success");
				}
			}
			else if (strcmp(strArray[0], "Left") == 0)
			{
				xReturn = xQueueSend(Test_Queue,  /* 消息队列的句柄 */
									 &send_data7, /* 发送的消息内容 */
									 0);		  /* 等待时间 0 */
				if (pdPASS == xReturn)
				{
					// printf("消息send_data2发送成功!\r\n");
					// OLED_ShowString(4, 1, "send7 success");
				}
			}
			else if (strcmp(strArray[0], "Right") == 0)
			{
				xReturn = xQueueSend(Test_Queue,  /* 消息队列的句柄 */
									 &send_data8, /* 发送的消息内容 */
									 0);		  /* 等待时间 0 */
				if (pdPASS == xReturn)
				{
					// printf("消息send_data2发送成功!\r\n");
					// OLED_ShowString(4, 1, "send8 success");
				}
			}

			Serial_RxFlag = 0;
		}

		vTaskDelay(20);
	}
}