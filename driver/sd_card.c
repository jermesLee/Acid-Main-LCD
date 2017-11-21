/******************************************************************************
*******************************************************************************
**                     板级支持包                                              
**                                                                            
**                     知行科技   Copyright 2017;                              
*******************************************************************************
** Filename      : sd_card.c
** Version       : V1.00    
** Programmer(s) : YZ       
** Date          : 2017.7.2 
** Discription   :中断向量配置，启动文件中的中断向量已改写，只需将板级中断函
                  数指针添加到本文件定义的中断向量数组BSP_IntVectTbl即可。
** Note(s)		 :         
*******************************************************************************
******************************************************************************/

#define  SD_CARD_MODULE
/******************************************************************************
*                      头文件                                                  
******************************************************************************/
//#include <includes.h>
#include "stm32f4xx.h"
#include "sd_card.h"

#include "stm324xg_eval_sdio_sd.h"
#include "dwin.h"
#include "sensor.h"
#include "ff.h"
#include "diskio.h"










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
static uint16_t String_to_uint16(uint8_t *str);
static void Num16_to_ASIC(uint16_t Num,unsigned char *buff,uint16_t Field,uint16_t Decimal);



/******************************************************************************
*                      内部函数定义                                                
******************************************************************************/
static void Num16_to_ASIC(uint16_t Num,unsigned char *buff,uint16_t Field,uint16_t Decimal)
{
    uint16_t i;
    uint16_t counter;
    counter =Field;
    
    buff+=(Field-1);
    for(i=0;i<Decimal ;i++) //小数部分
    {
        *(buff--)=Num%10 +0x30;
        Num/=10;
        counter --;
    }
    if(Decimal !=0)
    {
        *(buff--)='.';
        counter--;
    }
    *(buff--)=Num%10 +0x30;
    Num/=10;
    counter --;
    while(Num!=0)
    {
        *(buff--)=Num%10 +0x30;
        Num/=10;
        counter--;
    }
    while(counter--)
    {
        *(buff--)=' ';
    }
}


static uint16_t String_to_uint16(uint8_t *str)
{
    uint16_t num=0;
    uint16_t tmp;
    
    tmp=*(str++)-0x30;  
    num+=tmp*10000;
    tmp=*(str++)-0x30;  
    num+=tmp*1000;
    tmp=*(str++)-0x30;  
    num+=tmp*100;
    tmp=*(str++)-0x30;  
    num+=tmp*10;
    tmp=*(str++)-0x30;  
    num+=tmp*1;
    
    return num;    
}


/******************************************************************************
*                      外部函数定义                                          
******************************************************************************/
//__align(4)    uint8_t R_Buff[512];
//unsigned char write_buffer[512];     //写文件缓冲区
unsigned char read_buffer[128];        //读文件缓冲区
unsigned int counter;

