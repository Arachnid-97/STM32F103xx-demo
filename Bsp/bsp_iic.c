#include "bsp_iic.h"


/* ע��STM32��Ӳ�� I2C��ȷ��һЩ BUG������˵ʹ��ʱ�кܶ಻���������Ҫ��	\
       ���� DMA������жϣ���ʹ�ø�ģʽ���׳��������� */

#define WAIT_TIME		25

/************************************************
�������� �� IIC_Delay_us
��    �� �� ���������ʱ
��    �� �� Count ---- ����
�� �� ֵ �� ��
*************************************************/
static void IIC_Delay_us( uint32_t Count )
{
    while(Count)
    {
        Count--;
    }
}

/************************************************
�������� �� IIC_Start
��    �� �� IICд����
��    �� �� ��
�� �� ֵ �� 0 / 1
*************************************************/
static uint8_t IIC_Start(void)
{
	IIC_SCL(HIGH);
	IIC_SDA(HIGH);
    IIC_Delay_us(WAIT_TIME);

    if(!IIC_SDA_READ)              // �ؼ�һ�ε�ƽ״̬
    {
        return 0;
    }
	IIC_SDA(LOW);
    IIC_Delay_us(WAIT_TIME);

    if(IIC_SDA_READ)               // �ؼ�һ�ε�ƽ״̬
    {
        return 0;
    }
	IIC_SCL(LOW);

    return 1;
}

/************************************************
�������� �� IIC_Stop
��    �� �� IICдֹͣ
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
static void IIC_Stop(void)
{
	IIC_SCL(LOW);
	IIC_SDA(LOW);
    IIC_Delay_us(WAIT_TIME);

    IIC_SCL(HIGH);
    IIC_Delay_us(WAIT_TIME);

	IIC_SDA(HIGH);
}

/************************************************
�������� �� IIC_Ack
��    �� �� AckӦ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
static void IIC_Ack(void)
{
	IIC_SDA(LOW);
    IIC_Delay_us(WAIT_TIME);

    IIC_SCL(HIGH);
    IIC_Delay_us(WAIT_TIME);

	IIC_SCL(LOW);
	IIC_Delay_us(WAIT_TIME);
	
    IIC_SDA(HIGH);
}

/************************************************
�������� �� IIC_UnAck
��    �� �� Ack��Ӧ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
static void IIC_UnAck(void)
{
	IIC_SCL(LOW);
	IIC_SDA(HIGH);
    IIC_Delay_us(WAIT_TIME);

    IIC_SCL(HIGH);
    IIC_Delay_us(WAIT_TIME);

	IIC_SCL(LOW);
}

/************************************************
�������� �� IIC_Wait_Ack
��    �� �� ACK�ȴ�
��    �� �� ��
�� �� ֵ �� 0 / 1
*************************************************/
static uint8_t IIC_Wait_Ack(void)
{
    uint8_t time = 80;

    IIC_SCL(LOW);
    IIC_Delay_us(WAIT_TIME);

    IIC_SDA(HIGH);
    IIC_Delay_us(WAIT_TIME);

	IIC_SCL(HIGH);
    IIC_Delay_us(WAIT_TIME);

    while(IIC_SDA_READ)
    {
        time--;
        if(!time)
        {
            IIC_Stop();
            return 0;
        }
    }
	IIC_SCL(LOW);

    return 1;
}

/************************************************
�������� �� Write_IIC_Byte
��    �� �� IICдһ���ֽ�
��    �� �� Byte ---- ����
�� �� ֵ �� ��
*************************************************/
void Write_IIC_Byte( uint8_t Byte )
{
	uint8_t i;

	IIC_SCL(LOW);
	IIC_Delay_us(WAIT_TIME);

	for(i = 0;i < 8;i++)
	{
        IIC_SDA((BitAction)((Byte & 0x80) >> 7));
        Byte <<= 1;
        IIC_Delay_us(WAIT_TIME);

		IIC_SCL(HIGH);
        IIC_Delay_us(WAIT_TIME);

		IIC_SCL(LOW);
        IIC_Delay_us(WAIT_TIME);
    }
}

/************************************************
�������� �� Read_IIC_Byte
��    �� �� IIC��һ���ֽ�
��    �� �� ��
�� �� ֵ �� temp ---- ����
*************************************************/
uint8_t Read_IIC_Byte(void)
{
	uint8_t i;
	uint8_t temp = 0;

	IIC_SDA(HIGH);
	IIC_Delay_us(WAIT_TIME);

	for(i = 0;i < 8;i++)
	{
		temp <<= 1;

		IIC_SCL(LOW);
		IIC_Delay_us(WAIT_TIME);

		IIC_SCL(HIGH);
        IIC_Delay_us(WAIT_TIME);

//		if(IIC_SDA_READ)
//		{
//			temp++;
//		}		
		temp |= IIC_SDA_READ;
    }
	IIC_SCL(LOW);
	IIC_Delay_us(WAIT_TIME);
	
	return temp;
}


/*---------------------------- END OF FILE ----------------------------*/


