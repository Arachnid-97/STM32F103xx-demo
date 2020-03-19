#include "user_sdcard.h"
#include <string.h>
#include "bsp_uart.h"


/* 不使用测试功能请注释掉该宏 */
#define _SDCARD_TEST

/* User defined variables ----------------------------------------------------*/



#ifdef _SDCARD_TEST
/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private macro -------------------------------------------------------------*/
#define NUMBER_OF_BLOCKS      32  /* For Multi Blocks operation (Read/Write) */
#define MULTI_BUFFER_SIZE    (SD_BLOCK_SIZE * NUMBER_OF_BLOCKS)

#define SD_OPERATION_ERASE          0
#define SD_OPERATION_BLOCK          1
#define SD_OPERATION_MULTI_BLOCK    2
#define SD_OPERATION_END            3

/* Private variables ---------------------------------------------------------*/
uint8_t Buffer_Block_Tx[SD_BLOCK_SIZE], Buffer_Block_Rx[SD_BLOCK_SIZE];
uint8_t Buffer_MultiBlock_Tx[MULTI_BUFFER_SIZE], Buffer_MultiBlock_Rx[MULTI_BUFFER_SIZE];
volatile TestStatus EraseStatus = FAILED, TransferStatus1 = FAILED, TransferStatus2 = FAILED;
SD_Error Status = SD_OK;
__IO uint32_t SDCardOperation = SD_OPERATION_ERASE;

/* Private function prototypes -----------------------------------------------*/
void SD_EraseTest(void);
void SD_SingleBlockTest(void);
void SD_MultiBlockTest(void);
void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset);
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);
TestStatus eBuffercmp(uint8_t* pBuffer, uint32_t BufferLength);

#endif /* _SDCARD_TEST */



/* User defined Fatfs functions ----------------------------------------------*/
/*-----------------------------------------------------------------------*/
/* Get SD disk status                                                    */
/*-----------------------------------------------------------------------*/
uint8_t SD_disk_status(void)
{
    return 1;
}

/*-----------------------------------------------------------------------*/
/* Initialize SD disk drive                                              */
/*-----------------------------------------------------------------------*/
uint8_t SD_disk_initialize(void)
{
    if(SD_Init() == SD_OK)
        return 1;
    else
        return 0;
}

/*-----------------------------------------------------------------------*/
/* Read SD sector(s)                                                     */
/*-----------------------------------------------------------------------*/
uint8_t SD_disk_read( uint8_t *Buff, uint32_t Sector, uint32_t Count )
{
    SD_Error SD_state = SD_OK;
	uint8_t temp = 0;

	if((uint32_t)Buff & 3)
	{
		_Bool flag = 1;
		uint32_t scratch[SD_BLOCK_SIZE / 4];
		while(Count--)
		{
			flag = SD_disk_read((void *)scratch, Sector++, 1);

			if(flag != 1)
			{
				break;
			}
			memcpy(Buff, scratch, SD_BLOCK_SIZE);
			Buff += SD_BLOCK_SIZE;
		}
	}
	else
	{
		SD_state = SD_ReadMultiBlocks(Buff,(uint64_t)Sector*SD_BLOCK_SIZE,	\
										SD_BLOCK_SIZE,Count);
	}
	
    if(SD_state == SD_OK)
    {
        /* Check if the Transfer is finished */
        SD_state=SD_WaitReadOperation();
        while(SD_GetStatus() != SD_TRANSFER_OK);
    }
	if(SD_state != SD_OK)
		temp = 0;
	else
		temp = 1;
	
	return temp;
}

