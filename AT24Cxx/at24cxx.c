#include "at24cxx.h"
#include "bsp_uart.h"


/* 使用 16位地址则定义该宏,否者默认 8位地址 */
//#define AT24CXX_16BIT_ADDR

#define MAX_TIME_OUT		((uint32_t)0x1000)
#define MAX_LONGTIME_OUT	((uint32_t)(10 * MAX_TIME_OUT))

static __IO uint32_t AT24C_TimeOut = MAX_TIME_OUT;

uint16_t AT24Cxx_Addr = 0;

/************************************************
函数名称 ： TimeOut_Callback
功    能 ： 等待超时回调函数
参    数 ： ErrorCode ---- 错误代号
返 回 值 ： 错误值 0
*************************************************/
static uint8_t TimeOut_Callback( char ErrorCode )
{
    /* 等待超时后的处理,输出错误信息 */
    AT24C_DUBUG_PRINTF("SPI 等待超时!  EerrorCode = %d\n",ErrorCode);

    return 0;
}

/************************************************
函数名称 ： AT24Cxx_Busy_Wait
功    能 ： AT24Cxx忙等待
参    数 ： 无
返 回 值 ： 0 / 1
*************************************************/
static uint8_t AT24Cxx_Busy_Wait(void)
{
	uint8_t temp = 1;
    __IO uint16_t temp_SR1;

    /*!< While the bus is busy */
    AT24C_TimeOut = MAX_LONGTIME_OUT;
    while(I2C_GetFlagStatus(AT24C_I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(0);
    }

//    /* Send START condition */
//    I2C_GenerateSTART(AT24C_I2Cx, ENABLE);

//    /* Test on EV5 and clear it */
//    AT24C_TimeOut = MAX_TIME_OUT;
//    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
//    {
//        if(0 == (AT24C_TimeOut--))
//            return TimeOut_Callback(1);
//    }

//    /*!< Send EEPROM address for write */
//    I2C_Send7bitAddress(AT24C_I2Cx, AT24Cxx_Addr, I2C_Direction_Transmitter);

    /* Wait for ADDR flag to be set (Slave acknowledged his address) */
    AT24C_TimeOut = MAX_LONGTIME_OUT;
    do
    {
		I2C_GenerateSTART(AT24C_I2Cx, ENABLE);
		I2C_Send7bitAddress(AT24C_I2Cx, AT24Cxx_Addr, I2C_Direction_Transmitter);
		
        temp_SR1 = I2C_ReadRegister(AT24C_I2Cx, I2C_Register_SR1);		
        if(0 == (AT24C_TimeOut--))
        {
            temp = TimeOut_Callback(2);
            break;
        }
    }while(!(I2C_ReadRegister(AT24C_I2Cx, I2C_Register_SR1) & 0x0002));

    /* Clear AF flag */
    I2C_ClearFlag(AT24C_I2Cx, I2C_FLAG_AF);

    /* STOP condition */
    I2C_GenerateSTOP(AT24C_I2Cx, ENABLE);
	
	return temp;
}

/************************************************
函数名称 ： AT24Cxx_Page_Program
功    能 ： AT24Cxx页编程
参    数 ： pBuffer ---- 数据
			Address ---- 地址
			Len ---- 长度
返 回 值 ： 0 / 1
*************************************************/
uint8_t AT24Cxx_Page_Program( uint8_t *pBuffer, uint16_t Address, uint16_t Len )
{
    /* While the bus is busy */
    AT24C_TimeOut = MAX_LONGTIME_OUT;
    while(I2C_GetFlagStatus(AT24C_I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(4);
    }

    /* Send START condition */
    I2C_GenerateSTART(AT24C_I2Cx, ENABLE);

    /* Test on EV5 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(5);
    }

    /* Send EEPROM address for write */
    AT24C_TimeOut = MAX_TIME_OUT;
    I2C_Send7bitAddress(AT24C_I2Cx, AT24Cxx_Addr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(6);
    }

#ifndef AT24CXX_16BIT_ADDR

    /* Send the EEPROM's internal address to write to : only one byte Address */
    I2C_SendData(AT24C_I2Cx, (uint8_t)(Address & 0x00FF));

#else

    /* Send the EEPROM's internal address to write to : MSB of the address first */
    I2C_SendData(AT24C_I2Cx, (uint8_t)((Address & 0xFF00) >> 8));

    /* Test on EV8 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(6.5);
    }

    /*!< Send the EEPROM's internal address to write to : LSB of the address */
    I2C_SendData(AT24C_I2Cx, (uint8_t)(Address & 0x00FF));

#endif /* AT24CXX_16BIT_ADDR */

    /* Test on EV8 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(7);
    }

    /* While there is data to be written */
    while(Len--)
    {
        /* Send the current byte */
        I2C_SendData(AT24C_I2Cx, *pBuffer);

        /* Point to the next byte to be written */
        pBuffer++;

        /* Test on EV8 and clear it */
        AT24C_TimeOut = MAX_TIME_OUT;
        while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if(0 == (AT24C_TimeOut--))
                return TimeOut_Callback(8);
        }
    }


    /* Send STOP condition */
    I2C_GenerateSTOP(AT24C_I2Cx, ENABLE);

    /* If all operations OK, return 1 */
    return 1;
}

/************************************************
函数名称 ： AT24Cxx_Write_EEPROM
功    能 ： 从 AT24Cxx中写入数据块
参    数 ： pBuffer ---- 数据
			Address ---- 地址
			Len ---- 长度
返 回 值 ： 无
*************************************************/
void AT24Cxx_Write_EEPROM( uint8_t *pBuffer, uint16_t Address, uint16_t Len )
{
    uint8_t NumOfPage = 0, NumOfSingle = 0;
    uint8_t Addr = 0, count = 0, temp = 0;

    /* mod运算求余，若 Address是 AT24C_PAGE_SIZE整数倍，运算结果 Addr值为 0 */
    Addr = Address % AT24C_PAGE_SIZE;

    /* 差count个数据值，刚好可以对齐到页地址 */
    count = AT24C_PAGE_SIZE - Addr;

    /* 计算出要写多少整数页 */
    NumOfPage =  Len / AT24C_PAGE_SIZE;

    /* 计算出剩余不满一页的字节数 */
    NumOfSingle = Len % AT24C_PAGE_SIZE;

    /* Addr = 0,则 Address刚好按页对齐 */
    if(Addr == 0)
    {
        /* Len <= AT24C_PAGE_SIZE */
        if(0 == NumOfPage)
        {
            /* 不到一页 or 刚好一页 */
            AT24Cxx_Page_Program(pBuffer, Address, Len);
            AT24Cxx_Busy_Wait();
        }
        else /* Len > AT24C_PAGE_SIZE */
        {
            /* 先把整数页的都写了 */
            while(NumOfPage--)
            {
                AT24Cxx_Page_Program(pBuffer, Address, AT24C_PAGE_SIZE);
                AT24Cxx_Busy_Wait();
                Address += AT24C_PAGE_SIZE;
                pBuffer += AT24C_PAGE_SIZE;
            }
            /* 若有多余的不满一页的数据，下一页把它写完 */
            if(NumOfSingle != 0)
            {
                AT24Cxx_Page_Program(pBuffer, Address, NumOfSingle);
                AT24Cxx_Busy_Wait();
            }
        }
    }
    /* 若地址与 AT24C_PAGE_SIZE不对齐  */
    else
    {
        /* Len < AT24C_PAGE_SIZE */
        if(NumOfPage == 0)
        {
            /* 当前页剩余的 count个位置比 NumOfSingle小，一页写不完 */
            if(NumOfSingle > count)
            {
                /* 先写满当前页 */
                AT24Cxx_Page_Program(pBuffer, Address, count);
                AT24Cxx_Busy_Wait();

                temp = NumOfSingle - count;
                Address += count;
                pBuffer += count;
                /* 再写剩余的数据 */
                AT24Cxx_Page_Program(pBuffer, Address, temp);
                AT24Cxx_Busy_Wait();
            }
            else /* 当前页剩余的 count个位置能写完 NumOfSingle个数据 */
            {
                AT24Cxx_Page_Program(pBuffer, Address, Len);
                AT24Cxx_Busy_Wait();
            }
        }
        else /* Len > AT24C_PAGE_SIZE */
        {
            /* 地址不对齐多出的 count分开处理，不加入这个运算 */
            Len -= count;
            NumOfPage =  Len / AT24C_PAGE_SIZE;
            NumOfSingle = Len % AT24C_PAGE_SIZE;

            if(count != 0)
            {
                /* 先写完count个数据，为的是让下一次要写的地址对齐 */
                AT24Cxx_Page_Program(pBuffer, Address, count);
                AT24Cxx_Busy_Wait();
                /* 接下来就重复地址对齐的情况 */
                Address += count;
                pBuffer += count;
            }

            /* 把整数页都写了 */
            while(NumOfPage--)
            {
                AT24Cxx_Page_Program(pBuffer, Address, AT24C_PAGE_SIZE);
                AT24Cxx_Busy_Wait();
                Address +=  AT24C_PAGE_SIZE;
                pBuffer += AT24C_PAGE_SIZE;
            }
            /* 若有多余的不满一页的数据，把它写完*/
            if(NumOfSingle != 0)
            {
                AT24Cxx_Page_Program(pBuffer, Address, NumOfSingle);
                AT24Cxx_Busy_Wait();
            }
        }
    }
}

/************************************************
函数名称 ： AT24Cxx_Read_EEPROM
功    能 ： 从 AT24Cxx中读取数据块
参    数 ： pBuffer ---- 数据
			Address ---- 地址
			Len ---- 长度
返 回 值 ： 0 / 1
*************************************************/
uint8_t AT24Cxx_Read_EEPROM( uint8_t *pBuffer, uint32_t Address, uint16_t Len )
{
    /*!< While the bus is busy */
    AT24C_TimeOut = MAX_LONGTIME_OUT;
    while(I2C_GetFlagStatus(AT24C_I2Cx, I2C_FLAG_BUSY))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(9);
    }

    /*!< Send START condition */
    I2C_GenerateSTART(AT24C_I2Cx, ENABLE);

    /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(10);
    }

    /*!< Send EEPROM address for write */
    I2C_Send7bitAddress(AT24C_I2Cx, AT24Cxx_Addr, I2C_Direction_Transmitter);

    /*!< Test on EV6 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(11);
    }

    /* Clear EV6 by setting again the PE bit */
    I2C_Cmd(AT24C_I2Cx, ENABLE);

