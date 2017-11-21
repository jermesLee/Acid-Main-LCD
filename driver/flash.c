/******************************************************************************
*******************************************************************************
**                     板级支持包                                              
**                                                                            
**                     知行科技   Copyright 2017;                              
*******************************************************************************
** Filename      : flash.c
** Version       : V1.00    
** Programmer(s) : YZ       
** Date          : 2017.7.2 
** Discription   :中断向量配置，启动文件中的中断向量已改写，只需将板级中断函
                  数指针添加到本文件定义的中断向量数组BSP_IntVectTbl即可。
** Note(s)		 :         
*******************************************************************************
******************************************************************************/

#define  FLASH_MODULE
/******************************************************************************
*                      头文件                                                  
******************************************************************************/
//#include <includes.h>
#include "stm32f4xx.h"

#include "flash.h"
#include "dwin.h"
#include "main.h"
//#include "os.h"

//#include "bsp.h"        //板级函数声明
//#include "bsp_mcu.h"    //芯片相关的宏定义
//#include "bsp_cfg.h"    //板级软件、硬件配置

//#include "bsp_led.h"
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
static void Save_Parameter_to_Flash(void);
static void Read_Parameter_from_Flash(void);
/******************************************************************************
*                      内部函数定义                                                
******************************************************************************/
static void Read_Parameter_from_Flash(void)
{
	__IO FLASH_Status FLASHStatus;
	//指针p指向最后1kB flash的首地址
	//512kB flash地址为0x08000000-0x08080000
	//每2kB（0x800）一页，最后一页的地址为0x08080000-0x800=0x0807F800
	//(0x08080000-0x800);
    uint16_t * p=(uint16_t *)0x080E0000;
	uint16_t * Settings_p=(uint16_t *)(&Dwin_Settings );
	uint16_t i;
	if(*p==0xBAAA)
	{
		p++;
		for(i=0;i<APP_DWIN_RAM_NUM_OF_SETTINGS;i++)
		{
			*(Settings_p +i)=*(p+i);
		}
		
	}
    else
    {
        Set_Default_Settings();
        Save_Parameter_to_Flash();
    }

}
static void Save_Parameter_to_Flash(void)
{
	__IO FLASH_Status FLASHStatus;
	uint16_t  i;
	uint16_t * Settings_p=(uint16_t *)(&Dwin_Settings);
	
	
    
    FLASHStatus =FLASH_COMPLETE ;
	FLASH_Unlock ();
	FLASH_ClearFlag (FLASH_FLAG_OPERR |
                     FLASH_FLAG_EOP   |
                     FLASH_FLAG_PGAERR|
                     FLASH_FLAG_WRPERR|
                     FLASH_FLAG_PGPERR|
                     FLASH_FLAG_PGSERR );
    //FLASHStatus =FLASH_ErasePage (0x0807f800);
    FLASH_EraseSector(FLASH_Sector_8,VoltageRange_3); //擦除 
    //FLASH_ProgramByte(addr, *ptr);//写入 	
    
	for(i=0;i<APP_DWIN_RAM_NUM_OF_SETTINGS;i++)
	{
		FLASHStatus =FLASH_ProgramHalfWord (0x080E0000+2+2*i,*(Settings_p+i));
	}
	FLASHStatus =FLASH_ProgramHalfWord (0x080E0000,0xBAAA);	
	FLASH_Lock ();
}








/******************************************************************************
*                      外部函数定义                                          
******************************************************************************/
void Flash_Init(void)
{

     Read_Parameter_from_Flash();

}


void Flash_Process(void)
{
    //uint8_t Settings_updated=0;
    dwin_settings_t *p;
    
    if(SysTick_ms_for_Flash==0)
    {
        p=(dwin_settings_t *)0x080E0002;
//        if( p->Ambient_Temp_Plus  != Dwin_Settings .Ambient_Temp_Plus   ||
//            p->Ambient_Temp_Sub   != Dwin_Settings .Ambient_Temp_Sub    ||
//            p->Flue_Temp_Plus     != Dwin_Settings .Flue_Temp_Plus      ||
//            p->Flue_Temp_Sub      != Dwin_Settings .Flue_Temp_Sub       ||
//            p->Sensor_Current_Plus!= Dwin_Settings .Sensor_Current_Plus ||
//            p->Sensor_Current_Sub != Dwin_Settings .Sensor_Current_Sub  ||
//            p->Sensor_Temp_Plus   != Dwin_Settings .Sensor_Temp_Plus    ||
//            p->Sensor_Temp_Sub    != Dwin_Settings .Sensor_Temp_Sub     ||
//            p->Oxygen_Content     != Dwin_Settings .Oxygen_Content      ||
//            p->MMT_Offset         != Dwin_Settings .MMT_Offset          ||
//            p->Water_Content      != Dwin_Settings .Water_Content       ||
//            p->SO3_A   != Dwin_Settings .SO3_A   ||
//            p->SO3_B   != Dwin_Settings .SO3_B   ||
//            p->SO3_C   != Dwin_Settings .SO3_C   ||
//            p->SO3_D   != Dwin_Settings .SO3_D   ||
//            p->H2SO4_A != Dwin_Settings .H2SO4_A ||
//            p->H2SO4_B != Dwin_Settings .H2SO4_B ||
//            p->H2SO4_C != Dwin_Settings .H2SO4_C ||
//            p->H2SO4_D != Dwin_Settings .H2SO4_D )
        {
            Save_Parameter_to_Flash();
            SysTick_ms_for_Flash=2000;
        }
        
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

/******************************************************************************
*                      中断处理函数                                            
******************************************************************************/





//End of File
