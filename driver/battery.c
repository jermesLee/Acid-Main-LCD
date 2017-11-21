/******************************************************************************
*******************************************************************************
**                     板级支持包                                              
**                                                                            
**                     知行科技   Copyright 2017;                              
*******************************************************************************
** Filename      : battery.c
** Version       : V1.00    
** Programmer(s) : YZ       
** Date          : 2017.7.2 
** Discription   : 电池监控模块。
** Note(s)		 :         
*******************************************************************************
******************************************************************************/

#define  BATTERY_MODULE
/******************************************************************************
*                      头文件                                                  
******************************************************************************/
#include "stdio.h"
#include "main.h"
#include "stm32f4xx.h"
#include "battery.h"

/******************************************************************************
*                      配置宏                                                  
******************************************************************************/
    
    /******************模块1*****************************/

    /******************模块2*****************************/

    /******************模块3*****************************/


/******************************************************************************
*                      内部全局变量声明                                        
******************************************************************************/


/******************************************************************************
*                      内部函数声明                                            
******************************************************************************/
static void I2C1_TxIRQ(void);
static void I2C1_RxIRQ(void);
static uint32_t I2C1_Write(uint8_t *buff, uint8_t WriteAddress, uint16_t WriteLen);
static uint32_t I2C1_Read (uint8_t *buff, uint8_t ReadAddress,  uint16_t ReadLen );


/******************************************************************************
*                      内部函数定义                                                
******************************************************************************/
static void I2C1_TxIRQ(void)
{
    switch (I2C_GetLastEvent(I2C1))
    {
    /* Test on I2Cx EV5 and clear it */
    case I2C_EVENT_MASTER_MODE_SELECT:
        I2C_Send7bitAddress(I2C1, I2C1_ParamStructure.id, I2C_Direction_Transmitter);
        break;
    
    /* Test on I2Cx EV6 and first EV8 and clear them */
    case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
        /* Send the first data */
        I2C_SendData(I2C1, I2C1_ParamStructure.addr);  /* EV8 just after EV6 */
        break;
    
    case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
        if((I2C1_ParamStructure.index < I2C1_ParamStructure.bufLen) 
            && (I2C1_ParamStructure.r_w == 0))
        {
            /* Transmit buffer data */
            I2C_SendData(I2C1, I2C1_ParamStructure.pBuff[I2C1_ParamStructure.index++]);
        }
        else
        {    
            I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
        }
        break;
    
    /* Test on I2Cx EV8 and clear it */
    case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
        if (I2C1_ParamStructure.r_w != 0)
        {
            I2C1_ParamStructure.FunCallBack = (void (*)(void))I2C1_RxIRQ;            
            I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
            I2C_GenerateSTART(I2C1, ENABLE);
        }
        else
        {
            I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);            
            I2C1_ParamStructure.busy = 0;            //接收结束标志
        }
        break;
    }
}

static void I2C1_RxIRQ(void)
{
    switch (I2C_GetLastEvent(I2C1))
    {
    /* Test on I2Cx EV5 and clear it */
    case I2C_EVENT_MASTER_MODE_SELECT:  
        /* Send I2Cx slave Address for write */
        I2C_Send7bitAddress(I2C1, I2C1_ParamStructure.id, I2C_Direction_Receiver);
        break;
    
    /* Test on I2Cx EV6 and first EV8 and clear them */
    case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED: 
        if (I2C1_ParamStructure.index == (I2C1_ParamStructure.bufLen - 1))
        {
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
        }
        break;
    
    /* Test on I2Cx EV2 and clear it */
    case I2C_EVENT_MASTER_BYTE_RECEIVED:
        I2C1_ParamStructure.pBuff[I2C1_ParamStructure.index++] = I2C_ReceiveData(I2C1);
        if (I2C1_ParamStructure.index == (I2C1_ParamStructure.bufLen - 1))
        {
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
        }
        else if (I2C1_ParamStructure.index >= I2C1_ParamStructure.bufLen)
        {
            I2C1_ParamStructure.FunCallBack = (void (*)(void))I2C1_TxIRQ;    //默认进接收中断       
            I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
            I2C1_ParamStructure.busy  = 0;            
        }
        break; 
    }
}