/*-----------------------------------------------------------------------*/
/* Write SD sector(s)                                                    */
/*-----------------------------------------------------------------------*/
uint8_t SD_disk_write( uint8_t *Buff, uint32_t Sector, uint32_t Count )
{
    SD_Error SD_state = SD_OK;
	uint8_t temp = 0;

	if((uint32_t)Buff & 3)
	{
		_Bool flag = 1;
		uint32_t scratch[SD_BLOCK_SIZE / 4];
		while(Count--)
		{
			memcpy(scratch, Buff, SD_BLOCK_SIZE);
			flag = SD_disk_write((void *)scratch, Sector++, 1);

			if(flag != 1)
			{
				break;
			}
			Buff += SD_BLOCK_SIZE;
		}
	}
	else
	{
		SD_state = SD_WriteMultiBlocks(Buff,(uint64_t)Sector*SD_BLOCK_SIZE,	\
										SD_BLOCK_SIZE,Count);
	}
	
    if(SD_state == SD_OK)
    {
        /* Check if the Transfer is finished */
        SD_state=SD_WaitWriteOperation();
        while(SD_GetStatus() != SD_TRANSFER_OK);
    }
	if(SD_state != SD_OK)
		temp = 0;
	else
		temp = 1;
	
	return temp;
}


/* Test functions ------------------------------------------------------------*/
#ifdef _SDCARD_TEST

/************************************************
函数名称 ： SD_test
功    能 ： SD测试
参    数 ： 无
返 回 值 ： 无
*************************************************/
void SD_test(void)
{
    /*------------------------------ SD Init ---------------------------------- */
    if((Status = SD_Init()) != SD_OK)
    {
        printf("SD卡初始化失败，请确保SD卡已正确接入开发板，或换一张SD卡测试！\n");
    }

    while((Status == SD_OK) && (SDCardOperation != SD_OPERATION_END) && (SD_Detect()== SD_PRESENT))
    {
        switch(SDCardOperation)
        {
        /*-------------------------- SD Erase Test ---------------------------- */
        case (SD_OPERATION_ERASE):
        {
            SD_EraseTest();
            SDCardOperation = SD_OPERATION_BLOCK;
            break;
        }
        /*-------------------------- SD Single Block Test --------------------- */
        case (SD_OPERATION_BLOCK):
        {
            SD_SingleBlockTest();
            SDCardOperation = SD_OPERATION_MULTI_BLOCK;
            break;
        }
        /*-------------------------- SD Multi Blocks Test --------------------- */
        case (SD_OPERATION_MULTI_BLOCK):
        {
            SD_MultiBlockTest();
            SDCardOperation = SD_OPERATION_END;
            break;
        }
        }
    }
}

/**
  * @brief  Tests the SD card erase operation.
  * @param  None
  * @retval None
  */
void SD_EraseTest(void)
{
    /*------------------- Block Erase ------------------------------------------*/
    if (Status == SD_OK)
    {
        /* Erase NumberOfBlocks Blocks of WRITE_BL_LEN(512 Bytes) */
        Status = SD_Erase(0x00, (SD_BLOCK_SIZE * NUMBER_OF_BLOCKS));
    }

    if (Status == SD_OK)
    {
        Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, SD_BLOCK_SIZE, NUMBER_OF_BLOCKS);

        /* Check if the Transfer is finished */
        Status = SD_WaitReadOperation();

        /* Wait until end of DMA transfer */
        while(SD_GetStatus() != SD_TRANSFER_OK);
    }

    /* Check the correctness of erased blocks */
    if (Status == SD_OK)
    {
        EraseStatus = eBuffercmp(Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE);
    }

    if(EraseStatus == PASSED)
    {
        printf("SD卡擦除测试成功！\n");
    }
    else
    {
        printf("SD卡擦除测试失败！\n");
    }
}

/**
  * @brief  Tests the SD card Single Blocks operations.
  * @param  None
  * @retval None
  */
