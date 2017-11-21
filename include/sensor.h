/******************************************************************************
*******************************************************************************
**                     板级支持包（头文件）                                    
**                                                                             
**                     知行科技   Copyright 2017;                              
*******************************************************************************
** Filename      : sensor.h  
** Version       : V1.00      
** Programmer(s) : YZ         
** Date          : 2017.7.2   
** Discription   : 
** Note(s)		 :            
*******************************************************************************
******************************************************************************/
#ifndef SENSOR_H_     //防止多重包含
#define SENSOR_H_

/******************************************************************************
*                      头文件                                                  
******************************************************************************/
#include "stm32f4xx.h"

/******************************************************************************
*                      数据类型                                                
******************************************************************************/
typedef enum
{
    sensor_state_buffing,
    sensor_state_seek_head,
    //sensor_state_wait_for_data,
    sensor_state_decoding
    
}sensor_state_t;

typedef struct
{
    float Sensor_Current;
    float Sensor_Temp;
    float Flue_Temp;
    float Ambient_Temp;
}sensor_data_t;

/******************************************************************************
*                      软件配置宏                                              
******************************************************************************/


/******************************************************************************
*                      硬件配置宏                                              
******************************************************************************/
    /******************USART6*****************************/
#define USART6_BAUND_RATE               115200
#define USART6_GPIO_PORT                'C'
#define USART6_TBUFF_SIZE               50        
#define USART6_RBUFF_SIZE               50  

#if(USART6_GPIO_PORT == 'C')
    #define USART6_GPIO_RCC_ID      RCC_AHB1Periph_GPIOC   
    
    #define USART6_TX_GPIO_PORT     GPIOC
    #define USART6_TX_PIN           GPIO_Pin_6
    #define USART6_TX_PINSOURCE     GPIO_PinSource6
    
    #define USART6_RX_GPIO_PORT     GPIOC    
    #define USART6_RX_PIN           GPIO_Pin_7
    #define USART6_RX_PINSOURCE     GPIO_PinSource7

    #define USART6_485_RCC_ID       RCC_AHB1Periph_GPIOC
    #define USART6_485_GPIO_PORT    GPIOC
    #define USART6_485_PIN          GPIO_Pin_5
    #define USART6_485_PINSOURCE    GPIO_PinSource5

#elif(USART6_GPIO_PORT == 'G')
    #define USART6_GPIO_RCC_ID      RCC_AHB1Periph_GPIOG   
    #define USART6_TX_GPIO_PORT     GPIOG
    #define USART6_TX_PIN           GPIO_Pin_14
    #define USART6_TX_PINSOURCE     GPIO_PinSource14
    #define USART6_RX_GPIO_PORT     GPIOG    
    #define USART6_RX_PIN           GPIO_Pin_9
    #define USART6_RX_PINSOURCE     GPIO_PinSource9

#else
    #error "USART6 引脚配置错误"
#endif



    /******************模块2*****************************/

    /******************模块3*****************************/

/******************************************************************************
*                      外部全局变量(模块内定义，模块外声明）                 
******************************************************************************/
#ifdef   SENSOR_MODULE
#define  SENSOR_EXT
#else
#define  SENSOR_EXT  extern
#endif


/******************************************************************************
*                      外部函数声明                                          
******************************************************************************/
void Usart6_Init(void);
void Sensor_Process(void);


/******************************************************************************
*                      中断服务函数声明                                        
******************************************************************************/


#endif    //End of File

