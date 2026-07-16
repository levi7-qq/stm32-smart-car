#include "stm32f10x.h"                  // Device header
#include "PWM.h"
/*
设置方向和速度的底层逻辑
配置stm32的4567引脚，4567引脚控制高低电平，
引脚还需要连线，到电机驱动的对应引脚，
pwm通过引脚2进行传输，pwm控制速度，也需要连接到电机
*/
void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	PWM_Init();
}
/*45引脚控制左边电机，45引脚有电位差，电机转动，
代码认为4高电平时候为正方向
*/
void Motor_SetLeftSpeed(int8_t Speed)
{
	if (Speed >0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		PWM_SetCompare3(Speed);//设置占空比调速
	}
	else if(Speed==0){
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
		PWM_SetCompare3(Speed);
	}else{
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
		PWM_SetCompare3(-Speed);
	}
}
void Motor_SetRightSpeed(int8_t Speed)
{
	if (Speed >0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_6);
		GPIO_ResetBits(GPIOA, GPIO_Pin_7);
		PWM_SetCompare3(Speed);
	}
	else if(Speed==0){
		GPIO_SetBits(GPIOA, GPIO_Pin_6);
		GPIO_SetBits(GPIOA, GPIO_Pin_7);
		PWM_SetCompare3(Speed);
	}else{
		GPIO_ResetBits(GPIOA, GPIO_Pin_6);
		GPIO_SetBits(GPIOA, GPIO_Pin_7);
		PWM_SetCompare3(-Speed);
	}
}
