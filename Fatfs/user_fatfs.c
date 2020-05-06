#include "user_fatfs.h"
#include "bsp_uart.h"


#define FATFS_DEBUG_PRINTF(fmt,arg...)		do{\
											if(_DEBUG)\
												printf(""fmt"",##arg);\
											}while(0)

/* ���ò����������� */
#define DRIVER_DISK			"1:"

FATFS FatFs;				/* ÿ���߼����������ļ�ϵͳ���� */
FIL File;					/* �ļ����� */
FRESULT res_sd;				/* FatFs ��������������� */
UINT br, bw;				/* �ļ��� /д�ֽڼ��� */

__attribute__ ((aligned (4)))  \
BYTE FF_Buff[FF_MAX_SS] = "Fatfs�ļ�ϵͳ��д����ʵ��\r\n";	/* Working buffer */


/************************************************
�������� �� FF_Test
��    �� �� Fatfs�ļ�ϵͳ����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void FF_Test(void)
{
	uint32_t num = 50;
	
	FF_System_Creates(DRIVER_DISK, 1);
	FF_ViewRootDir(DRIVER_DISK);
	FF_OpenWrite("1:temp.txt", FF_Buff, num);
	FF_OpenRead("1:temp.txt", &FF_Buff[1024], num);
	
	
    /* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
    f_mount(NULL, DRIVER_DISK, 1);
}

/************************************************
�������� �� FF_System_Creates
��    �� �� Fatfs�ļ�ϵͳע��
��    �� �� Drive ---- �̷�
			Opt ---- 0�����ڲ�Ҫ��װ���ڵ�һ�η��ʸþ�ʱ��װ��
					 1��ǿ�ư�װ�þ��Լ�����Ƿ���Թ���
�� �� ֵ �� ��
*************************************************/
void FF_System_Creates( char *pDrive, uint8_t Opt )
{
	/* Ϊ�߼�������������ע�� */
    res_sd = f_mount(&FatFs, pDrive, Opt);

	if(1 == Opt)
	{
		/* ���û���ļ�ϵͳ�͸�ʽ�������ļ�ϵͳ */
		if(res_sd == FR_NO_FILESYSTEM)
		{
			DEBUG_PRINTF("SD����û���ļ�ϵͳ���������и�ʽ��...\r\n");
			
			res_sd = f_mkfs(pDrive, 0, FF_Buff, sizeof(FF_Buff));	// ��ʽ��

			if(res_sd == FR_OK)
			{
				DEBUG_PRINTF("SD���ѳɹ���ʽ���ļ�ϵͳ��\r\n");
				
				res_sd = f_mount(NULL, pDrive, 1);		// ��ʽ������ȡ������
				
				res_sd = f_mount(&FatFs, pDrive, 1);		// ���¹���
			}
			else
			{
				DEBUG_PRINTF("��ʽ��ʧ�ܡ�\r\n");
				while(1);
			}
		}
		else if(res_sd != FR_OK)
		{
			DEBUG_PRINTF("����SD�����ذ�װ�ļ�ϵͳʧ�ܡ�(error code:%d)\r\n",res_sd);
			DEBUG_PRINTF("��������ԭ��SD����ʼ�����ɹ���\r\n");
			while(1);
		}
		else
		{
			DEBUG_PRINTF("�ļ�ϵͳ���ذ�װ�ɹ������Խ����ļ���д����\r\n");
		}
	}
	else
	{
		DEBUG_PRINTF("���ش�����ɣ�����δ��װ��\r\n");
	}
}

/************************************************
�������� �� FF_OpenWrite
��    �� �� ���ļ���д����Ϣ
��    �� �� pFile ---- ��Ҫ�򿪵��ļ�
			pStr ---- ��Ҫд�����Ϣ
			Len ---- ����
�� �� ֵ �� 0 / 1
*************************************************/
uint8_t FF_OpenWrite( char *pFile, void *pStr, uint16_t Len )
{
	uint8_t temp = 0;
	
    res_sd = f_open(&File, pFile, FA_CREATE_ALWAYS | FA_WRITE );

    if( res_sd == FR_OK )
    {
        DEBUG_PRINTF("���ļ��ɹ���\r\n");
        /* ��ָ���洢������д�뵽�ļ��� */
        res_sd = f_write(&File, pStr, Len, &bw);
        if(res_sd == FR_OK)
        {
            DEBUG_PRINTF("�ļ�д��ɹ���д���ֽ����ݣ�%d\r\n", bw);
            DEBUG_PRINTF("���ļ�д�������Ϊ��%s\r\n", (char*)pStr);
			
			temp = 1;
        }
        else
        {
            DEBUG_PRINTF("�����ļ�д��ʧ�ܡ�(error code:%d)\r\n", res_sd);
        }
        
        f_close(&File);		// ���ٶ�д���ر��ļ�
    }
    else
    {
        DEBUG_PRINTF("������/�����ļ�ʧ�ܡ�\r\n");
    }
	
	return temp;
}

