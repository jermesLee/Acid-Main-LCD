/******************************************************************************
*******************************************************************************
**                     板级支持包                                              
**                                                                            
**                     知行科技   Copyright 2017;                              
*******************************************************************************
** Filename      : sensor.c
** Version       : V1.00    
** Programmer(s) : YZ       
** Date          : 2017.7.2 
** Discription   :
** Note(s)		 :         
*******************************************************************************
******************************************************************************/

#define  SENSOR_MODULE
/******************************************************************************
*                      头文件                                                  
******************************************************************************/
#include "stm32f4xx.h"
#include "sensor.h"

#include "dwin.h"
#include "math.h"


/******************************************************************************
*                      配置宏                                                  
******************************************************************************/
    
    /******************模块1*****************************/

    /******************模块2*****************************/

    /******************模块3*****************************/


/******************************************************************************
*                      内部全局变量声明                                        
******************************************************************************/
static uint8_t Usart6_RBuff[USART6_RBUFF_SIZE];
static uint8_t Usart6_RBuff_RPointer;
static uint8_t Usart6_RBuff_WPointer;

/******************************************************************************
*                      内部函数声明                                            
******************************************************************************/
static void Init_Usart6_GPIO( void );
static void Init_Usart6_NVIC( void );
static void Init_Usart6( void );
static void Start_Usart6_Receive(void) ;

static void Write_Usart6_RBuff(uint8_t data);
static uint8_t Read_Usart6_RBuff(void);

/******************************************************************************
*                      内部函数定义                                                
******************************************************************************/
//初始化串口引脚
static void Init_Usart6_GPIO( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //时钟使能
    RCC_AHB1PeriphClockCmd (USART6_GPIO_RCC_ID ,ENABLE );
    RCC_AHB1PeriphClockCmd (USART6_485_RCC_ID ,ENABLE );
    
    //引脚配置      
    GPIO_InitStructure.GPIO_Pin     =USART6_TX_PIN ;		
    GPIO_InitStructure.GPIO_Mode    =GPIO_Mode_AF;   //输入、输出、模拟、复用
    GPIO_InitStructure.GPIO_Speed   =GPIO_Speed_2MHz; //2\25\50\100
    GPIO_InitStructure.GPIO_OType   =GPIO_OType_PP ; //推挽\开漏
    GPIO_InitStructure.GPIO_PuPd    =GPIO_PuPd_UP ;//上拉\下拉\浮空
    GPIO_Init(USART6_TX_GPIO_PORT ,&GPIO_InitStructure);        
    
    GPIO_InitStructure.GPIO_Pin     =USART6_RX_PIN ;
    GPIO_InitStructure.GPIO_Mode    =GPIO_Mode_AF;   //输入、输出、模拟、复用
    GPIO_InitStructure.GPIO_PuPd    =GPIO_PuPd_UP ;//上拉\下拉\浮空
    GPIO_Init(USART6_RX_GPIO_PORT ,&GPIO_InitStructure);    

    GPIO_InitStructure.GPIO_Pin     =USART6_485_PIN ;		
    GPIO_InitStructure.GPIO_Mode    =GPIO_Mode_OUT;   //输入、输出、模拟、复用
    GPIO_InitStructure.GPIO_Speed   =GPIO_Speed_2MHz; //2\25\50\100
    GPIO_InitStructure.GPIO_OType   =GPIO_OType_PP ; //推挽\开漏
    GPIO_InitStructure.GPIO_PuPd    =GPIO_PuPd_DOWN ;//上拉\下拉\浮空
    GPIO_Init(USART6_TX_GPIO_PORT ,&GPIO_InitStructure);        
    GPIO_ResetBits (USART6_485_GPIO_PORT,USART6_485_PIN );//接收状态
    
    //复用映射
    GPIO_PinAFConfig (USART6_TX_GPIO_PORT ,USART6_TX_PINSOURCE ,GPIO_AF_USART6 );
    GPIO_PinAFConfig (USART6_RX_GPIO_PORT ,USART6_RX_PINSOURCE ,GPIO_AF_USART6 );
}
//初始化串口中断优先级
static void Init_Usart6_NVIC( void )
{
    NVIC_InitTypeDef 	NVIC_InitStructure;	
    
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;		
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	        
}
//初始化串口
static void Init_Usart6( void )
{
    //定义初始化时用到的结构体变量
	USART_InitTypeDef 			USART_InitStructure;
    //时钟使能
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART6,ENABLE ); 

    //除波特率外，其他参数保持缺省：8位数据，1位停止，无校验，无硬件流控制
    USART_InitStructure.USART_BaudRate=USART6_BAUND_RATE;					//串口参数设置：9600,8,1,0
    USART_InitStructure.USART_WordLength=USART_WordLength_8b;
    USART_InitStructure.USART_StopBits=USART_StopBits_1;
    USART_InitStructure.USART_Parity =USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode=USART_Mode_Rx;	//使能接收模式
    
    USART_Init(USART6,&USART_InitStructure);

 	//串口中断使能
 	//USART_ITConfig(USART6,USART_IT_TC,ENABLE);			//发送完成中断    
    //USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);         //空闲中断
    //USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);       //接收完成中断，使能DMA后将被抑制
    //USART_ITConfig(USART6,USART_IT_TXE,ENABLE);        //发送缓冲空中断，使能DMA后将被抑制

    //USART_DMACmd(USART6, USART_DMAReq_Tx, ENABLE);  //USART发送DMA使能，使能之后将不再产生发送缓冲空中断               
    //USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);  //USART接收DMA使能，使能之后将不再产生接收完成中断    
    
    //使能串口
    USART_Cmd(USART6,ENABLE);    
}
//启动串口接收
static void Start_Usart6_Receive(void)
{
    Usart6_RBuff_WPointer =0;
    Usart6_RBuff_RPointer =0;
    //USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);         //空闲中断
    USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);       //接收完成中断，使能DMA后将被抑制    
}

