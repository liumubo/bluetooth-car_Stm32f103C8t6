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


// �������ȼ�
#define START_TASK_PRIO 1
// �����ջ��С
#define START_STK_SIZE 128
// ������
TaskHandle_t StartTask_Handler;
// ������
void start_task(void *pvParameters);

// �������ȼ�
#define LED1_TASK_PRIO 2
// �����ջ��С
#define LED1_STK_SIZE 50
// ������
TaskHandle_t LED1Task_Handler;
// ������
void led1_task(void *pvParameters);

// �������ȼ�
#define RECEIVE_TASK_PRIO 3
// �����ջ��С
#define RECEIVE_STK_SIZE 512
// ������
TaskHandle_t ReceiveTask_Handler;
// ������
void receive_task(void *pvParameters);

// �������ȼ�
#define SEND_TASK_PRIO 4
// �����ջ��С
#define SEND_STK_SIZE 512
// ������
TaskHandle_t SendTask_Handler;
// ������
void send_task(void *pvParameters);

QueueHandle_t Test_Queue = NULL; // ���о��

#define QUEUE_LEN 4	 /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
#define QUEUE_SIZE 4 /* ������ÿ����Ϣ��С���ֽڣ� */

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
	// Ӳ�������裩��ʼ��
	//	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // ����ϵͳ�ж����ȼ�����4
	Serial_Init();
	OLED_Init();
	LED_Init();
	Servo_Init();
	Motor_Init();
	Ultrasonic_Init();
	// handleReset_Init();
	OLED_ShowString(1,1, "Car");
	


	Servo1_SetAngle(110);
	

	// printf("FreeRTOS��Ϣ����ʵ��\r\n");
	// printf("����KEY_UP����KEY1���Ͷ�����Ϣ\r\n");
	// printf("Receive������յ���Ϣ�ڴ��ڻ���\r\n");

	// ������ʼ����
	xTaskCreate((TaskFunction_t)start_task,			 // ������
				(const char *)"start_task",			 // ��������
				(uint16_t)START_STK_SIZE,			 // �����ջ��С
				(void *)NULL,						 // ���ݸ��������Ĳ���
				(UBaseType_t)START_TASK_PRIO,		 // �������ȼ�
				(TaskHandle_t *)&StartTask_Handler); // ������

	vTaskStartScheduler(); // �����������
}

// ��ʼ����������
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL(); // �����ٽ���

	/* ����Test_Queue */
	Test_Queue = xQueueCreate((UBaseType_t)QUEUE_LEN,	/* ��Ϣ���еĳ��� */
							  (UBaseType_t)QUEUE_SIZE); /* ��Ϣ�Ĵ�С */

	// ����LED1����
	xTaskCreate((TaskFunction_t)led1_task,
				(const char *)"led1_task",
				(uint16_t)LED1_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)LED1_TASK_PRIO,
				(TaskHandle_t *)&LED1Task_Handler);

	// ������������
	xTaskCreate((TaskFunction_t)receive_task,
				(const char *)"receive_task",
				(uint16_t)RECEIVE_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)RECEIVE_TASK_PRIO,
				(TaskHandle_t *)&ReceiveTask_Handler);

	// ������������
	xTaskCreate((TaskFunction_t)send_task,
				(const char *)"send_task",
				(uint16_t)SEND_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)SEND_TASK_PRIO,
				(TaskHandle_t *)&SendTask_Handler);

	vTaskDelete(StartTask_Handler); // ɾ����ʼ����
	taskEXIT_CRITICAL();			// �˳��ٽ���
}

// LED1������
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

