/******************************************************************************
*******************************************************************************
**                     板级支持包（头文件）                                    
**                                                                             
**                     知行科技   Copyright 2017;                              
*******************************************************************************
** Filename      : dwin.h  
** Version       : V1.00      
** Programmer(s) : YZ         
** Date          : 2017.7.2   
** Discription   : 
** Note(s)		 :            
*******************************************************************************
******************************************************************************/
#ifndef _DWIN_H_     //防止多重包含
#define _DWIN_H_

/******************************************************************************
*                      头文件                                                  
******************************************************************************/
#include "stm32f4xx.h"

/******************************************************************************
*                      数据类型                                                
******************************************************************************/
//Dwin屏显示数据，用于电压、电流、功率等测试数据的存储（32个）
typedef struct 
{
    uint16_t Battery_Icon;       //0-6
    uint16_t Current_Icon;       //0-100
    uint16_t Sensor_TempIcon;    //100-300
    uint16_t Flue_Temp;          //0-600
    
    uint16_t ADT;                //酸露点，(单位0.1℃)[0,3000]
    uint16_t Sensor_Current;     //0-1500
    uint16_t Sensor_Temp;        //0-300
    uint16_t Ambient_Temp;       //LM35,0-70
    
    uint16_t SO3_Concentration;
    uint16_t H2SO4_Concentration;
    uint16_t Card_Space;         //MB
    uint16_t MMT;

    uint16_t U_Disk_Connected;
    uint16_t Version[2];
    uint16_t reserved[1];   
    
}dwin_measurements_t;


//Dwin屏指令数据，用于启停、保存、自检等指令的存储（16个）
typedef struct 
{
	uint16_t Reserved0;
    uint16_t Save_to_Disk;
	uint16_t Clear_Data;
	uint16_t Save_to_Card;
	uint16_t Password;
    uint16_t Restore_Default_Settings;
    uint16_t Save_Settings;
    uint16_t Delete_Data;
    //uint16_t reserved;
}dwin_commands_t;

//Dwin屏参数数据，用于用户设置参数的存储（32个）
typedef struct 
{
	uint16_t Oxygen_Content;
    uint16_t Product_Num;
    uint16_t Operator_Num;
    uint16_t MMT_Offset;
    
    uint16_t Water_Content;
    uint16_t Sensor_Temp_Plus;
    uint16_t Sensor_Temp_Sub;
    uint16_t Flue_Temp_Plus;
    
    uint16_t Flue_Temp_Sub;
    uint16_t Ambient_Temp_Plus;
    uint16_t Ambient_Temp_Sub;
    uint16_t Sensor_Current_Plus;
    
    uint16_t Sensor_Current_Sub;
    uint16_t SO3_Plus;
    uint16_t SO3_Sub;
    uint16_t H2SO4_Plus;
    uint16_t H2SO4_Sub;
//    uint16_t SO3_A;           //有必要改成有符号数
//    uint16_t SO3_B;
//    uint16_t SO3_C;
//    uint16_t SO3_D;
//    uint16_t H2SO4_A;
//    uint16_t H2SO4_B;
//    uint16_t H2SO4_C;
//    uint16_t H2SO4_D;
    uint16_t reserved[7];
   
}dwin_settings_t;


typedef struct
{
    uint8_t YY;   //BCD码,来自迪文屏
    uint8_t MM;   //BCD码,来自迪文屏
    uint8_t DD;   //BCD码,来自迪文屏
    uint8_t WW;   //BCD码,来自迪文屏
    uint8_t hh;   //BCD码,来自迪文屏
    uint8_t mm;   //BCD码,来自迪文屏
    uint8_t ss;   //BCD码,来自迪文屏
    uint8_t date[20];    //转码，字符串“yyyy_mm_dd_hh_mm_ss”
    
}dwin_rtc_t;

