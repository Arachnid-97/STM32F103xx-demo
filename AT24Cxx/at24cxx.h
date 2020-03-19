#ifndef __AT24CXX_H
#define __AT24CXX_H


#include "stm32f10x.h"

//#ifndef ENABLE
//#define ENABLE              1
//#endif /* ENABLE */

//#ifndef DISABLE
//#define DISABLE             0
//#endif /* DISABLE */

#ifndef HIGH
#define HIGH                1
#endif /* HIGH */

#ifndef LOW
#define LOW                 0
#endif /* LOW */

#define _AT24C_DUBUG		1
#define AT24C_DUBUG_PRINTF(fmt,arg...)		do{\
											if(_AT24C_DUBUG)\
												printf("<<-EEPROM-DEBUG->> < %s >[%d]\n"fmt"\n",__FILE__,__LINE__, ##arg);\
											}while(0)

#define I2C_SPEED               400000
#define I2C_SLAVE_ADDRESS7      0x0A

/* The M24C08W contains 4 blocks (128byte each) with the adresses below: E2 = 0 
   EEPROM Addresses defines */
#define EEPROM_Block0_ADDRESS     0xA0	/* E2 = 0 */ 
//#define EEPROM_Block1_ADDRESS     0xA2	/* E2 = 0 */  
//#define EEPROM_Block2_ADDRESS     0xA4	/* E2 = 0 */
//#define EEPROM_Block3_ADDRESS     0xA6	/* E2 = 0 */
 
/* AT24Cxx SPIx */
#define AT24C_I2Cx	        	I2C1
#define AT24C_I2C_CLK     		RCC_APB1Periph_I2C1
#define AT24C_I2C_APBxClock_FUN(x, y)	RCC_APB1PeriphClockCmd(x, y)

/* AT24Cxx GPIO */
#define AT24C_SCL_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define AT24C_SCL_CLK			RCC_APB2Periph_GPIOB
#define AT24C_SCL_PORT   		GPIOB
#define AT24C_SCL_PINS   		GPIO_Pin_6
#define AT24C_SDA_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)
#define AT24C_SDA_CLK			RCC_APB2Periph_GPIOB
#define AT24C_SDA_PORT   		GPIOB
#define AT24C_SDA_PINS   		GPIO_Pin_7

/* 如果 IO的桥接线一样就直接用下面的宏，否则分开配置 */
#define AT24C_IO_APBxClock_FUN(x, y)	RCC_APB2PeriphClockCmd(x, y)


/* AT24C01/02每页有8个字节 */
#define AT24C_PAGE_SIZE           8

/* AT24C04/08A/16A每页有16个字节 */
//#define AT24C_PAGE_SIZE           16	

uint8_t AT24Cxx_Page_Program( uint8_t *pBuffer, uint16_t Address, uint16_t Len );
void AT24Cxx_Write_EEPROM( uint8_t *pBuffer, uint16_t Address, uint16_t Len );
uint8_t AT24Cxx_Read_EEPROM( uint8_t *pBuffer, uint32_t Address, uint16_t Len );
void AT24Cxx_Config(void);
void AT24Cxx_Init(void);


#endif	/* __AT24CXX_H */


/*---------------------------- END OF FILE ----------------------------*/

