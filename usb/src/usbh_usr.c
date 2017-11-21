/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file includes the usb host library user callbacks
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usbh_usr.h"
#include "ff.h"
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "..\include\led.h"
#include "..\include\usart.h"

#include "dwin.h"
#include "sd_card.h"


extern USB_OTG_CORE_HANDLE          USB_OTG_Core;


unsigned char USBH_USR_ApplicationState = USH_USR_HS_DONE;
FATFS fatfs;
FIL file; 

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
    
};


void USBH_USR_Init(void)
{    
    //the below two lines are comment by yz at 20171025
    //usart1.printf(" USB OTG HS MSC Host\r\n");
    //usart1.printf(" USB Host Library v2.1.0\r\n");
}


void USBH_USR_DeviceAttached(void)
{
	//usart1.printf(" Device Attached\r\n");
}



void USBH_USR_UnrecoveredError (void)
{
  //usart1.printf(" UNRECOVERED ERROR STATE\r\n");
}



void USBH_USR_DeviceDisconnected (void)
{
	//usart1.printf(" Device Disconnected\r\n");
}

void USBH_USR_ResetDevice(void)
{
}

void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
}


void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
  //USBH_DevDesc_TypeDef *hs;
  //hs = DeviceDesc;  

  //usart1.printf(" VID : %04Xh\r\n" , (uint32_t)(*hs).idVendor); 
  //usart1.printf(" PID : %04Xh\r\n" , (uint32_t)(*hs).idProduct); 
}


void USBH_USR_DeviceAddressAssigned(void)
{
  
}



void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
//  USBH_InterfaceDesc_TypeDef *id;
//  
//  id = itfDesc;  
//  
//  if((*id).bInterfaceClass  == 0x08)
//  {
//    usart1.printf(" Mass storage device connected\r\n");
//  }
//  else if((*id).bInterfaceClass  == 0x03)
//  {
//    usart1.printf(" HID device connected\r\n");
//  }    
}


void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
  //usart1.printf(" Manufacturer : %s\r\n", (char *)ManufacturerString);
}


void USBH_USR_Product_String(void *ProductString)
{
  //usart1.printf(" Product : %s\r\n", (char *)ProductString);  
}


void USBH_USR_SerialNum_String(void *SerialNumString)
{
  //usart1.printf(" Serial Number : %s\r\n", (char *)SerialNumString);    
} 




void USBH_USR_EnumerationDone(void)
{
} 



void USBH_USR_DeviceNotSupported(void)
{
  //usart1.printf(" Device not supported\r\n"); 
}  

USBH_USR_Status USBH_USR_UserInput(void)
{
  USBH_USR_Status usbh_usr_status;
   
  usbh_usr_status = USBH_USR_RESP_OK;

  return usbh_usr_status;
}  


void USBH_USR_OverCurrentDetected (void)
{
  //usart1.printf(" Overcurrent detected\r\n");
}

//static void Num16_to_ASIC(uint16_t Num,unsigned char *buff,uint16_t Field,uint16_t Decimal)
//{
//    uint32_t i;
//    buff+=(Field-1);
//    for(i=0;i<Decimal ;i++) //小数部分
//    {
//        *(buff--)=Num%10 +0x30;
//        Num/=10;
//    }
//    if(Decimal !=0)
//    {
//        *(buff--)='.';
//    }
//    *(buff--)=Num%10 +0x30;
//    Num/=10;
//    while(Num!=0)
//    {
//        *(buff--)=Num%10 +0x30;
//        Num/=10;    
//    }
//}

//static void Fill_USB_WBuff(unsigned char * buff)
//{
//    unsigned char *p=(unsigned char*)"产品编号 操作员编号 日期       时间      数据";
//    //uint16_t i;
//    
//    while((*buff++ = *p++) !='\0')
//    {
//        ;
//    }
//    (*buff++) ='\r';
//    (*buff++) ='\n';
//    
////    Num16_to_ASIC(Dwin_Settings .Product_Num ,buff,8,0);
////    buff+=(8+1);
////    
////    Num16_to_ASIC(Dwin_Settings .Operator_Num ,buff,10,0);
////    buff+=(10+1);
////    
////    for(i=0;i<13;i++)
////    {
////        *(buff++)=Dwin_RTC .date [i];
////    }
////    *(buff++)=':';
////    *(buff++)=Dwin_RTC .date [14];
////    *(buff++)=Dwin_RTC .date [15];
////    *(buff++)=':';
////    *(buff++)=Dwin_RTC .date [17];
////    *(buff++)=Dwin_RTC .date [18];

////    buff++;
////    buff++;
////    
////    *(buff++)='A';
////    *(buff++)='D';
////    *(buff++)='T';
////    *(buff++)=':';
////    Num16_to_ASIC(Dwin_Measurements .ADT ,buff,5,1);
////    buff+=6;
////    *(buff++)='o';
////    *(buff++)='C';
////    buff+=2;
////    
////    
////    *(buff++)='M';
////    *(buff++)='M';
////    *(buff++)='T';
////    *(buff++)=':';
////    Num16_to_ASIC(Dwin_Measurements .MMT ,buff,5,1);
////    buff+=6;
////    *(buff++)='o';
////    *(buff++)='C';
////    buff+=2;
////    
////    *(buff++)='S';
////    *(buff++)='O';
////    *(buff++)='3';
////    *(buff++)=':';
////    Num16_to_ASIC(Dwin_Measurements .SO3_Concentration  ,buff,5,1);
////    buff+=6;
////    *(buff++)='p';
////    *(buff++)='p';
////    *(buff++)='m';
////    buff+=2;
////    
////    *(buff++)='H';
////    *(buff++)='2';
////    *(buff++)='S';
////    *(buff++)='O';
////    *(buff++)='4';
////    *(buff++)=':';
////    Num16_to_ASIC(Dwin_Measurements .H2SO4_Concentration  ,buff,5,1);
////    buff+=6;
////    *(buff++)='p';
////    *(buff++)='p';
////    *(buff++)='m';
////    buff+=2;