//迪文屏通信错误类型
typedef enum
{
    dwin_err_none=0,
    dwin_err_no_answer,
    dwin_err_header,
    dwin_err_crc,
    dwin_err_cmd,
    dwin_err_auto_answer,
    dwin_err_unknown,
    dwin_err_uncompeleted
}dwin_err_t;


//串口状态机
typedef enum      
{
    dwin_usart_state_idle,
    dwin_usart_state_communication,
    dwin_usart_state_completed,
    dwin_usart_state_decode,
    dwin_usart_state_timeout
    
}dwin_uart_state_t;

//迪文指令状态
typedef enum      
{
    dwin_instruction_state_idle,          //空闲
    dwin_instruction_state_ready,
    dwin_instruction_state_communication, //硬件通信
    dwin_instruction_state_decode,        //解码
    dwin_instruction_state_timeout,       //通信超时
    dwin_instruction_state_err,       //通信错误
    dwin_instruction_state_fail,
    dwin_instruction_state_completed      //完成
    
}dwin_instruction_state_t;

//迪文屏通信状态机
typedef enum
{
    dwin_app_state_idle,
    dwin_app_state_delay0,
    dwin_app_state_init_settings,
    dwin_app_state_get_settings,
    dwin_app_state_get_commands,
    dwin_app_state_reset_commands,
    dwin_app_state_handle_commands, //无需通信，且归为迪文屏模块的命令处理   
    dwin_app_state_switch_page,     //需要通信的命令处理
    dwin_app_state_get_time,
    dwin_app_state_cal_measurements,
    dwin_app_state_update_measurements,
    dwin_app_state_draw_trendline,
    dwin_app_state_delay
    

    
}dwin_app_state_t;



/******************************************************************************
*                      软件配置宏                                              
******************************************************************************/
#define APP_DWIN_FRAME_HEAD0        0X5A                //通信帧头，也可以作为多机地址
#define APP_DWIN_FRAME_HEAD1        0XA5                //通信帧头，也可以作为多机地址
#define APP_DWIN_CMD_WRITE_REG      0X80                //迪文屏寄存器写指令
#define APP_DWIN_CMD_READ_REG       0X81                //迪文屏寄存器读指令
#define APP_DWIN_CMD_WRITE_RAM      0X82                //迪文屏变量写指令
#define APP_DWIN_CMD_READ_RAM       0X83                //迪文屏变量读指令
#define APP_DWIN_CMD_WRITE_BUFF     0X84                //迪文屏曲线缓冲写指令

#define APP_DWIN_RAM_ADDRESS_OF_MEASUREMENTS    0X0010  //迪文屏数据分区地址（参考迪文屏设计）
#define APP_DWIN_RAM_ADDRESS_OF_COMMANDS        0X0000  //迪文屏数据分区地址（参考迪文屏设计）
#define APP_DWIN_RAM_ADDRESS_OF_SETTINGS        0X0020  //迪文屏数据分区地址（参考迪文屏设计）

#define APP_DWIN_RAM_ADDRESS_OF_TIPS            0X0100  //迪文屏提示字符串地址

#define APP_DWIN_RAM_NUM_OF_MEASUREMENTS    (sizeof(dwin_measurements_t)/2) //分区数据数量
#define APP_DWIN_RAM_NUM_OF_COMMANDS        (sizeof(dwin_commands_t)/2)     //分区数据数量
#define APP_DWIN_RAM_NUM_OF_SETTINGS        (sizeof(dwin_settings_t)/2)     //分区数据数量
#define APP_DWIN_RAM_NUM_OF_TIPS             0x20                           //暂定，未使用
#define APP_DWIN_TRENDLINE_BUFF_CHS          0x03                           //0、1通道


#define DWIN_Sys_Settings_Page        15      //系统设置页面
#define DWIN_Sys_Settings_Password    1111    //系统设置页面密码
#define DWIN_USB_Saving_Page          28      //U盘保存页面
#define DWIN_USB_Save_Compeleted_Page 29      //U盘保存完成页面
#define DWIN_USB_Save_Err_Page        30      //U盘保存失败页面


