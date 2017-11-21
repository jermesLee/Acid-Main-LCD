/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "stm324xg_eval_sdio_sd.h"		/* Example: Header file of existing MMC/SDC contorl module */
#include "usb_conf.h"
#include "usbh_msc_core.h"

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */

#define ATA		2
#define MMC		1
#define USB		0

static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST                     USB_Host;

/*-----------------------------------------------------------------------*/
/* Inicializes a Drive                                                    */

DSTATUS disk_initialize (BYTE drv)    /* Physical drive nmuber (0..) */
{
//  DSTATUS stat = STA_NOINIT;
//  
////  if(HCD_IsDeviceConnected(&USB_OTG_Core_dev))
////  {  
////    stat &= ~STA_NOINIT;
////  }
////  
//  return stat;
//    return RES_OK;
    switch (drv) {
        
        case ATA:
            return 0;
        case MMC :
            return RES_OK;
        case USB:
              if(HCD_IsDeviceConnected(&USB_OTG_Core))
              {  
                Stat &= ~STA_NOINIT;
              }
              
              return Stat;

    }
        return RES_OK;

}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	//DSTATUS stat;
	//int result;

	switch (drv) {
//	case ATA :
//		result = ATA_disk_status();
//		// translate the reslut code here

//		return stat;

	case MMC :
//		result = MMC_disk_status();
//		// translate the reslut code here

//		return stat;
       return RES_OK;
	case USB :
		//result = USB_disk_status();
		// translate the reslut code here

		return RES_OK;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	//DRESULT res;
	//int result;
    BYTE status = USBH_MSC_OK;

	switch (drv) {
//	case ATA :
//		result = ATA_disk_read(buff, sector, count);
//		// translate the reslut code here

//		return res;

	case MMC :
//		result = MMC_disk_read(buff, sector, count);
//		// translate the reslut code here

//		return res;
        if( SD_ReadMultiBlocks (buff, 
                                                        sector * 512, 
                                                        512,
                                                        count) != 0)
        {
            return RES_ERROR;
        }
        SD_WaitReadOperation();
        while (SD_GetStatus() != SD_TRANSFER_OK);	

        return RES_OK;
	case USB :
//		result = USB_disk_read(buff, sector, count);
//		// translate the reslut code here

//		return res;
      
          if (drv || !count) return RES_PARERR;
          if (Stat & STA_NOINIT) return RES_NOTRDY;
          
          
          if(HCD_IsDeviceConnected(&USB_OTG_Core))
          {  
            
            do
            {
              status = USBH_MSC_Read10(&USB_OTG_Core, buff,sector,512 * count);
              USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);
              
              if(!HCD_IsDeviceConnected(&USB_OTG_Core))
              { 
                return RES_ERROR;
              }      
            }
            while(status == USBH_MSC_BUSY );
          }
          
          if(status == USBH_MSC_OK)
            return RES_OK;
          return RES_ERROR;
	}
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	//DRESULT res;
	//int result;
      BYTE status = USBH_MSC_OK;

	switch (drv) {
	case ATA :
//		result = ATA_disk_write(buff, sector, count);
//		// translate the reslut code here

//		return res;

	case MMC :
//		result = MMC_disk_write(buff, sector, count);
//		// translate the reslut code here

//		return res;
        if( SD_WriteMultiBlocks ((unsigned char *)buff, 
                                                         sector * 512, 
                                                         512,
                                                         count) != 0)
        {
            return RES_ERROR;
        } 
        SD_WaitWriteOperation();
        while (SD_GetStatus() != SD_TRANSFER_OK);
        return RES_OK;
    
	case USB :
//		result = USB_disk_write(buff, sector, count);
//		// translate the reslut code here

//		return res;
      if (drv || !count) return RES_PARERR;
      if (Stat & STA_NOINIT) return RES_NOTRDY;
      if (Stat & STA_PROTECT) return RES_WRPRT;
      
      
      if(HCD_IsDeviceConnected(&USB_OTG_Core))
      {  
        do
        {
          status = USBH_MSC_Write10(&USB_OTG_Core,(BYTE*)buff,sector,512 * count);
          USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);
          
          if(!HCD_IsDeviceConnected(&USB_OTG_Core))
          { 
            return RES_ERROR;
          }
        }
        
        while(status == USBH_MSC_BUSY );
        
      }
      
      if(status == USBH_MSC_OK)
        return RES_OK;
      return RES_ERROR;

	}
	return RES_PARERR;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	//int result;
    DRESULT res = RES_OK;

	switch (drv) {
//	case ATA :
//		// pre-process here

//		result = ATA_disk_ioctl(ctrl, buff);
//		// post-process here

//		return res;

//	case MMC :
//		// pre-process here

//		result = MMC_disk_ioctl(ctrl, buff);
//		// post-process here

//		return res;

	case USB :
//		// pre-process here

//		result = USB_disk_ioctl(ctrl, buff);
//		// post-process here

//		return res;
              
              if (drv) return RES_PARERR;
              
              res = RES_ERROR;
              
              if (Stat & STA_NOINIT) return RES_NOTRDY;
              
              switch (ctrl) {
              case CTRL_SYNC :		/* Make sure that no pending write process */
                
                res = RES_OK;
                break;
                
              case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
                
                *(DWORD*)buff = (DWORD) USBH_MSC_Param.MSCapacity;
                res = RES_OK;
                break;
                
              case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
                *(WORD*)buff = 512;
                res = RES_OK;
                break;
                
              case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
                
                *(DWORD*)buff = 512;
                
                break;
                
                
              default:
                res = RES_PARERR;
              }
              
              
              
              return res;
  
	}
	return RES_PARERR;
}

