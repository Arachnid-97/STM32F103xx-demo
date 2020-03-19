#include "bsp_flash.h"


/************************************************
函数名称 ： Flash_ReadHalfWord
功    能 ： 从 Flash连续读半个字（16 bit）
参    数 ： address ---- 地址位
			Buff ---- 读取的数据
			Len ---- 长度
返 回 值 ： 无
*************************************************/
void Flash_ReadHalfWord( uint32_t addr, uint8_t *Buff, uint16_t Len )
{
    uint8_t k;
    uint32_t Address;
    
    Address = WRITE_START_ADDR + addr;
    for(k = 0;k < Len;k++)
    {
        *(Buff + k) =  (*(vu32*) Address);	//读指定地址的半个字的数据
        Address += 2;						//地址偏移 2个字节        
    }
}

/************************************************
函数名称 ： Flash_WritenHalfWord
功    能 ： 向 Flash写半个字（16 bit）
参    数 ： address ---- 地址位
			Buff ---- 存储的数据
			Len ---- 长度
返 回 值 ： 0 / 1
*************************************************/
uint8_t Flash_WritenHalfWord( uint32_t addr, uint8_t *Buff, uint16_t Len )
{
	volatile FLASH_Status FLASHStatus;
    uint8_t k = 0;
    uint32_t Address;
 
    Address = WRITE_START_ADDR + addr;
    FLASHStatus = FLASH_COMPLETE;
    FLASH_Unlock();							//解锁
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清除所有标志
    FLASHStatus = FLASH_ErasePage(Address);	//扇区擦除
    if(FLASHStatus == FLASH_COMPLETE)
    {
        for(k = 0;(k<Len) && (FLASHStatus == FLASH_COMPLETE);k++)
        {
            FLASHStatus = FLASH_ProgramHalfWord(Address, *(Buff + k));		//写入半个字（16位）的数据入指定地址
            Address = Address + 2;			//地址偏移 2个字节
        }        
        FLASH_Lock();						//重新上锁，防止误写入
    }
    else
    {
        return 0;
    }
	
    if(FLASHStatus == FLASH_COMPLETE)
    {
        return 1;
    }
    return 0;
}

/************************************************
函数名称 ： ReadFlash_HalfWord
功    能 ： 从 Flash读半个字（16 bit）
参    数 ： address ---- 地址位
			Buff ---- 读取的数据
			Len ---- 长度
返 回 值 ： 无
*************************************************/
uint16_t ReadFlash_HalfWord( uint32_t addr )
{
    uint32_t Address;
    
    Address = WRITE_START_ADDR + addr;
	
    return (*(vu16*) Address);				//读指定地址的半个字的数据
}


/*---------------------------- END OF FILE ----------------------------*/


