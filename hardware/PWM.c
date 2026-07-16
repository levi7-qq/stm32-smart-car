#include "stm32f10x.h"                  // Device header
void PWM_Init(void)
{
	//开时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//初始化gpio，根据pwm和引脚关系配置
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//引脚复用规定pa2对应tim2的通道3
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//配置tim2使用内部时钟
	TIM_InternalClockConfig(TIM2);
	/*时基单元初始化，psc，arr
	pwm频率=72mhz/(psc+1)*(arr+1)
	pwm周期=1/pwm频率
	*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分频选择不分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//计数模式选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;		//ARR自动重装在值，规定一个周期的计数值等于arr+1
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;		//PSC，对输入的72mhz时钟再分频，得到计数器计数频率，72mhz/psc+1
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;//重复计数器只有tim1和8有，通用的写0
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	//输出比较通道初始化，配置极性和比较
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);//先给结构体赋默认值，避免未初始化的成员异常
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//输出比较模式。pwm1计数器值小于ccr输出有效电平，大于相反
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//配置输出极性有效电平是高电平
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//打开输出通道，pwm信号送到引脚
	TIM_OCInitStructure.TIM_Pulse = 0;		//CCR，捕获比较值，初始值写0，占空比为
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	
	//启动定时器
	TIM_Cmd(TIM2, ENABLE);
}
//封装改ccr值的函数，调用函数实现调节占空比
void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM2, Compare);
}