#include "stm32f10x.h"                  // Device header
//一秒7200 0000个时钟周期
//一毫秒72000
//一微秒72
//val寄存器写任意值，出发硬件把LOAD的值复制到VAL里，真正倒数的是val寄存器
void Delay_us(uint32_t xus)
{
    SysTick->LOAD=72*xus;//从多少开始数
	SysTick->VAL=0x00;//从零开始，当前值
	SysTick->CTRL=0x00000005;//启停，选始钟，看数完没
	
	/*
	与运算，二进制形式0000 0000 0000 0000 0000...//第十六位为标志位，硬件数完自动置为1
	*/
	while(!(SysTick->CTRL&0x00010000));
	
	SysTick->CTRL=0x00000004;//关闭时钟
}
//xms不等于零，调用微秒函数继续延时
void Delay_ms(uint32_t xms)
{
while(xms--)
{
Delay_us(1000);
}
}
void Delay_s(uint32_t xs)
{
while(xs--)
{
Delay_ms(1000);
}
}