/******************************************************************************
*                      硬件配置宏                                              
******************************************************************************/
    /******************USART1*****************************/

    /******************USART2*****************************/

    /******************USART3*****************************/
#define USART3_BAUND_RATE               115200
#define USART3_GPIO_PORT                'D'

#if(USART3_GPIO_PORT == 'B')
    #define USART3_GPIO_RCC_ID      RCC_AHB1Periph_GPIOB   
    #define USART3_TX_GPIO_PORT     GPIOB
    #define USART3_TX_PIN           GPIO_Pin_10
    #define USART3_TX_PINSOURCE     GPIO_PinSource10
    #define USART3_RX_GPIO_PORT     GPIOB    
    #define USART3_RX_PIN           GPIO_Pin_11
    #define USART3_RX_PINSOURCE     GPIO_PinSource11
#elif(USART3_GPIO_PORT == 'C')
    #define USART3_GPIO_RCC_ID      RCC_AHB1Periph_GPIOC  
    #define USART3_TX_GPIO_PORT     GPIOC
    #define USART3_TX_PIN           GPIO_Pin_10
    #define USART3_TX_PINSOURCE     GPIO_PinSource10
    #define USART3_RX_GPIO_PORT     GPIOC    
    #define USART3_RX_PIN           GPIO_Pin_11
    #define USART3_RX_PINSOURCE     GPIO_PinSource11
#elif(USART3_GPIO_PORT == 'D')
    #define USART3_GPIO_RCC_ID      RCC_AHB1Periph_GPIOD  
    #define USART3_TX_GPIO_PORT     GPIOD
    #define USART3_TX_PIN           GPIO_Pin_8
    #define USART3_TX_PINSOURCE     GPIO_PinSource8
    #define USART3_RX_GPIO_PORT     GPIOD    
    #define USART3_RX_PIN           GPIO_Pin_9
    #define USART3_RX_PINSOURCE     GPIO_PinSource9        
#else
    #error "USART3 引脚配置错误"
#endif

    /******************USART4*****************************/

    /******************USART5*****************************/

    /******************USART6*****************************/

/******************************************************************************
*                      外部全局变量(模块内定义，模块外声明）                 
******************************************************************************/
#ifdef   DWIN_MODULE
#define  DWIN_EXT
#else
#define  DWIN_EXT  extern
#endif


DWIN_EXT dwin_measurements_t    Dwin_Measurements;      //
DWIN_EXT dwin_commands_t        Dwin_Commands;          //从Dwin屏接收的数据，发布后复位并回传
DWIN_EXT dwin_commands_t        Dwin_PostedCommands;  //对其他模块发布的命令，
DWIN_EXT dwin_settings_t        Dwin_Settings;     //初始化读，之后只写
DWIN_EXT dwin_settings_t        Dwin_Default_Settings;     //出产参数，上电时从SD卡读取
DWIN_EXT dwin_rtc_t             Dwin_RTC;
DWIN_EXT uint16_t               Dwin_Page_to_Switch;

//DWIN_EXT uint8_t                flag_dwin_commands_excuted; //命令执行完毕标志
//DWIN_EXT uint8_t                flag_dwin_commands_updated; //命令执行完毕标志



DWIN_EXT uint16_t   Dwin_TrendLine_Buff[100] ;
DWIN_EXT uint8_t    Dwin_TrendLine_Points_Num;
/******************************************************************************
*                      外部函数声明                                          
******************************************************************************/
void        Usart3_Init(void);
void        Dwin_Process(void); //提供给主函数的接口
uint16_t    Get_CRC16(__IO uint8_t *p_frame,uint8_t len_in_byte);
void        Dwin_AddPoint(uint16_t data1,uint16_t data2);
//void        Set_Default_Settings(void);

/******************************************************************************
*                      中断服务函数声明                                        
******************************************************************************/


#endif    //End of File


