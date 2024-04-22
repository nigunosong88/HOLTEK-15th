#ifndef _IIC_H
#define _IIC_H
 
#include "ht32f5xxxx_01.h"
 /* AS7341 */
#define EEPROM_I2C_WR				0		/* 写控制bit */
#define EEPROM_I2C_RD				1		/* 读控制bit */
#define EEPROM_DEV_ADDR				0x39	/* 24xx02的设备地址 */
#define EEPROM_PAGE_SIZE		  	8		/* 24xx02的页面大小 */
#define EEPROM_SIZE				  	256		/* 24xx02总容量 */
 
/* I2C的SCL */
#define I2C_SCL_GPIO_ID				GPIO_PA					/* GPIO端口 */
#define I2C_SCL_PIN						GPIO_PIN_10				/* 连接到SCL时钟线的GPIO */
#define I2C_SCL_AFIO_MODE			AFIO_FUN_GPIO
#define I2C_SCL_PORT					HT_GPIOA
 
/* I2C的SDA */
#define I2C_SDA_GPIO_ID				GPIO_PA					/* GPIO端口 */
#define I2C_SDA_PIN						GPIO_PIN_5				/* 连接到SCL时钟线的GPIO */
#define I2C_SDA_AFIO_MODE			AFIO_FUN_GPIO
#define I2C_SDA_PORT					HT_GPIOA

#define I2C_SCL_1()						GPIO_SetOutBits(I2C_SCL_PORT, I2C_SCL_PIN)
#define I2C_SCL_0()						GPIO_ClearOutBits(I2C_SCL_PORT, I2C_SCL_PIN)
/* 馬達*/
#define HTCFG_OUTPUT_MOTOR1_ID                                (GPIO_PA)
#define HTCFG_OUTPUT_MOTOR2_ID                                (GPIO_PC)
#define HTCFG_OUTPUT_MOTOR1_CLK(CK)                           (CK.Bit.PA)
#define HTCFG_OUTPUT_MOTOR2_CLK(CK)                           (CK.Bit.PC)
#define HTCFG_MOTOR1                                          (HT_GPIOA)
#define HTCFG_MOTOR2                                          (HT_GPIOC)
#define HTCFG_OUTPUT_MOTOR1_AFIO_PIN                          (AFIO_PIN_0)
#define HTCFG_OUTPUT_MOTOR2_AFIO_PIN                          (AFIO_PIN_9)
#define HTCFG_OUTPUT_MOTOR1_GPIO_PIN                          (GPIO_PIN_0)
#define HTCFG_OUTPUT_MOTOR2_GPIO_PIN                          (GPIO_PIN_9)

#define MOTOR1_1()									GPIO_SetOutBits(HTCFG_MOTOR1, HTCFG_OUTPUT_MOTOR1_GPIO_PIN)
#define MOTOR1_0()									GPIO_ClearOutBits(HTCFG_MOTOR1, HTCFG_OUTPUT_MOTOR1_GPIO_PIN)
#define MOTOR2_1()									GPIO_SetOutBits(HTCFG_MOTOR2, HTCFG_OUTPUT_MOTOR2_GPIO_PIN)
#define MOTOR2_0()									GPIO_ClearOutBits(HTCFG_MOTOR2, HTCFG_OUTPUT_MOTOR2_GPIO_PIN)
	
#define I2C_SDA_1()									GPIO_SetOutBits(I2C_SDA_PORT, I2C_SDA_PIN)
#define I2C_SDA_0()									GPIO_ClearOutBits(I2C_SDA_PORT, I2C_SDA_PIN)
#define I2C_SDA_Direction_Input()		GPIO_DirectionConfig(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_DIR_IN)
#define I2C_SDA_InputConfig()				GPIO_InputConfig(I2C_SDA_PORT, I2C_SDA_PIN, ENABLE)
#define I2C_SDA_InputDisable()			GPIO_InputConfig(I2C_SDA_PORT, I2C_SDA_PIN, DISABLE)
#define I2C_SDA_Direction_Output()	GPIO_DirectionConfig(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_DIR_OUT)
#define I2C_SCL_Direction_Output()	GPIO_DirectionConfig(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_DIR_OUT)
#define I2C_SDA_READ()							GPIO_ReadInBit(I2C_SDA_PORT, I2C_SDA_PIN)
 
