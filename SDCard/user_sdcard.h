#ifndef __USER_SDCARD_H
#define __USER_SDCARD_H


#include "stm32f10x.h"
#include "stm32_eval_sdio_sd.h"


#define SD_BLOCK_SIZE            512 /* Block Size in Bytes */

uint8_t SD_disk_status(void);
uint8_t SD_disk_initialize(void);
uint8_t SD_disk_read( uint8_t *Buff, uint32_t Sector, uint32_t Count );
uint8_t SD_disk_write( uint8_t *Buff, uint32_t Sector, uint32_t Count );

void SD_test(void);


#endif	/* __USER_SDCARD_H */


/*---------------------------- END OF FILE ----------------------------*/

