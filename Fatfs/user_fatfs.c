#include "user_fatfs.h"
#include "bsp_uart.h"


FATFS FatFs;				/* 每个逻辑驱动器的文件系统对象 */
FIL File;					/* 文件对象 */
FRESULT res_sd;				/* FatFs 函数公共结果代码 */
UINT br, bw;				/* 文件读 /写字节计数 */

__attribute__ ((aligned (4)))  \
BYTE FF_Buff[FF_MAX_SS] = "这是一个测试数据\r\n";	/* Working buffer */
    
/************************************************
函数名称 ： FF_System_Creates
功    能 ： Fatfs文件系统注册
参    数 ： Drive ---- 盘符
			Opt ---- 0：现在不要安装（在第一次访问该卷时安装）
					 1：强制安装该卷以检查它是否可以工作
返 回 值 ： 无
*************************************************/
void FF_Test(void)
{
	FF_System_Creates("1:", 1);
	FF_OpenWrite("1:temp.txt", FF_Buff, 21);
	FF_OpenRead("1:temp.txt", &FF_Buff[1024], 21);
	
	
    /* 不再使用文件系统，取消挂载文件系统 */
    f_mount(NULL,"1:", 1);
}

/************************************************
函数名称 ： FF_System_Creates
功    能 ： Fatfs文件系统注册
参    数 ： Drive ---- 盘符
			Opt ---- 0：现在不要安装（在第一次访问该卷时安装）
					 1：强制安装该卷以检查它是否可以工作
返 回 值 ： 无
*************************************************/
void FF_System_Creates( char *pDrive, uint8_t Opt )
{
	/* 为逻辑驱动器工作区注册 */
    res_sd = f_mount(&FatFs, pDrive, Opt);

	if(1 == Opt)
	{
		/* 如果没有文件系统就格式化创建文件系统 */
		if(res_sd == FR_NO_FILESYSTEM)
		{
			DEBUG_PRINTF("SD卡还没有文件系统，即将进行格式化...\r\n");
			
			res_sd = f_mkfs(pDrive, 0, FF_Buff, sizeof(FF_Buff));	// 格式化

			if(res_sd == FR_OK)
			{
				DEBUG_PRINTF("SD卡已成功格式化文件系统。\r\n");
				
				res_sd = f_mount(NULL, pDrive, 1);		// 格式化后，先取消挂载
				
				res_sd = f_mount(&FatFs, pDrive, 1);		// 重新挂载
			}
			else
			{
				DEBUG_PRINTF("格式化失败。\r\n");
				while(1);
			}
		}
		else if(res_sd != FR_OK)
		{
			DEBUG_PRINTF("！！SD卡挂载安装文件系统失败。(error code:%d)\r\n",res_sd);
			DEBUG_PRINTF("！！可能原因：SD卡初始化不成功。\r\n");
			while(1);
		}
		else
		{
			DEBUG_PRINTF("文件系统挂载安装成功，可以进行文件读写操作\r\n");
		}
	}
	else
	{
		printf("挂载磁盘完成，但并未安装。\r\n");
	}
}

/************************************************
函数名称 ： FF_OpenWrite
功    能 ： 打开文件并写入信息
参    数 ： pFile ---- 需要打开的文件
			pStr ---- 需要写入的信息
			Len ---- 长度
返 回 值 ： 0 / 1
*************************************************/
uint8_t FF_OpenWrite( char *pFile, void *pStr, uint16_t Len )
{
	uint8_t temp = 0;
	
    res_sd = f_open(&File, pFile, FA_CREATE_ALWAYS | FA_WRITE );

    if( res_sd == FR_OK )
    {
        DEBUG_PRINTF("打开文件成功。\r\n");
        /* 将指定存储区内容写入到文件内 */
        res_sd = f_write(&File, pStr, Len, &bw);
        if(res_sd == FR_OK)
        {
            DEBUG_PRINTF("文件写入成功，写入字节数据：%d\r\n", bw);
            DEBUG_PRINTF("向文件写入的数据为：%s\r\n", (char*)pStr);
			
			temp = 1;
        }
        else
        {
            DEBUG_PRINTF("！！文件写入失败。(error code:%d)\r\n", res_sd);
        }
        
        f_close(&File);		// 不再读写，关闭文件
    }
    else
    {
        DEBUG_PRINTF("！！打开/创建文件失败。\r\n");
    }
	
	return temp;
}

/************************************************
函数名称 ： FF_OpenRead
功    能 ： 打开文件并读取信息
参    数 ： pFile ---- 需要打开的文件
			pStr ---- 需要读取的信息
返 回 值 ： 0 / 1
*************************************************/
uint8_t FF_OpenRead( char *pFile, void *pStr, uint16_t Len )
{
	uint8_t temp = 0;
	
    res_sd = f_open(&File, pFile, FA_OPEN_EXISTING | FA_READ);
    if(res_sd == FR_OK)
    {
        DEBUG_PRINTF("打开文件成功。\r\n");
		/* 将文件内容读取到指定存储区内 */
        res_sd = f_read(&File, pStr, Len, &br);
        if(res_sd == FR_OK)
        {
            DEBUG_PRINTF("文件读取成功,读到字节数据：%d\r\n",br);
            DEBUG_PRINTF("读取得的文件数据为：%s\r\n", (char*)pStr);
			
			temp = 1;
        }
        else
        {
            DEBUG_PRINTF("！！文件读取失败。(error code:%d)\r\n",res_sd);
        }
    }
    else
    {
        DEBUG_PRINTF("！！打开文件失败。\r\n");
    }
    
    f_close(&File);		// 不再读写，关闭文件

	return temp;
}


/*---------------------------- END OF FILE ----------------------------*/