//写串口接收缓冲
static void Write_Usart6_RBuff(uint8_t data)
{
    Usart6_RBuff[Usart6_RBuff_WPointer]=data;
    if(Usart6_RBuff_WPointer==USART6_RBUFF_SIZE -1)
        Usart6_RBuff_WPointer=0;
    else
        Usart6_RBuff_WPointer++;
}
//读串口接收缓冲
static uint8_t Read_Usart6_RBuff(void)
{
    uint8_t data;
    data=Usart6_RBuff[Usart6_RBuff_RPointer];
    if(Usart6_RBuff_RPointer==USART6_RBUFF_SIZE -1)
        Usart6_RBuff_RPointer=0;
    else
        Usart6_RBuff_RPointer++;
    return data;
}

/******************************************************************************
*                      外部函数定义                                          
******************************************************************************/
//USART6模块初始化
void Usart6_Init(void)
{
    Init_Usart6_GPIO();
	Init_Usart6_NVIC();
    //Init_Usart3_DMA(); 	    
    Init_Usart6();
    Start_Usart6_Receive();
}


//传感器通信总功能函数

float    gAverage_Current;
uint16_t gStable_Time;
void Sensor_Process(void)
{
    #define SENSOR_FRAME_LEN  ((uint8_t)19)
    #define SENSOR_FRAME_HEAD '$'
    #define CURRENT_THRESHOLD  50
    #define STABLE_THRESHOLD   5
    
    __align(4) static uint8_t frame[SENSOR_FRAME_LEN];  
    static uint8_t received_bytes=0;
    
    static float    Average_Current=0;
    //static float    Current=0;
    static uint16_t Stable_Time=0;
    static uint8_t ADT_is_Found=0;
    
    uint8_t i;
    uint16_t crc;
    sensor_data_t* pSensor;
    while(Usart6_RBuff_RPointer != Usart6_RBuff_WPointer) //缓冲有数据
    {
        frame[received_bytes ++]=Read_Usart6_RBuff();
        if(received_bytes==SENSOR_FRAME_LEN)  //满一帧数据
        {
            if(frame[0]==SENSOR_FRAME_HEAD)   //有帧头
            {
                crc=Get_CRC16 (frame,SENSOR_FRAME_LEN-2);
                if(frame[SENSOR_FRAME_LEN-2]==(crc&0x00FF) &&
                   frame[SENSOR_FRAME_LEN-1]==((crc>>8)&0x00FF) ) //CRC正确
                {
                    //去帧头处理
                    for(i=1;i<SENSOR_FRAME_LEN ;i++)
                    {
                        frame[i-1]=frame[i];
                    }
                    pSensor=(sensor_data_t*)frame;
                    
//                    if(Dwin_Measurements .Sensor_Current>CURRENT_THRESHOLD &&
//                        pSensor ->Sensor_Current< CURRENT_THRESHOLD)
//                    {
//                        Dwin_Measurements.ADT= pSensor ->Sensor_Temp*10;
//                    }
                    //更新dwin屏界面显示
                    Dwin_Measurements .Sensor_Current=pSensor ->Sensor_Current;
                    Dwin_Measurements .Sensor_Temp =pSensor ->Sensor_Temp ;
                    Dwin_Measurements .Ambient_Temp =pSensor ->Ambient_Temp ;
                    //Dwin_Measurements .Ambient_Temp/=10;
                    Dwin_Measurements .Flue_Temp =pSensor ->Flue_Temp ;
                    
                    Dwin_Measurements .Sensor_Current+=(Dwin_Settings .Sensor_Current_Plus/10) ;
                    Dwin_Measurements .Sensor_Current-=(Dwin_Settings .Sensor_Current_Sub/10) ;
                    Dwin_Measurements .Sensor_Temp+=(Dwin_Settings .Sensor_Temp_Plus/10) ;
                    Dwin_Measurements .Sensor_Temp-=(Dwin_Settings .Sensor_Temp_Sub/10) ;
                    Dwin_Measurements .Ambient_Temp+=(Dwin_Settings .Ambient_Temp_Plus) ;
                    Dwin_Measurements .Ambient_Temp-=(Dwin_Settings .Ambient_Temp_Sub );
                    Dwin_Measurements .Flue_Temp+=(Dwin_Settings .Flue_Temp_Plus/10) ;
                    Dwin_Measurements .Flue_Temp-=(Dwin_Settings .Flue_Temp_Sub /10);
                    
                    //限幅
                    if(Dwin_Measurements .Sensor_Current>32767)
                    {
                        Dwin_Measurements .Sensor_Current=0;
                    }
                    else if(Dwin_Measurements .Sensor_Current>1500)
                    {
                        Dwin_Measurements .Sensor_Current=1500;
                    }
                    //限幅
                    if( Dwin_Measurements .Sensor_Temp>32767)
                    {
                        Dwin_Measurements .Sensor_Temp=0;
                    }    
                    else if(Dwin_Measurements .Sensor_Temp>600)
                    {
                        Dwin_Measurements .Sensor_Temp=600;
                    }
                    //限幅
                    if( Dwin_Measurements .Flue_Temp>32767)
                    {
                        Dwin_Measurements .Flue_Temp=0;
                    }    
                    else if(Dwin_Measurements .Flue_Temp>600)
                    {
                        Dwin_Measurements .Flue_Temp=600;
                    }
                    //画曲线图
                    Dwin_AddPoint(Dwin_Measurements .Sensor_Current ,Dwin_Measurements .Sensor_Temp );
                    
                    //计算酸露点
                    if(ADT_is_Found ==0)
                    {
                        Average_Current *= 0.8;
                        Average_Current += Dwin_Measurements .Sensor_Current*0.2;
                        gAverage_Current=Average_Current;
                        if( Average_Current> CURRENT_THRESHOLD &&
                            Dwin_Measurements .Sensor_Current< Average_Current+ STABLE_THRESHOLD &&
                            Dwin_Measurements .Sensor_Current> Average_Current- STABLE_THRESHOLD )
                        {
                            Stable_Time++;
                            gStable_Time=Stable_Time;
                            if(Stable_Time >5)
                            {
                                ADT_is_Found =1;
                                Dwin_Measurements .ADT = Dwin_Measurements .Sensor_Temp*10;
                                //Dwin_Measurements .MMT = Dwin_Measurements .ADT +Dwin_Settings .MMT_Offset ;
                                Stable_Time=0;
                                Average_Current=0;
                            }
                        }
                        else
                        {
                            Stable_Time =0;
                        }                        
                    }
                    received_bytes=0;
                }
                else
                {
                    for(i=1;i<SENSOR_FRAME_LEN ;i++)
                    {
                        frame[i-1]=frame[i];
                    }
                    received_bytes --;                               
                }
            }
            else
            {
                for(i=1;i<SENSOR_FRAME_LEN ;i++)
                {
                    frame[i-1]=frame[i];
                }
                received_bytes --;
            }
        }
    }
    

    
    if(ADT_is_Found==1)
    {
        float t,x;
        float so3,h2so4;
        float a,b,c,d; //拟合系数        
        //计算三氧化硫浓度
        t=Dwin_Measurements .ADT; //单位0.1℃
        t/=10;    //单位1℃
        t-=100;   //平移到拟合曲线

        a=0.0003704;
        b=0.005309;
        c=0.006593;
        d=-0.006;
        
        so3=a*t*t*t+b*t*t+c*t-d;
        //so3手动修正
        so3+=(Dwin_Settings .SO3_Plus/10);
        so3-=(Dwin_Settings .SO3_Sub/10) ;
        
        Dwin_Measurements .SO3_Concentration =so3*10;
        
        
        //计算硫酸浓度
        t=Dwin_Measurements .ADT; //单位0.1℃
        t/=10;    //单位1℃
        x=Dwin_Settings .Water_Content ;
        x/=10;
        
//一次项系数        
//y1=0.00095784*x+0.036123;
//%常数项
//y2=0.18864*x+3.49; 
        a=0.00095784*x+0.036123;
        b=0.18864*x+3.49;
        
        //h2so4=(t-117)/22;
        h2so4=a*t-b;
        h2so4=pow(10,h2so4);
        //h2so4=0;
        h2so4 +=(Dwin_Settings .H2SO4_Plus/10) ;
        h2so4-=(Dwin_Settings .H2SO4_Sub /10);
        
        Dwin_Measurements.H2SO4_Concentration =h2so4*10;
        
        //add code here
        
        

        
        a=
        
        
        //计算MMT
        Dwin_Measurements.MMT =Dwin_Measurements .ADT+Dwin_Settings .MMT_Offset ;
        
        
    }
    
    
    
    
    if(Dwin_PostedCommands.Clear_Data ==1)
    {
        Dwin_Measurements .ADT =0;
        Dwin_Measurements .MMT =0;
        Dwin_Measurements .SO3_Concentration =0;
        Dwin_Measurements .H2SO4_Concentration =0;
        ADT_is_Found=0;
        Dwin_PostedCommands .Clear_Data =0;    
    }
    
    
    
}



/******************************************************************************
*                     中断处理函数
******************************************************************************/
void USART6_IRQHandler(void)
{
    uint8_t data;
    //在串口接收完成中断中
    if((USART6->SR)&USART_SR_RXNE)
    {
        data=USART6->DR ; 
        Write_Usart6_RBuff (data);
    } 
}



/******************************************************************************
*                       main( )
* Description : C代码入口函数，负责初始化uCOS，建立起始任务.
* Argument(s) : 无.
* Return(s)   : 无.
* Caller(s)   : .
* Designed by : YZ
* DATE        : 2017.7.9
* Version     : V2.0
* Note(s)     : .
******************************************************************************/



//End of File