//}
/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
int USBH_USR_MSC_Application(void)
{
    static FRESULT res;
    __align(4) unsigned char write_buffer[128];
	unsigned int counter;
	int i;
    char USB_File_Name[30]="0:/1999-12-12 23-59-59.txt";

    if(Dwin_PostedCommands .Save_to_Disk ==1)
    {
        USBH_USR_ApplicationState= USH_USR_HS_INIT;
        Dwin_PostedCommands .Save_to_Disk =0;
        Dwin_Page_to_Switch =DWIN_USB_Saving_Page ;
    }
    switch (USBH_USR_ApplicationState)
    {
        case USH_USR_HS_INIT:
            //初始化文件系统 
            if (f_mount( 0, &fatfs ) != FR_OK ) 
            {
                Dwin_Page_to_Switch =DWIN_USB_Save_Err_Page ;
                USBH_USR_ApplicationState = USH_USR_HS_DONE;
                return(-1);
            }
            else
            {
				//usart1.printf("\r\n\r\n f_mount successful!\r\n");
			}				
            //判断U盘是否写保护
            if (USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
            {
				//usart1.printf(" U disk is protect!\r\n");
                //while(1){;}
                Dwin_Page_to_Switch =DWIN_USB_Save_Err_Page ;
                USBH_USR_ApplicationState = USH_USR_HS_DONE;
                return(-1);       //add by yz 20171025
            }
		
            //跳转到测试程序
            USBH_USR_ApplicationState = USH_USR_HS_TEST;
            break;
		
		case USH_USR_HS_TEST:
			USB_OTG_BSP_mDelay(10);    //呵呵
			//res = f_open(&file,"0:/test.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //打开驱动器0上的源文件
            //将时间嵌入文件名
            for(i=0;i<19;i++) USB_File_Name [i+3]=Dwin_RTC .date [i];
        
            res = f_open(&file,USB_File_Name,FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //打开驱动器0上的源文件
			if(res != FR_OK) //打开文件失败
            {
				//usart1.printf(" f_open error!\r\n");
				//while(1){; }				
                Dwin_Page_to_Switch =DWIN_USB_Save_Err_Page ;
                USBH_USR_ApplicationState = USH_USR_HS_DONE;
                return(-1);       //add by yz 20171025
			}
            else
            {
				//usart1.printf(" f_open successful!\r\n");
			}
			
			res = f_lseek(&file,0);                                                  //移动写指针到文件首
			if(res != FR_OK)
            {
				//usart1.printf(" f_lseek error!\r\n");
				//while(1){;}
				Dwin_Page_to_Switch =DWIN_USB_Save_Err_Page ;
                USBH_USR_ApplicationState = USH_USR_HS_DONE;
                return(-1);       //add by yz 20171025
			}
            else
            {
				//usart1.printf(" f_lseek successful!\r\n");
			}
    
            res = f_write(&file,"产品编号 操作员编号 日期       时间      数据 \r\n",48,&counter);
            
            i=0;
            
            while(SD_Read_Record ((uint8_t*)write_buffer,i)==FR_OK )
            {
                res = f_write(&file,write_buffer,100,&counter); 
                if(res==FR_OK )
                {
                    i++;
                }
//                else
//                {
//                    i++;
//                }
                
            }
            
            
////            for(j=0;j<30;j++)
////            {
//                for(i=0;i<60;i++)
//                {
////                    SD_Read_Record ((uint8_t*)write_buffer,i+5);
////                    SD_Read_Record ((uint8_t*)write_buffer,i+4);
////                    SD_Read_Record ((uint8_t*)write_buffer,i+3);
////                    SD_Read_Record ((uint8_t*)write_buffer,i+2);
////                    SD_Read_Record ((uint8_t*)write_buffer,i+1);
////                    SD_Read_Record ((uint8_t*)write_buffer,i);
////                    f_write(&file,write_buffer,100,&counter);


////                    SD_Read_Record ((uint8_t*)write_buffer,i);
////                    f_write(&file,write_buffer,100,&counter);
////                    SD_Read_Record ((uint8_t*)write_buffer,i);
////                    f_write(&file,write_buffer,100,&counter);
//                } 
                //f_write(&file,write_buffer,100,&counter);
//            }
            
            
//            for(i=0;i<1000;i++)
//            {
//                SD_Read_Record ((uint8_t*)write_buffer,19); 
//                f_write(&file,write_buffer,100,&counter);
//            
//            }
			f_close(&file); 
			f_mount(0, NULL); 
			Dwin_Page_to_Switch =DWIN_USB_Save_Compeleted_Page ;			
			USBH_USR_ApplicationState = USH_USR_HS_DONE; 
			break;
			
        case USH_USR_HS_DONE:

            break;
	
        default:
            break;
    }
  return(0);
}
/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
  //USBH_USR_ApplicationState = USH_USR_HS_INIT;
}


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

