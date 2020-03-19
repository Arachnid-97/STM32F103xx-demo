/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#include "user_sdcard.h"


/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
#define DEV_SD		3	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT;
    int result;

    switch (pdrv) {
    case DEV_RAM :
//		result = RAM_disk_status();

        // translate the reslut code here

        break;

    case DEV_MMC :
//		result = MMC_disk_status();

        // translate the reslut code here

        break;

    case DEV_USB :
//		result = USB_disk_status();

        // translate the reslut code here

        break;

    case DEV_SD :
        result = SD_disk_status();

        // translate the reslut code here
        if(1 == result)
            stat &= ~STA_NOINIT;

        break;
    }
    return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT;
    int result;

    switch (pdrv) {
    case DEV_RAM :
//		result = RAM_disk_initialize();

        // translate the reslut code here

        break;

    case DEV_MMC :
//		result = MMC_disk_initialize();

        // translate the reslut code here

        break;

    case DEV_USB :
//		result = USB_disk_initialize();

        // translate the reslut code here

        break;

    case DEV_SD :
        result = SD_disk_initialize();

        // translate the reslut code here
        if(1 == result)
            stat &= ~STA_NOINIT;

        break;
    }
    return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,		/* Physical drive nmuber to identify the drive */
    BYTE *buff,		/* Data buffer to store read data */
    LBA_t sector,	/* Start sector in LBA */
    UINT count		/* Number of sectors to read */
)
{
    DRESULT res = RES_ERROR;
    int result;

    switch (pdrv) {
    case DEV_RAM :
        // translate the arguments here

//		result = RAM_disk_read(buff, sector, count);

        // translate the reslut code here

        break;

    case DEV_MMC :
        // translate the arguments here

//		result = MMC_disk_read(buff, sector, count);

        // translate the reslut code here

        break;

    case DEV_USB :
        // translate the arguments here

//		result = USB_disk_read(buff, sector, count);

        // translate the reslut code here

        break;

    case DEV_SD :
        // translate the arguments here

        result = SD_disk_read(buff, sector, count);

        // translate the reslut code here
        if(1 == result)
            res = RES_OK;

        break;
    }

    return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
    BYTE pdrv,			/* Physical drive nmuber to identify the drive */
    const BYTE *buff,	/* Data to be written */
    LBA_t sector,		/* Start sector in LBA */
    UINT count			/* Number of sectors to write */
)
{
    DRESULT res = RES_ERROR;
    int result;

    switch (pdrv) {
    case DEV_RAM :
        // translate the arguments here

//		result = RAM_disk_write(buff, sector, count);

        // translate the reslut code here

        break;

    case DEV_MMC :
        // translate the arguments here

//		result = MMC_disk_write(buff, sector, count);

        // translate the reslut code here

        break;

    case DEV_USB :
        // translate the arguments here

//		result = USB_disk_write(buff, sector, count);

        // translate the reslut code here

        break;

    case DEV_SD :
        // translate the arguments here

        result = SD_disk_write((uint8_t *)buff, sector, count);

        // translate the reslut code here
        if(1 == result)
            res = RES_OK;

        break;
    }

    return res;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,		/* Physical drive nmuber (0..) */
    BYTE cmd,		/* Control code */
    void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT res = RES_ERROR;
    int result;

    switch (pdrv) {
    case DEV_RAM :

        // Process of the command for the RAM drive

        break;

    case DEV_MMC :

        // Process of the command for the MMC/SD card

        break;

    case DEV_USB :

        // Process of the command the USB drive

        break;

    case DEV_SD :
        switch (cmd) {
        case CTRL_SYNC :		/* Wait for end of internal write process of the drive */
			result = 1;
            break;

        case GET_SECTOR_COUNT :	/* Get drive capacity in unit of sector (DWORD) */
			*(DWORD * )buff = SDCardInfo.CardCapacity /SDCardInfo.CardBlockSize;
			result = 1;
            break;

		case GET_SECTOR_SIZE :	// Get R/W sector size (WORD) 
			*(WORD * )buff = SD_BLOCK_SIZE;
			result = 1;
			break;
		
        case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
			*(DWORD * )buff = 1;
			result = 1;
            break;

        case CTRL_TRIM :		/* Erase a block of sectors (used when _USE_ERASE == 1) */
			result = 1;
            break;

        default:
            res = RES_PARERR;
        }

        // Process of the command the SD drive
        if(1 == result)
            res = RES_OK;

        break;
    }

    return res;
}

/*---------------------------------------------------------*/
/* User provided RTC function for FatFs module             */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called back     */
/* from FatFs module.                                      */

#if !FF_FS_NORTC && !FF_FS_READONLY
DWORD get_fattime (void)
{
	return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				/* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}

#endif


/*---------------------------- END OF FILE ----------------------------*/