// ����������
void receive_task(void *pvParameters)
{
	BaseType_t xReturn = pdTRUE; /* ����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE */
	uint32_t r_queue;			 /* ����һ��������Ϣ�ı��� */
	uint32_t sum1 = 0;
	uint32_t sum2 = 0;
	uint32_t send_data1 = 1;

	while (1)
	{
		xReturn = xQueueReceive(Test_Queue,		/* ��Ϣ���еľ�� */
								&r_queue,		/* ������Ϣ�Ļ����� */
								portMAX_DELAY); /* �ȴ�ʱ�� һֱ�� */
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

				// ǰ��
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
					// ��ת
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
						// ��ת
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
						// ����
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
							// ǰ��
							Speed = 30;
							Motor_SetSpeed(Speed);
							Motor_SetSpeed2(Speed);
							Motor_SetSpeed3(Speed);
							Motor_SetSpeed4(Speed);
						
						}
						else
						{
							// ��ת
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

				// ǰ��
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
						// �Ⱥ���
						Speed = (0 - 70);
						Motor_SetSpeed(Speed);
						Motor_SetSpeed2(Speed);
						Motor_SetSpeed3(Speed);
						Motor_SetSpeed4(Speed);

						Delay_ms(200);

						// ����ת
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

				// ǰ��
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
						// �Ⱥ���
						Speed = (0 - 70);
						Motor_SetSpeed(Speed);
						Motor_SetSpeed2(Speed);
						Motor_SetSpeed3(Speed);
						Motor_SetSpeed4(Speed);

						Delay_ms(200);

						// ����ת
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

// ����������
void send_task(void *pvParameters)
{
	BaseType_t xReturn = pdPASS; /* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
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

				xReturn = xQueueSend(Test_Queue,  /* ��Ϣ���еľ�� */
									 &send_data0, /* ���͵���Ϣ���� */
									 0);		  /* �ȴ�ʱ�� 0 */
				if (pdPASS == xReturn)
				{
					// printf("��Ϣsend_data1���ͳɹ�!\r\n");
					// OLED_ShowString(4, 1, "send0 success");
				}
			}

			if (strcmp(strArray[0], "left") == 0)
			{

				xReturn = xQueueSend(Test_Queue,  /* ��Ϣ���еľ�� */
									 &send_data1, /* ���͵���Ϣ���� */
									 0);		  /* �ȴ�ʱ�� 0 */
				if (pdPASS == xReturn)
				{
					// printf("��Ϣsend_data1���ͳɹ�!\r\n");
					// OLED_ShowString(4, 1, "send1 success");
				}
			}
			else if (strcmp(strArray[0], "right") == 0)
			{
				xReturn = xQueueSend(Test_Queue,  /* ��Ϣ���еľ�� */
									 &send_data2, /* ���͵���Ϣ���� */
									 0);		  /* �ȴ�ʱ�� 0 */
				if (pdPASS == xReturn)
				{
					// printf("��Ϣsend_data1���ͳɹ�!\r\n");
					// OLED_ShowString(4, 1, "send2 success");
				}
			}
			else if (strcmp(strArray[0], "forward") == 0)
			{

				xReturn = xQueueSend(Test_Queue,  /* ��Ϣ���еľ�� */
									 &send_data3, /* ���͵���Ϣ���� */
									 0);		  /* �ȴ�ʱ�� 0 */
				if (pdPASS == xReturn)
				{
					// printf("��Ϣsend_data1���ͳɹ�!\r\n");
					// OLED_ShowString(4, 1, "send3 success");
				}
			}
			else if (strcmp(strArray[0], "back") == 0)
			{
				xReturn = xQueueSend(Test_Queue,  /* ��Ϣ���еľ�� */
									 &send_data4, /* ���͵���Ϣ���� */
									 0);		  /* �ȴ�ʱ�� 0 */
				if (pdPASS == xReturn)
				{
					// printf("��Ϣsend_data1���ͳɹ�!\r\n");
					// OLED_ShowString(4, 1, "send4 success");
				}
			}
			else if (strcmp(strArray[0], "stop") == 0)
			{
				xReturn = xQueueSend(Test_Queue,  /* ��Ϣ���еľ�� */
									 &send_data5, /* ���͵���Ϣ���� */
									 0);		  /* �ȴ�ʱ�� 0 */
				if (pdPASS == xReturn)
				{
					// printf("��Ϣsend_data1���ͳɹ�!\r\n");
					// OLED_ShowString(4, 1, "send5 success");
				}
			}

			if (strcmp(strArray[0], "Obstacle") == 0)
			{

				xReturn = xQueueSend(Test_Queue,  /* ��Ϣ���еľ�� */
									 &send_data6, /* ���͵���Ϣ���� */
									 0);		  /* �ȴ�ʱ�� 0 */
				if (pdPASS == xReturn)
				{
					// printf("��Ϣsend_data2���ͳɹ�!\r\n");
					// OLED_ShowString(4, 1, "send6 success");
				}
			}
			else if (strcmp(strArray[0], "Left") == 0)
			{
				xReturn = xQueueSend(Test_Queue,  /* ��Ϣ���еľ�� */
									 &send_data7, /* ���͵���Ϣ���� */
									 0);		  /* �ȴ�ʱ�� 0 */
				if (pdPASS == xReturn)
				{
					// printf("��Ϣsend_data2���ͳɹ�!\r\n");
					// OLED_ShowString(4, 1, "send7 success");
				}
			}
			else if (strcmp(strArray[0], "Right") == 0)
			{
				xReturn = xQueueSend(Test_Queue,  /* ��Ϣ���еľ�� */
									 &send_data8, /* ���͵���Ϣ���� */
									 0);		  /* �ȴ�ʱ�� 0 */
				if (pdPASS == xReturn)
				{
					// printf("��Ϣsend_data2���ͳɹ�!\r\n");
					// OLED_ShowString(4, 1, "send8 success");
				}
			}

			Serial_RxFlag = 0;
		}

		vTaskDelay(20);
	}
}