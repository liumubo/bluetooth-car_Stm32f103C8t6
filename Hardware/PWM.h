#ifndef __PWM_H__
#define __PWM_H__

void PWM_Init(uint8_t Mode);

void PWM_SetCompare1(uint16_t CompareNum);

void PWM_SetCompare2(uint16_t CompareNum);

void PWM_SetCompare3(uint16_t CompareNum);

void PWM_SetCompare4(uint16_t CompareNum);


void PWM_TIM3_SetCompare1(uint16_t CompareNum);

void PWM_TIM3_SetCompare2(uint16_t CompareNum);

#endif