/************************************************
�������� �� FF_OpenRead
��    �� �� ���ļ�����ȡ��Ϣ
��    �� �� pFile ---- ��Ҫ�򿪵��ļ�
			pStr ---- ��Ҫ��ȡ����Ϣ
�� �� ֵ �� 0 / 1
*************************************************/
uint8_t FF_OpenRead( char *pFile, void *pStr, uint16_t Len )
{
	uint8_t temp = 0;
	
    res_sd = f_open(&File, pFile, FA_OPEN_EXISTING | FA_READ);
    if(res_sd == FR_OK)
    {
        DEBUG_PRINTF("���ļ��ɹ���\r\n");
		/* ���ļ����ݶ�ȡ��ָ���洢���� */
        res_sd = f_read(&File, pStr, Len, &br);
        if(res_sd == FR_OK)
        {
            DEBUG_PRINTF("�ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",br);
            DEBUG_PRINTF("��ȡ�õ��ļ�����Ϊ��%s\r\n", (char*)pStr);
			
			temp = 1;
        }
        else
        {
            DEBUG_PRINTF("�����ļ���ȡʧ�ܡ�(error code:%d)\r\n",res_sd);
        }
    }
    else
    {
        DEBUG_PRINTF("�������ļ�ʧ�ܡ�\r\n");
    }
    
    f_close(&File);		// ���ٶ�д���ر��ļ�

	return temp;
}

/************************************************
�������� �� FF_ViewRootDir
��    �� �� Fatfs�ļ�ɨ����ʾ
��    �� �� Drive ---- �̷�
�� �� ֵ �� ��
*************************************************/
void FF_ViewRootDir( char *pDrive )
{
    /* ������ʹ�õľֲ�����ռ�ý϶࣬���޸������ļ�����֤��ջ�ռ乻�� */
    DIR DirInf;
    FILINFO FileInf;
    uint32_t cnt = 0;

    /* �򿪸��ļ��� */
    res_sd = f_opendir(&DirInf, pDrive);
    if (res_sd != FR_OK)
    {
        DEBUG_PRINTF("�����򿪸�Ŀ¼ʧ�ܡ�(error code:%d)\r\n", res_sd);
        return;
    }

    /* ��ȡ��ǰ�ļ����µ��ļ���Ŀ¼ */
	DEBUG_PRINTF("��ǰ�ļ����µ��ļ���Ϣ��");
    FATFS_DEBUG_PRINTF("\r\n|      ����      |  �ļ���С  | �ļ���\r\n");
    for (cnt = 0; ;cnt++)
    {
        res_sd = f_readdir(&DirInf, &FileInf);         /* ��ȡĿ¼��������Զ����� */
        if (res_sd != FR_OK || FileInf.fname[0] == 0)
        {
            break;
        }

        if (FileInf.fname[0] == '.')
        {
            continue;
        }

        /* �ж����ļ����ͼ�Ŀ¼Ŀ¼ */
		switch(FileInf.fattrib)
		{
			case AM_DIR:
				FATFS_DEBUG_PRINTF("| (0x%02X)��Ŀ¼  ", FileInf.fattrib);
				break;
			case AM_RDO:
				FATFS_DEBUG_PRINTF("| (0x%02X)ֻ���ļ�", FileInf.fattrib);
				break;
			case AM_HID:
				FATFS_DEBUG_PRINTF("| (0x%02X)�����ļ�", FileInf.fattrib);
				break;
			case AM_SYS:
				FATFS_DEBUG_PRINTF("| (0x%02X)ϵͳ�ļ�", FileInf.fattrib);
				break;
			case AM_ARC:
				FATFS_DEBUG_PRINTF("| (0x%02X)�浵�ļ�", FileInf.fattrib);
				break;
			default:
				FATFS_DEBUG_PRINTF("| (0x%02X)δ֪����", FileInf.fattrib);
				break;
		}

        /* ��ӡ�ļ���С, ���4G */
        FATFS_DEBUG_PRINTF(" |%10d ", FileInf.fsize);

        FATFS_DEBUG_PRINTF(" | %s\r\n", (char *)FileInf.fname);    /* ���ļ��� */
    }
	FATFS_DEBUG_PRINTF("\r\n\n");
}


/*---------------------------- END OF FILE ----------------------------*/