void SD_SingleBlockTest(void)
{
    /*------------------- Block Read/Write --------------------------*/
    /* Fill the buffer to send */
    Fill_Buffer(Buffer_Block_Tx, SD_BLOCK_SIZE, 0x320F);

    if (Status == SD_OK)
    {
        /* Write block of 512 bytes on address 0 */
        Status = SD_WriteBlock(Buffer_Block_Tx, 0x00, SD_BLOCK_SIZE);
        /* Check if the Transfer is finished */
        Status = SD_WaitWriteOperation();
        while(SD_GetStatus() != SD_TRANSFER_OK);
    }

    if (Status == SD_OK)
    {
        /* Read block of 512 bytes from address 0 */
        Status = SD_ReadBlock(Buffer_Block_Rx, 0x00, SD_BLOCK_SIZE);
        /* Check if the Transfer is finished */
        Status = SD_WaitReadOperation();
        while(SD_GetStatus() != SD_TRANSFER_OK);
    }

    /* Check the correctness of written data */
    if (Status == SD_OK)
    {
        TransferStatus1 = Buffercmp(Buffer_Block_Tx, Buffer_Block_Rx, SD_BLOCK_SIZE);
    }

    if(TransferStatus1 == PASSED)
    {
        printf("Single block 测试成功！\n");
    }
    else
    {
        printf("Single block 测试失败，请确保SD卡正确接入开发板，或换一张SD卡测试！\n");
    }
}

/**
  * @brief  Tests the SD card Multiple Blocks operations.
  * @param  None
  * @retval None
  */
void SD_MultiBlockTest(void)
{
    /*--------------- Multiple Block Read/Write ---------------------*/
    /* Fill the buffer to send */
    Fill_Buffer(Buffer_MultiBlock_Tx, MULTI_BUFFER_SIZE, 0x0);

    if (Status == SD_OK)
    {
        /* Write multiple block of many bytes on address 0 */
        Status = SD_WriteMultiBlocks(Buffer_MultiBlock_Tx, 0x00, SD_BLOCK_SIZE, NUMBER_OF_BLOCKS);
        /* Check if the Transfer is finished */
        Status = SD_WaitWriteOperation();
        while(SD_GetStatus() != SD_TRANSFER_OK);
    }

    if (Status == SD_OK)
    {
        /* Read block of many bytes from address 0 */
        Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, SD_BLOCK_SIZE, NUMBER_OF_BLOCKS);
        /* Check if the Transfer is finished */
        Status = SD_WaitReadOperation();
        while(SD_GetStatus() != SD_TRANSFER_OK);
    }

    /* Check the correctness of written data */
    if (Status == SD_OK)
    {
        TransferStatus2 = Buffercmp(Buffer_MultiBlock_Tx, Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE);
    }

    if(TransferStatus2 == PASSED)
    {
        printf("Multi block 测试成功！");
    }
    else
    {
        printf("Multi block 测试失败，请确保SD卡正确接入开发板，或换一张SD卡测试！");
    }
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer1 != *pBuffer2)
        {
            return FAILED;
        }

        pBuffer1++;
        pBuffer2++;
    }

    return PASSED;
}

/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferLength: size of the buffer to fill
  * @param  Offset: first value to fill on the Buffer
  * @retval None
  */
void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset)
{
    uint16_t index = 0;

    /* Put in global buffer same values */
    for (index = 0; index < BufferLength; index++)
    {
        pBuffer[index] = index + Offset;
    }
}

/**
  * @brief  Checks if a buffer has all its values are equal to zero.
  * @param  pBuffer: buffer to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer values are zero
  *         FAILED: At least one value from pBuffer buffer is different from zero.
  */
TestStatus eBuffercmp(uint8_t* pBuffer, uint32_t BufferLength)
{
    while (BufferLength--)
    {
        /* In some SD Cards the erased state is 0xFF, in others it's 0x00 */
        if ((*pBuffer != 0xFF) && (*pBuffer != 0x00))
        {
            return FAILED;
        }

        pBuffer++;
    }

    return PASSED;
}

#endif /* _SDCARD_TEST */


/*---------------------------- END OF FILE ----------------------------*/