#ifndef AT24CXX_16BIT_ADDR

    /*!< Send the EEPROM's internal address to read from: Only one byte address */
    I2C_SendData(AT24C_I2Cx, (uint8_t)(Address & 0x00FF));

#else

    /*!< Send the EEPROM's internal address to read from: MSB of the address first */
    I2C_SendData(AT24C_I2Cx, (uint8_t)((Address & 0xFF00) >> 8));

    /*!< Test on EV8 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(11.5);
    }

    /*!< Send the EEPROM's internal address to read from: LSB of the address */
    I2C_SendData(AT24C_I2Cx, (uint8_t)(ReadAddr & 0x00FF));

#endif /*!< AT24CXX_16BIT_ADDR */

    /*!< Test on EV8 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(I2C_GetFlagStatus(AT24C_I2Cx, I2C_FLAG_BTF) == RESET)
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(12);
    }

    /*!< Send STRAT condition a second time */
    I2C_GenerateSTART(AT24C_I2Cx, ENABLE);

    /*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(13);
    }

    /*!< Send EEPROM address for read */
    I2C_Send7bitAddress(AT24C_I2Cx, AT24Cxx_Addr, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    AT24C_TimeOut = MAX_TIME_OUT;
    while(!I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(0 == (AT24C_TimeOut--))
            return TimeOut_Callback(14);
    }

    /* While there is data to be read */
    while(Len)
    {
        if(Len == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(AT24C_I2Cx, DISABLE);

            /* Send STOP Condition */
            I2C_GenerateSTOP(AT24C_I2Cx, ENABLE);
        }

        /* Test on EV7 and clear it */
        AT24C_TimeOut = MAX_LONGTIME_OUT;

        while(I2C_CheckEvent(AT24C_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)
        {
            if(0 == (AT24C_TimeOut--))
                return TimeOut_Callback(15);
        }
        {
            /* Read a byte from the EEPROM */
            *pBuffer = I2C_ReceiveData(AT24C_I2Cx);

            /* Point to the next location where the byte read will be saved */
            pBuffer++;

            /* Decrement the read bytes counter */
            Len--;
        }
    }

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(AT24C_I2Cx, ENABLE);

    /* If all operations OK, return 1 */
    return 1;
}

/************************************************
函数名称 ： AT24Cxx_Config
功    能 ： AT24Cxx配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void AT24Cxx_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    /* AT24C_I2Cx IO Periph clock enable */
    AT24C_IO_APBxClock_FUN(AT24C_SCL_CLK | AT24C_SDA_CLK, ENABLE);

    /* AT24C_I2Cx Periph clock enable */
    AT24C_I2C_APBxClock_FUN(AT24C_I2C_CLK, ENABLE);

    /* Configure AT24C_I2Cx pins: SCL, SDA */
    /* Confugure SCL and SDA pins as Alternate Function Open Drain Output */
    GPIO_InitStructure.GPIO_Pin = AT24C_SCL_PINS;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(AT24C_SCL_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = AT24C_SDA_PINS;
    GPIO_Init(AT24C_SDA_PORT, &GPIO_InitStructure);

    /* AT24C_I2Cx configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
    I2C_Init(AT24C_I2Cx, &I2C_InitStructure);

    /* Enable AT24C_I2Cx */
    I2C_Cmd(AT24C_I2Cx, ENABLE);
}

/************************************************
函数名称 ： AT24Cxx_Init
功    能 ： AT24Cxx初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void AT24Cxx_Init(void)
{
    AT24Cxx_Config();

    /* depending on the EEPROM Address selected in the at24cxx.h file */
#ifdef EEPROM_Block0_ADDRESS
    /*!< Select the EEPROM Block0 to write on */
    AT24Cxx_Addr = EEPROM_Block0_ADDRESS;
#endif

#ifdef sEE_Block1_ADDRESS
    /*!< Select the EEPROM Block1 to write on */
    AT24Cxx_Addr = EEPROM_Block1_ADDRESS;
#endif

#ifdef sEE_Block2_ADDRESS
    /*!< Select the EEPROM Block2 to write on */
    AT24Cxx_Addr = EEPROM_Block2_ADDRESS;
#endif

#ifdef sEE_Block3_ADDRESS
    /*!< Select the EEPROM Block3 to write on */
    AT24Cxx_Addr = EEPROM_Block3_ADDRESS;
#endif
}


/*---------------------------- END OF FILE ----------------------------*/


