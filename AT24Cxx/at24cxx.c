#include "at24cxx.h"
#include "bsp_uart.h"


/* ʹ�� 16λ��ַ����ú�,����Ĭ�� 8λ��ַ */
//#define AT24CXX_16BIT_ADDR

#define MAX_TIME_OUT		((uint32_t)0x1000)
#define MAX_LONGTIME_OUT	((uint32_t)(10 * MAX_TIME_OUT))

static __IO uint32_t AT24C_TimeOut = MAX_TIME_OUT;

uint16_t AT24Cxx_Addr = 0;

/************************************************
�������� �� TimeOut_Callback
��    �� �� �ȴ���ʱ�ص�����
��    �� �� ErrorCode ---- �������
�� �� ֵ �� ����ֵ 0
*************************************************/
static uint8_t TimeOut_Callback( char ErrorCode )
{
    /* �ȴ���ʱ��Ĵ���,���������Ϣ */
    AT24C_DUBUG_PRINTF("SPI �ȴ���ʱ!  EerrorCode = %d\n",ErrorCode);

    return 0;
}

/************************************************
�������� �� AT24Cxx_Busy_Wait
��    �� �� AT24Cxxæ�ȴ�
��    �� �� ��
�� �� ֵ �� 0 / 1
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
�������� �� AT24Cxx_Page_Program
��    �� �� AT24Cxxҳ���
��    �� �� pBuffer ---- ����
			Address ---- ��ַ
			Len ---- ����
�� �� ֵ �� 0 / 1
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
�������� �� AT24Cxx_Write_EEPROM
��    �� �� �� AT24Cxx��д�����ݿ�
��    �� �� pBuffer ---- ����
			Address ---- ��ַ
			Len ---- ����
�� �� ֵ �� ��
*************************************************/
void AT24Cxx_Write_EEPROM( uint8_t *pBuffer, uint16_t Address, uint16_t Len )
{
    uint8_t NumOfPage = 0, NumOfSingle = 0;
    uint8_t Addr = 0, count = 0, temp = 0;

    /* mod�������࣬�� Address�� AT24C_PAGE_SIZE�������������� AddrֵΪ 0 */
    Addr = Address % AT24C_PAGE_SIZE;

    /* ��count������ֵ���պÿ��Զ��뵽ҳ��ַ */
    count = AT24C_PAGE_SIZE - Addr;

    /* �����Ҫд��������ҳ */
    NumOfPage =  Len / AT24C_PAGE_SIZE;

    /* �����ʣ�಻��һҳ���ֽ��� */
    NumOfSingle = Len % AT24C_PAGE_SIZE;

    /* Addr = 0,�� Address�պð�ҳ���� */
    if(Addr == 0)
    {
        /* Len <= AT24C_PAGE_SIZE */
        if(0 == NumOfPage)
        {
            /* ����һҳ or �պ�һҳ */
            AT24Cxx_Page_Program(pBuffer, Address, Len);
            AT24Cxx_Busy_Wait();
        }
        else /* Len > AT24C_PAGE_SIZE */
        {
            /* �Ȱ�����ҳ�Ķ�д�� */
            while(NumOfPage--)
            {
                AT24Cxx_Page_Program(pBuffer, Address, AT24C_PAGE_SIZE);
                AT24Cxx_Busy_Wait();
                Address += AT24C_PAGE_SIZE;
                pBuffer += AT24C_PAGE_SIZE;
            }
            /* ���ж���Ĳ���һҳ�����ݣ���һҳ����д�� */
            if(NumOfSingle != 0)
            {
                AT24Cxx_Page_Program(pBuffer, Address, NumOfSingle);
                AT24Cxx_Busy_Wait();
            }
        }
    }
    /* ����ַ�� AT24C_PAGE_SIZE������  */
    else
    {
        /* Len < AT24C_PAGE_SIZE */
        if(NumOfPage == 0)
        {
            /* ��ǰҳʣ��� count��λ�ñ� NumOfSingleС��һҳд���� */
            if(NumOfSingle > count)
            {
                /* ��д����ǰҳ */
                AT24Cxx_Page_Program(pBuffer, Address, count);
                AT24Cxx_Busy_Wait();

                temp = NumOfSingle - count;
                Address += count;
                pBuffer += count;
                /* ��дʣ������� */
                AT24Cxx_Page_Program(pBuffer, Address, temp);
                AT24Cxx_Busy_Wait();
            }
            else /* ��ǰҳʣ��� count��λ����д�� NumOfSingle������ */
            {
                AT24Cxx_Page_Program(pBuffer, Address, Len);
                AT24Cxx_Busy_Wait();
            }
        }
        else /* Len > AT24C_PAGE_SIZE */
        {
            /* ��ַ���������� count�ֿ������������������ */
            Len -= count;
            NumOfPage =  Len / AT24C_PAGE_SIZE;
            NumOfSingle = Len % AT24C_PAGE_SIZE;

            if(count != 0)
            {
                /* ��д��count�����ݣ�Ϊ��������һ��Ҫд�ĵ�ַ���� */
                AT24Cxx_Page_Program(pBuffer, Address, count);
                AT24Cxx_Busy_Wait();
                /* ���������ظ���ַ�������� */
                Address += count;
                pBuffer += count;
            }

            /* ������ҳ��д�� */
            while(NumOfPage--)
            {
                AT24Cxx_Page_Program(pBuffer, Address, AT24C_PAGE_SIZE);
                AT24Cxx_Busy_Wait();
                Address +=  AT24C_PAGE_SIZE;
                pBuffer += AT24C_PAGE_SIZE;
            }
            /* ���ж���Ĳ���һҳ�����ݣ�����д��*/
            if(NumOfSingle != 0)
            {
                AT24Cxx_Page_Program(pBuffer, Address, NumOfSingle);
                AT24Cxx_Busy_Wait();
            }
        }
    }
}

/************************************************
�������� �� AT24Cxx_Read_EEPROM
��    �� �� �� AT24Cxx�ж�ȡ���ݿ�
��    �� �� pBuffer ---- ����
			Address ---- ��ַ
			Len ---- ����
�� �� ֵ �� 0 / 1
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
�������� �� AT24Cxx_Config
��    �� �� AT24Cxx����
��    �� �� ��
�� �� ֵ �� ��
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
�������� �� AT24Cxx_Init
��    �� �� AT24Cxx��ʼ��
��    �� �� ��
�� �� ֵ �� ��
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