#define I2C_SLAVE_ADDRESS 0xC8
static uint32_t I2C1_Write(uint8_t *buff, uint8_t WriteAddress, uint16_t WriteLen)
{
    if (NULL == buff)
        return 0;   
    if (0 == WriteLen)
        return 0;
    if (0 != I2C1_ParamStructure.busy)
        return 0;
    
    I2C1_ParamStructure.busy       = 1;
    I2C1_ParamStructure.id         = I2C_SLAVE_ADDRESS;
    I2C1_ParamStructure.addr       = WriteAddress;
    I2C1_ParamStructure.index      = 0;
    I2C1_ParamStructure.r_w        = 0;
    I2C1_ParamStructure.bufLen     = WriteLen;
    I2C1_ParamStructure.pBuff      = buff;
    I2C1_ParamStructure.FunCallBack = (void (*)( void ))I2C1_TxIRQ;
    
    I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    /* Send I2C START condition */
    I2C_GenerateSTART(I2C1, ENABLE);
    return WriteLen;
}

static uint32_t I2C1_Read(uint8_t *buff, uint8_t ReadAddress, uint16_t ReadLen)
{
    if (NULL == buff)
        return 0;
    
    if (0 == ReadLen)
        return 0;
    
    if (0 != I2C1_ParamStructure.busy)
        return 0;
       
    I2C1_ParamStructure.busy       = 1;
    I2C1_ParamStructure.id         = I2C_SLAVE_ADDRESS;
    I2C1_ParamStructure.addr       = ReadAddress;
    I2C1_ParamStructure.index      = 0;
    I2C1_ParamStructure.r_w        = 1;
    I2C1_ParamStructure.bufLen     = ReadLen;
    I2C1_ParamStructure.pBuff      = buff;
    I2C1_ParamStructure.FunCallBack = (void (*)(void))I2C1_TxIRQ;
    
    I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    /* Send I2C START condition */
    I2C_GenerateSTART(I2C1, ENABLE);
    return ReadLen;
}








static void Init_LT2943(void)
{
    uint8_t ADC_Mode=TL2943_ADC_MODE_SCAN;
    uint8_t Coulomb_Perscaler=TL2943_COULOMB_PRESCALER_4096;
    uint8_t ALCC_Mode=TL2943_ALCC_MODE_ALERT;
    uint8_t Analog_State=TL2943_ANALOG_ON;
    
    //工作模式设定
    I2C1_Buff[0]=ADC_Mode|Coulomb_Perscaler|ALCC_Mode|Analog_State;
    I2C1_Write(&I2C1_Buff[0],TL2943_REG_CONTROL,0x01);
}
/******************************************************************************
*                      外部函数定义                                          
******************************************************************************/
void I2C1_Init(void)
{
    I2C_InitTypeDef  I2C_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1 , ENABLE);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    I2C_Cmd(I2C1, DISABLE);
    I2C_DeInit(I2C1);   
  
    GPIO_PinAFConfig(GPIOB , GPIO_PinSource8 , GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB , GPIO_PinSource9 , GPIO_AF_I2C1);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE); 

    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x30;  //主机地址
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_Init(I2C1, &I2C_InitStructure);

    //memset (&I2C_PARAM[I2C_x], 0, sizeof(I2C_PARAM_TYPE));
    
    NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);   
    
    I2C_Cmd(I2C1, ENABLE);
}