//读取出厂参数
void SD_Read_Default_Settings(void)
{
    int i;
    FIL file;      //文件对象
    FATFS fatfs;   //逻辑驱动器的工作区
    uint16_t * p_data;
    
    static FRESULT res; 
    
    
    res = f_mount(1,&fatfs);    //挂载SD卡
	res = f_open(&file,"1:/default_settings.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //打开驱动器0上的源文件
    res = f_lseek(&file,0);     //移动写指针到文件首
    

    p_data=(uint16_t *)&Dwin_Default_Settings;
    for(i=0;i<APP_DWIN_RAM_NUM_OF_SETTINGS;i++)
    {
        res = f_read(&file,read_buffer,27,&counter);  
        *p_data=String_to_uint16(&read_buffer [20]);
        p_data++;
    }
    if(res){;}     //avoid warnings
    f_close(&file);//关闭源文件
}

void SD_Load_Settings(void)
{
    //int i;
    FIL file;                                                                //文件对象
    FATFS fatfs;                                                             //逻辑驱动器的工作区
    static FRESULT res;   

  	res = f_mount(1,&fatfs);                                            //挂载SD卡
	res = f_open(&file,"1:/settings.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //打开驱动器0上的源文件	
	
    if(file.fsize >10)  //有参数文件
    {
        res = f_lseek(&file,0);
        res = f_read(&file,&Dwin_Settings,sizeof(Dwin_Settings),&counter); 
        f_close(&file); 
    }
    else    //无参数文件
    {
        Dwin_Settings=Dwin_Default_Settings ;
        SD_Save_Settings();
    
    }
    
    if(res){;}     //avoid warnings
}


void SD_Save_Settings(void)
{
    //int i;
    FIL file;                                                                //文件对象
    FATFS fatfs;                                                             //逻辑驱动器的工作区
    static FRESULT res;   

  	res = f_mount(1,&fatfs);                                            //挂载SD卡
	res = f_open(&file,"1:/settings.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //打开驱动器0上的源文件	
	res = f_lseek(&file,0);                                                  //移动写指针到文件首
	res = f_write (&file,&Dwin_Settings,sizeof(Dwin_Settings),&counter);                          //将缓冲器中的内容写入源文件
	f_close(&file); 
    if(res){;}     //avoid warnings    //关闭源文件
}

void SD_Save_Data(void)
{
    //int i;
    FIL file;                           //文件对象
    FATFS fatfs;                        //逻辑驱动器的工作区
    static FRESULT res;   

  	res = f_mount(1,&fatfs);           //挂载SD卡
	res = f_open(&file,"1:/data.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);    //打开驱动器0上的源文件	
	res = f_lseek(&file,file.fsize);                                          //移动写指针到文件首
    
	res = f_write (&file,&Dwin_Settings .Product_Num ,2,&counter);            //将缓冲器中的内容写入源文件
	res = f_write (&file,&Dwin_Settings .Operator_Num,2,&counter);            //将缓冲器中的内容写入源文件
    res = f_write (&file,&Dwin_RTC.date ,sizeof(Dwin_RTC.date),&counter);
    
	res = f_write (&file,&Dwin_Measurements .ADT ,2,&counter);                //将缓冲器中的内容写入源文件
	res = f_write (&file,&Dwin_Measurements .MMT ,2,&counter);                //将缓冲器中的内容写入源文件
	res = f_write (&file,&Dwin_Measurements .SO3_Concentration  ,2,&counter); //将缓冲器中的内容写入源文件
	res = f_write (&file,&Dwin_Measurements .H2SO4_Concentration ,2,&counter);//将缓冲器中的内容写入源文件
	
    f_close(&file);      //关闭源文件
    if(res){;}     //avoid warnings
}

void SD_Delete_Data(void)
{
    //int i;
    //FIL file;      //文件对象
    FATFS fatfs;     //逻辑驱动器的工作区
    //static FRESULT res;   

  	f_mount(1,&fatfs);                      //挂载SD卡
	//f_open(&file,"1:/data.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //打开驱动器0上的源文件	
    f_unlink ("1:/data.txt");
}
FRESULT SD_Read_Record(uint8_t * buff,uint32_t index)
{
#define SD_RECORD_LEN (4+20+8)        //时间20B，2个参数4B，4个测量值8B
    int i;
    FIL file;      //文件对象
    FATFS fatfs;   //逻辑驱动器的工作区
    uint16_t * p_data;
    
    static FRESULT res; 
    
   	res = f_mount(1,&fatfs);    //挂载SD卡
	//res = f_open(&file,"1:/data1.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //打开驱动器0上的源文件
    res = f_open(&file,"1:/data.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS );   //打开驱动器0上的源文件
    res = f_lseek(&file,index*SD_RECORD_LEN);     //移动指针
    res = f_read(&file,read_buffer,SD_RECORD_LEN,&counter);  
    f_close(&file);//关闭源文件    
    if(counter==SD_RECORD_LEN)
    {
        p_data=(uint16_t*)read_buffer;    //指向记录里的第一个整型数据（产品编号）
        Num16_to_ASIC(*p_data ,buff,8,0); //写入ASIC buff,8位宽度
        buff+=(8);
        *(buff++)=' ';  
        
        p_data++;   //指向记录里第二个整型数据（操作员编号）
        Num16_to_ASIC(*p_data ,buff,10,0); //写入ASIC buff,10位宽度
        buff+=(10);
        *(buff++)=' ';  
    
        
        for(i=0;i<20;i++)      //RTC文本
        {
            *(buff++)=read_buffer [i+4];
        }
        *(buff-4)=':';      //时分秒的间隔符修改为冒号
        *(buff-7)=':';
        

        *(buff++)='A';
        *(buff++)='D';
        *(buff++)='T';
        *(buff++)=':';
        p_data+=11; //20字符折合10个整型 ，指向ADT数据 (不知为什么没对齐)
        Num16_to_ASIC(*p_data ,buff,5,1); //写入ASIC buff,5位宽度
        buff+=(5);
        *(buff++)=' ';  
        *(buff++)='o';
        *(buff++)='C';
        *(buff++)=' ';  
        *(buff++)=' ';  
    
        *(buff++)='M';
        *(buff++)='M';
        *(buff++)='T';
        *(buff++)=':';
        p_data++;       //指向MMT数据
        Num16_to_ASIC(*p_data ,buff,5,1); //写入ASIC buff,5位宽度
        buff+=(5);
        *(buff++)=' ';  
        *(buff++)='o';
        *(buff++)='C';
        *(buff++)=' ';  
        *(buff++)=' ';  
    
        *(buff++)='S';
        *(buff++)='O';
        *(buff++)='3';
        *(buff++)=':';
        p_data++;       //指向SO3数据
        Num16_to_ASIC(*p_data ,buff,5,1); //写入ASIC buff,5位宽度
        buff+=(5);
        *(buff++)=' ';  
        *(buff++)='p';
        *(buff++)='p';
        *(buff++)='m';
        *(buff++)=' '; 
        *(buff++)=' '; 
    
        *(buff++)='H';
        *(buff++)='2';
        *(buff++)='S';
        *(buff++)='O';
        *(buff++)='4';
        *(buff++)=':';
        p_data++;       //指向H2SO4数据
        Num16_to_ASIC(*p_data ,buff,5,1); //写入ASIC buff,5位宽度
        buff+=(5);
        *(buff++)=' ';  
        *(buff++)='p';
        *(buff++)='p';
        *(buff++)='m';
        (*buff++) ='\r';
        (*buff++) ='\n';       //每条记录共100字节        
        
        //res=FR_OK ;        
    }
    else
    {
        //res=!FR_OK ;
        res=FR_NO_FILE ; //临时用一个非零值代替
    }
    return res;
}


void SD_Process(void)
{
    if(Dwin_PostedCommands.Save_Settings==1 )
    {
        SD_Save_Settings();
        Dwin_PostedCommands.Save_Settings=0;
    }
    if(Dwin_PostedCommands.Save_to_Card==1 )
    {
        SD_Save_Data();
        Dwin_PostedCommands.Save_to_Card=0;
    }
    
    if(Dwin_PostedCommands .Delete_Data ==1)
    {
        SD_Delete_Data();
        Dwin_PostedCommands .Delete_Data =0;
    }
    
    
    

}

FRESULT SD_Get_FreeCap(void)
{
    FATFS fs;
    FATFS *pfs;
    DWORD fre_clust;
    DWORD free_bytes;
    FRESULT  res;
                                              
    pfs=&fs;
    f_mount(1,&fs ); // 挂载文件系统

    res = f_getfree("1:/", &fre_clust, &pfs);
    if (res) return res; // 如果f_getfree函数执行错误，返回错误值
    else // 如果成功获得容量
    {
        //TOT_SIZE = (pfs->n_fatent - 2) * pfs->csize/2; //总容量 单位byte
        //FRE_SIZE = fre_clust * pfs->csize/2; // 可用容量 单位byte
        free_bytes= fre_clust * pfs->csize/2;
        Dwin_Measurements .Card_Space =free_bytes/1000;
    }
    f_mount(1, 0); //卸载文件系统
    return FR_OK ;
}

void sd_test(void)
{
//    int i;
//    FIL file;                                                                //文件对象
//    FATFS fatfs;                                                             //逻辑驱动器的工作区
//    static FRESULT res;   
//    
//	res = f_mount(1,&fatfs);                                            //挂载SD卡
//	res = f_open(&file,"1:/test33.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //打开驱动器0上的源文件	
//	res = f_lseek(&file,0);                                                  //移动写指针到文件首
//	
//	for(i = 0;i < 512;i++){
//		write_buffer[i] = i % 256;
//	}
//    write_buffer[0]='a';
//    write_buffer[1]='s';
//    write_buffer[2]='\r';
//    write_buffer[3]='\n';
//    
//    
//	res = f_write(&file,write_buffer,4,&counter);                          //将缓冲器中的内容写入源文件
//	res = f_write(&file,write_buffer,4,&counter); 
//	f_close(&file);                                                          //关闭源文件
//	res = f_open(&file,"1:/test33.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //打开驱动器0上的源文件	
//	res = f_lseek(&file,file.fsize);                                                  //移动写指针到文件首


//	res = f_write(&file,write_buffer,4,&counter);                          //将缓冲器中的内容写入源文件
//	res = f_write(&file,write_buffer,4,&counter);                          //将缓冲器中的内容写入源文件
//	if(res != RES_OK || counter != 512)
//    {
//        ;	
//	}
//	
//	res = f_lseek(&file,0);	                                                 //移动读指针到文件首
//	res = f_read(&file,read_buffer,512,&counter);
//	if(res != RES_OK || counter != 512)
//    {
//        ;
//	}
//	f_close(&file);                                                          //关闭源文件


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
