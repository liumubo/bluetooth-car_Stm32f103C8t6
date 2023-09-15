#include "stm32f10x.h"                  // Device header
#include "PWM.h"

#define SERVO_MODE			2

void Servo_Init(void)
{
	PWM_Init(SERVO_MODE);
}

//500 代表0度
//2500 代表180度

void Servo1_SetAngle(float Angle)
{
	PWM_TIM3_SetCompare1( Angle/180*2000 + 500);
}

void Servo2_SetAngle(float Angle)
{
	PWM_TIM3_SetCompare2( Angle/180*2000 + 500);
}

