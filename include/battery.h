/******************************************************************************
*******************************************************************************
**                     板级支持包（头文件）                                    
**                                                                             
**                     知行科技   Copyright 2017;                              
*******************************************************************************
** Filename      : battery.h  
** Version       : V1.00      
** Programmer(s) : YZ         
** Date          : 2017.7.2   
** Discription   : 。
** Note(s)		 :            
*******************************************************************************
******************************************************************************/
#ifndef BATTERY_H_     //防止多重包含
#define BATTERY_H_

/******************************************************************************
*                      头文件                                                  
******************************************************************************/
#include "stm32f4xx.h"


/******************************************************************************
*                      数据类型                                                
******************************************************************************/
typedef struct 
{
    volatile uint8_t    busy;    //空闲标志  0)空闲   1)忙碌
    uint8_t             r_w;              //读写标志  0)写     1)读
    uint8_t             id;               //从机设备ID号
    uint8_t             addr;             //要读写的地址
    volatile uint16_t   index;  //当前缓冲区数据长度
    uint16_t            bufLen;          //要发送或接收的数据长度   
    uint8_t * volatile  pBuff; //缓冲区首地址    
    void (* volatile FunCallBack)(void);//中断回调函数
}I2C_ParamTypeDef;

typedef enum
{
    TL2943_state_set_delay0,
    TL2943_state_delay0,
    TL2943_state_init,
    TL2943_state_delay1,
    TL2943_state_get_current,
    TL2943_state_delay2,
    TL2943_state_get_voltage,
    TL2943_state_delay3,
    TL2943_state_set_charge,
    TL2943_state_delay4,
    
    TL2943_state_get_charge,
    TL2943_state_delay5
}TL2943_state_t;

typedef struct
{
    uint16_t V;    //单位mV
    int16_t  A;    //单位mA
    uint16_t C;    //百分比

    
}battery_t;
/******************************************************************************
*                      软件配置宏                                              
******************************************************************************/


/******************************************************************************
*                      硬件配置宏                                              
******************************************************************************/
    /******************模块1*****************************/
#define TL2943_ADC_MODE_AUTO    0xC0    //连续
#define TL2943_ADC_MODE_SCAN    0x80    //间隔10s
#define TL2943_ADC_MODE_MANUAL  0x40    //手动，测量完成后休眠
#define TL2943_ADC_MODE_SLEEP   0x00    //休眠


#define TL2943_COULOMB_PRESCALER_1      0x00
#define TL2943_COULOMB_PRESCALER_4      0x08
#define TL2943_COULOMB_PRESCALER_16     0x10
#define TL2943_COULOMB_PRESCALER_64     0x18
#define TL2943_COULOMB_PRESCALER_256    0x20
#define TL2943_COULOMB_PRESCALER_1024   0x28
#define TL2943_COULOMB_PRESCALER_4096   0x30

#define TL2943_ALCC_MODE_ALERT              0x04
#define TL2943_ALCC_MODE_CHARGE_COMPLETE    0x02
#define TL2943_ALCC_MODE_DISABLE            0x00

#define TL2943_ANALOG_ON   0x00
#define TL2943_ANALOG_OFF  0x01

#define TL2943_REG_STATE                0x00
#define TL2943_REG_CONTROL              0x01
#define TL2943_REG_CHARGE               0x02
#define TL2943_REG_CHARGE_THRESHOLD_HI  0x04
#define TL2943_REG_CHARGE_THRESHOLD_LO  0x06

#define TL2943_REG_VOLTAGE              0x08
#define TL2943_REG_VOLTAGE_THRESHOLD_HI 0x0A
#define TL2943_REG_VOLTAGE_THRESHOLD_LO 0x0C

#define TL2943_REG_CURRENT              0x0E
#define TL2943_REG_CURRENT_THRESHOLD_HI 0x10
#define TL2943_REG_CURRENT_THRESHOLD_LO 0x12

#define TL2943_REG_TEMP                 0x14
#define TL2943_REG_TEMP_THRESHOLD_HI    0x16
#define TL2943_REG_TEMP_THRESHOLD_LO    0x17

#define TL2943_SENSOR_RESISTANCE        0.025  
#define TL2943_ADC_VOLTAGE_RANGE        23.6
#define TL2943_ADC_CURRENT_RANGE        (0.12/TL2943_SENSOR_RESISTANCE)
 
#define TL2943_ADC_PER_VOLT             (65535.0/23.6)

#define TL2943_ADC_PER_AMP              (65535.0/0.12*TL2943_SENSOR_RESISTANCE)
#define TL2943_ADC_PER_mAH

//注：18650单节容量9.0Wh，即2500mAh,三节并联7500mAh；
//    电流采样电阻25毫欧，充放电限制电流2A；
//    电荷积分系数4096，1LSB=0.68mAh,充满电量对应库伦计数11029，
//    考虑容量衰减，充满电时记为10000；

    /******************模块2*****************************/

    /******************模块3*****************************/

/******************************************************************************
*                      外部全局变量(模块内定义，模块外声明）                 
******************************************************************************/
#ifdef   BATTERY_MODULE
#define  BATTERY_EXT
#else
#define  BATTERY_EXT  extern
#endif

BATTERY_EXT I2C_ParamTypeDef I2C1_ParamStructure;
BATTERY_EXT uint8_t I2C1_Buff[10];
BATTERY_EXT battery_t Battery;

/******************************************************************************
*                      外部函数声明                                          
******************************************************************************/
void I2C1_Init(void);
void Battery_Process (void);

/******************************************************************************
*                      中断服务函数声明                                        
******************************************************************************/
void I2C1_EV_IRQHandler(void);

#endif    //End of File