void  Battery_Process (void)
{
    static TL2943_state_t TL2943_State=TL2943_state_set_delay0 ;

    uint32_t V;
    int32_t  A;
    uint32_t C;
    uint16_t    tmp;
    
    switch(TL2943_State)
    {
        case TL2943_state_set_delay0:
            SysTick_ms_for_Battery=1000;
            TL2943_State= TL2943_state_delay0;
            break;
        case TL2943_state_delay0:
            if(SysTick_ms_for_Battery==0)
                TL2943_State=TL2943_state_init;
            break;
        case TL2943_state_init:
            Init_LT2943();
            SysTick_ms_for_Battery=100;
            TL2943_State= TL2943_state_delay1;
            break;
        case TL2943_state_delay1:
            if(SysTick_ms_for_Battery==0)
                TL2943_State=TL2943_state_get_current;
            break;
        case TL2943_state_get_current:
            I2C1_Read(&I2C1_Buff[0],TL2943_REG_CURRENT,0x02);
            SysTick_ms_for_Battery=100;
            TL2943_State= TL2943_state_delay2;        
            break;
        case TL2943_state_delay2:
            if(SysTick_ms_for_Battery==0)
            {
                //解析电流值
                tmp=I2C1_Buff[0];
                tmp<<=8;               //高字节
                tmp+=I2C1_Buff[1];     //低字节
                A=tmp;
                A-=32767;              //转成有符号数，范围[-32767,32768]
                A=(A*2400)/32768;    //量程±2400mA  (60mV/25mΩ)            
                Battery.A=A;
                TL2943_State=TL2943_state_get_voltage;
            }
            break;
        case TL2943_state_get_voltage:
            I2C1_Read(&I2C1_Buff[0],TL2943_REG_VOLTAGE,0x02);
            SysTick_ms_for_Battery=100;
            TL2943_State= TL2943_state_delay3;
            break;
        case TL2943_state_delay3:
            if(SysTick_ms_for_Battery==0)
            {
                //解析电压值
                tmp=I2C1_Buff[0];
                tmp<<=8;              //高字节
                tmp+=I2C1_Buff[1];    //低字节
                V=tmp;
                V=(V*23600)/65535;    //单位mV
                Battery .V =V; 
                
                if(Battery .V>12000 &&  Battery .A <100)   //充电完成 (充电电流符号为﹢)
                {
                     //设置电量满
                    TL2943_State=TL2943_state_set_charge;
                }
                else
                {
                    TL2943_State=TL2943_state_get_charge;
                }  
            }            
        
            break;
        case TL2943_state_set_charge:
            I2C1_Buff[0]=10000>>8;
            I2C1_Buff[1]=10000 & 0x00ff;
                    
            I2C1_Write(&I2C1_Buff[0],TL2943_REG_CHARGE,0x02);
            SysTick_ms_for_Battery=100;
            TL2943_State=TL2943_state_delay4;
        case TL2943_state_delay4: 
             if(SysTick_ms_for_Battery==0)
            {
                 TL2943_State=TL2943_state_get_charge;
            }
        
        case TL2943_state_get_charge:
            I2C1_Read(&I2C1_Buff[0],TL2943_REG_CHARGE,0x02);
            SysTick_ms_for_Battery=100;
            TL2943_State= TL2943_state_delay5;            
            break;
        case TL2943_state_delay5: 
            if(SysTick_ms_for_Battery==0)
            {
                //解析电荷值
                tmp=I2C1_Buff[0];
                tmp<<=8;               //高字节
                tmp+=I2C1_Buff[1];     //低字节
                C=tmp;
                Battery .C =C/100;
                TL2943_State=TL2943_state_get_current;
            }    
        
            break;
        default :
            TL2943_State=TL2943_state_set_delay0 ;
            break;
    }    
}
/******************************************************************************
*                      中断处理函数                                            
******************************************************************************/
void I2C1_EV_IRQHandler(void)
{
    if (I2C1_ParamStructure.FunCallBack)
    {
        I2C1_ParamStructure.FunCallBack();
    }
}




//End of File
