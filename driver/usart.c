/*
 * FILE                : usart.c
 * DESCRIPTION         : This file is iCore3 usart driver demo.
 * Author              : XiaomaGee@Gmail.com
 * Copyright           :
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 12/05/2015
 *
 * create.
 * --------------------
 */
//------------------------ Include files ------------------------//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
 
#include <ctype.h>
#include <math.h>
#include <stdarg.h>

#include "..\FWlib\inc\stm32f4xx_rcc.h"
#include "..\FWlib\inc\stm32f4xx_gpio.h"
#include "..\FWlib\inc\stm32f4xx_usart.h"
#include "..\include\usart.h"

//--------------------------- Variable --------------------------// 
//usart4 debug
static int send_string_to_usart1(char * str);
static int send_byte_to_usart1(char data);
static int initialize_usart1(unsigned long int baudrate);
static int my_printf1(const char * fmt,...);
static int send_buffer1(void * ,int);

UART_T usart1 = {
	.receive_ok_flag=0,
	.counter=0,
	.send_byte = send_byte_to_usart1,
	.send_string = send_string_to_usart1,
	.initialize=initialize_usart1,
	.printf=my_printf1,
	.send_buffer = send_buffer1
	};
//--------------------------- Function --------------------------//
//USART4 DEBUG
/*
 * Name               : initialize_usart4
 * Description        : ---
 * Author             : XiaomaGee
 * 
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 12/05/2015
 *
 * create.
 * -------------------
 */
static int initialize_usart1(unsigned long int baudrate)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);             // GPIO时钟使能
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                        //PA9为复用推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                        //PA10为浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);           //PA9引脚复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);           //PA10引脚复用为USART1
	
	USART_DeInit(USART1);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
	  
	USART_InitStructure.USART_BaudRate = baudrate;                   //波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;      //8个数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;           //1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No ;             //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //收发模式
	USART_Init(USART1, &USART_InitStructure);                         //初始化USART1
	USART_Cmd(USART1, ENABLE);                                        //使能USART1
	USART_ITConfig(USART1,USART_IT_PE,ENABLE);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);                      //打开USART1的中断
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	return 0;
}

/*
 * Name               : send_buffer1
 * Description        : ---
 * Author             : XiaomaGee
 * 
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 12/05/2015
 *
 * create.
 * -------------------
 */
static int
send_buffer1(void * buf,int len)                                  
{
	char *p = (char *)buf;
	
	if(len <= 0)return -1;
	
	while(len --){
		send_byte_to_usart1(*p);
		p ++;
	}
	
	return 0;
}

/*
 * Name               : send_byte_to_usart4
 * Description        : ---
 * Author             : XiaomaGee
 * 
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 12/05/2015
 *
 * create.
 * -------------------
 */
static int 
send_byte_to_usart1(char data)          	                          //TTL通信
{
	while(!(USART_GetFlagStatus(USART1,USART_FLAG_TC) == 1));          //等待发送数据完成
	USART_SendData(USART1,data);                                       //将数据写入数据寄存器中
	
	return 0;
}

/*
 * Name               : send_string_to_usart4
 * Description        : ---
 * Author             : XiaomaGee
 * 
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 12/05/2015
 *
 * create.
 * -------------------
 */
static int 
send_string_to_usart1(char * str)
{
	while(*str!='\0'){
		while(!(USART_GetFlagStatus(USART1,USART_FLAG_TC) == 1));
		USART_SendData(USART1,*str++);	
	}
	return 0;
}

/*
 * Name               : UART4_IRQHandler
 * Description        : ---
 * Author             : XiaomaGee
 * 
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 12/05/2015
 *
 * create.
 * -------------------
 */
int 
USART1_IRQHandler(void)
{	
  while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == 0);                                                                 
 	usart1.receive_buffer[usart1.counter++]=USART_ReceiveData(USART1); //接收数据
 	
 	if(usart1.receive_buffer[usart1.counter - 1] == '\n' && usart1.receive_buffer[usart1.counter - 2] == '\r'){   			
			usart1.receive_buffer[usart1.counter-1]=0;
 			usart1.counter=0;                                
 	   	usart1.receive_ok_flag=1;                      
 		}
	return 0;			
}

/*
 * Name               : printf
 * Description        : ---
 * Author             : XiaomaGee
 * 
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 12/05/2015
 *
 * create.
 * -------------------
 */
static int
my_printf1(const char * fmt,...)             		                    //串口4输出
{
	__va_list arg_ptr; 
	char buf[UART_BUFFER_SIZE];
  		
	memset(buf,'\0',sizeof(buf));

	va_start(arg_ptr,fmt);
	vsprintf(buf,fmt,arg_ptr);
	va_end(arg_ptr);

	send_string_to_usart1(buf);

	return 0;
}
