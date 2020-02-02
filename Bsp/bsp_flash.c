#include "bsp_flash.h"


/************************************************
�������� �� Flash_ReadHalfWord
��    �� �� �� Flash����������֣�16 bit��
��    �� �� address ---- ��ַλ
			Buff ---- ��ȡ������
			Len ---- ����
�� �� ֵ �� ��
*************************************************/
void Flash_ReadHalfWord( uint32_t addr, uint8_t *Buff, uint16_t Len )
{
    uint8_t k;
    uint32_t Address;
    
    Address = WRITE_START_ADDR + addr;
    for(k = 0;k < Len;k++)
    {
        *(Buff + k) =  (*(vu32*) Address);	//��ָ����ַ�İ���ֵ�����
        Address += 2;						//��ַƫ�� 2���ֽ�        
    }
}

/************************************************
�������� �� Flash_WritenHalfWord
��    �� �� �� Flashд����֣�16 bit��
��    �� �� address ---- ��ַλ
			Buff ---- �洢������
			Len ---- ����
�� �� ֵ �� 0 / 1
*************************************************/
uint8_t Flash_WritenHalfWord( uint32_t addr, uint8_t *Buff, uint16_t Len )
{
	volatile FLASH_Status FLASHStatus;
    uint8_t k = 0;
    uint32_t Address;
 
    Address = WRITE_START_ADDR + addr;
    FLASHStatus = FLASH_COMPLETE;
    FLASH_Unlock();							//����
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//������б�־
    FLASHStatus = FLASH_ErasePage(Address);	//��������
    if(FLASHStatus == FLASH_COMPLETE)
    {
        for(k = 0;(k<Len) && (FLASHStatus == FLASH_COMPLETE);k++)
        {
            FLASHStatus = FLASH_ProgramHalfWord(Address, *(Buff + k));		//д�����֣�16λ����������ָ����ַ
            Address = Address + 2;			//��ַƫ�� 2���ֽ�
        }        
        FLASH_Lock();						//������������ֹ��д��
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
�������� �� ReadFlash_HalfWord
��    �� �� �� Flash������֣�16 bit��
��    �� �� address ---- ��ַλ
			Buff ---- ��ȡ������
			Len ---- ����
�� �� ֵ �� ��
*************************************************/
uint16_t ReadFlash_HalfWord( uint32_t addr )
{
    uint32_t Address;
    
    Address = WRITE_START_ADDR + addr;
	
    return (*(vu16*) Address);				//��ָ����ַ�İ���ֵ�����
}


/*---------------------------- END OF FILE ----------------------------*/