#define WAITMAX							1000	
#define DELAYTIME 					200//200
/* Catch HX711 */
#define Catch_HX711_DT_GPIO 			GPIO_PC
#define Catch_HX711_DT_HT 				HT_GPIOC
#define Catch_HX711_DT_AF_Pin 		AFIO_PIN_5
#define Catch_HX711_DT_GP_Pin 		GPIO_PIN_5
#define Catch_HX711_SCK_GPIO 			GPIO_PA
#define Catch_HX711_SCK_HT 				HT_GPIOA
#define Catch_HX711_SCK_AF_Pin		AFIO_PIN_1
#define Catch_HX711_SCK_GP_Pin 		GPIO_PIN_1

/* Weight HX711 */
#define Weight_HX711_DT_GPIO 			GPIO_PA
#define Weight_HX711_DT_HT 				HT_GPIOA
#define Weight_HX711_DT_AF_Pin 		AFIO_PIN_3
#define Weight_HX711_DT_GP_Pin 		GPIO_PIN_3
#define Weight_HX711_SCK_GPIO			GPIO_PA
#define Weight_HX711_SCK_HT 			HT_GPIOA
#define Weight_HX711_SCK_AF_Pin		AFIO_PIN_2
#define Weight_HX711_SCK_GP_Pin 	GPIO_PIN_2

/* 復歸-微動開關 */
#define Reset_Btn_GPIO 			GPIO_PA
#define Reset_Btn_HT 				HT_GPIOA
#define Reset_Btn_AF_Pin 		AFIO_PIN_7
#define Reset_Btn_GP_Pin 		GPIO_PIN_7

/* Private function I2C -----------------------------------------------------------------------------*/
 
void I2C_Initia(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendByte(u8 _ucByte);
void I2C_Ack(void);
void I2C_NAck(void);
void I2Cmain();
void loop(void);
void setup_F1F4_Clear_NIR(void);
void setup_F5F8_Clear_NIR(void);
void readAllChannels(void);
void setSMUXLowChannels(bool);
uint16_t loopreadAllChannels(uint16_t *);
u8 IIC_Read_Byte_(u8);
u8 I2C_WaitAck(void);
u8 EE_CheckOK(void);
u8 IIC_Write_Byte(u8,u8);
u8 I2C_ReadByte(void);
u8 I2C_CheckDevice(u8 _Address);

uint16_t readChannels[6];
uint16_t readTwoRegister1(u8);
__ALIGN4 u8 gWriteBuf[8];
__ALIGN4 u8 gReadBuf[8];
 
u8 EE_CheckOK(void);
u8 EE_WriteBytes(u8 _pWriteBuf, u16 _usAddress, u16 _usSize);
u8 EE_ReadBytes(u8 *_pReadBuf, u16 _usAddress, u16 _usSize);


/* Private function HX711 -----------------------------------------------------------------------------*/
void HX711();
void CKCU_Configuration(void);
void GPIO_IN_Configuration(void);
void GPIO_OUT_Configuration(void);
void GPIO_MainRoutine(void);
void HX711SetGain(void);
void HX711WaitReady(long delay_ms);
bool HX711Is_Ready(void);
int GHV (int, int);
int GSD (int);
int GHD (int[]);
void FruitReset (void);
int ABS (int);
u64 gain = 128;
u64 GAIN = 1;
u32 HX711Is_Read(void);
u8 shiftInSlow(void);
FlagStatus TmpStatus = RESET;
#define SHIFTIN_WITH_SPEED_SUPPORT(void) shiftInSlow(void)
int weightBefore = 0;
int scale = 100;						//	穩定應變規數據參數
int coefficient = 0;			//	應變規數據歸零參數
double multiple = 4.4;					//	數據與歸零後參數差距

/* 馬達 functions ----------------------------------------------------------------------------------------*/
void motormain();
void GPIO_OUT_motor(void);
void display();
/* Private function LCD -----------------------------------------------------------------------------*/
void LCDmain();
void charLCD(void);
int CHS(int[],int[],int[], int);
u32 hx711 = 0;
u32 catchVal = 0;
int i = 0;
int j = 0;
char weight[14] = {'W', 'e', 'i', 'g', 'h', 't', ':', '0', '0', '0', 'g', ' ', ' ', ' '};
char weight2[3];
int HX711Count = 0;
#endif 