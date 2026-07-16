#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
/*
串口收发驱动，初始化串口，pa9发送，pa10接受
开启了接收中断
*/
void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//复用推挽输出，tx引脚高低电平由外设自动控制，交给片上外设控制
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//rx是接收引脚，需要读外部电平，上拉输入保证没有信号时候稳定高电平
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//RX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*
	每秒9600bit，最常用的波特率，收发双方必须一致，否则乱码
	不需要rts/cts硬件流控引脚
	同时开启发送和接收功能
	无校验位，一般通信不用
	每帧数据后面跟一个停止位，用来标志一帧结束
	数据位长度8b
	*/
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	//开启接收中断rxne，接收到一个字节，数据寄存器满的时候触发中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	//优先级分为两组，2位抢占，2位子优先
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/*配置中断通道
	抢占优先级1:数字越小优先级越高
	子优先级1，抢占优先级相同时候比这个
	*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	//启动usart1
	USART_Cmd(USART1, ENABLE);
}

void Serial_SendByte(uint8_t Byte)
{
	//字节写进来发送到数据寄存器
	USART_SendData(USART1, Byte);
	/*
	txe标志位，等于0还有数据继续发，等于1没有数据可以发下一个
	等全部发完可以用TC
	*/
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	//一个一个字节发送数组
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		//将数字利用辅助幂运算，将数字一位一位发送
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}
//重写printf函数底层输出函数，让printf本来从屏幕打印转为从串口发出
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}
//自定义格式化打印
void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}



