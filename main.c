/*********************************************************************************************************//**
 * @file    ADC/OneShot_PWMTrigger/main.c
 * @version $Rev:: 4845         $
 * @date    $Date:: 2020-07-29 #$
 * @brief   Main program.
 *************************************************************************************************************
 * @attention
 *
 * Firmware Disclaimer Information
 *
 * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
 *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
 *    other intellectual property laws.
 *
 * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
 *    other than HOLTEK and the customer.
 *
 * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
 *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
 *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
 *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
 *
 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 ************************************************************************************************************/

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "gpio.h"
#include "systick.h"
#include "picture.h"
/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup ADC_Examples ADC
  * @{
  */

/** @addtogroup OneShot_PWMTrigger
  * @{
  */

/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
int main(void){
  HX711();
	motormain();
	I2Cmain();
	display();
}
void display(){
	weightBefore = ABS(GHV(2, 11));
	weightBefore = weightBefore / scale;				//	穩定應變規數據(將低位數浮動值銷除)
	coefficient = weightBefore;	//	應變規數據歸零
	while (1) {
		LCDmain();
		GPIO_MainRoutine();
		Systick_Delay_ms(100);
	}
}
void motormain(){
	CKCU_Configuration(); 
	GPIO_OUT_motor();
}
void LCDmain(){
	 /* Initialize LCD related peripheral                          */
  LCD_Init();

  /* LCD driver configuration                                    */
  LCD_Config();

  /* Set back color as black                                     */
  LCD_BackColorSet(Black);

  /* Set text color as yellow                                    */
  LCD_TextColorSet(Yellow);
}
void HX711(){
	CKCU_Configuration();                                               
  GPIO_IN_Configuration();                                                  
  GPIO_OUT_Configuration();
  RETARGET_Configuration();
}
void I2Cmain(){
	u8 a=0,b=0;
		RETARGET_Configuration();      	
		I2C_Initia();	
		EE_CheckOK();
		IIC_Write_Byte(0x81,0x64); //ATIME
		IIC_Write_Byte(0xCA,0xE7); //7:0 ASTEP
		IIC_Write_Byte(0xCB,0x03); //15:8 ASTEP
		IIC_Write_Byte(0xAA,0x09); //CFG1
		IIC_Write_Byte(0x80,0x01);
		IIC_Write_Byte(0x70,0x00);
		IIC_Write_Byte(0x74,0x88);
		IIC_Write_Byte(0xA9,0x10);
}
/*********************************************************************************************************//**
  * @brief  TM configuration.
  * @retval None
  ***********************************************************************************************************/

void loop(){
  readAllChannels();
  loopreadAllChannels(readChannels);
}

uint16_t readTwoRegister1(u8 addr){
	uint8_t readingL;
  uint16_t readingH;
  uint16_t reading =0;
	readingH=(IIC_Read_Byte_((addr+0x01)))<<8;
	readingL= IIC_Read_Byte_(addr);
	reading=readingL|readingH;
	return reading;
}
uint16_t loopreadAllChannels(uint16_t *readChannels){
  int i =0;
  u8 add=0x95;
  for(i=0;i<6;i++){//readTwoRegister1
    //readChannels[i]=IIC_Read_Byte_(add+0x01)<<8|IIC_Read_Byte_(add);
    readChannels[i]=readTwoRegister1(add);
    add=add+0x02;
  }
  return *readChannels;
}
/////////////////////////////
void readAllChannels() {
  
  setSMUXLowChannels(TRUE);
  Systick_Delay_us(200);
  while(!IIC_Read_Byte_(0xA3)>>6){
  printf("A3_notready");
    Systick_Delay_us(1);
  }
  setSMUXLowChannels(FALSE);
  Systick_Delay_us(200);
  while(!IIC_Read_Byte_(0xA3)>>6){
  printf("A3_notready");
    Systick_Delay_us(1);
  }
  
}

void setSMUXLowChannels(bool f1_f4) {
  IIC_Write_Byte(0x80,0x01); 
  IIC_Write_Byte(0xAF,0x10); 
  if (f1_f4) {
    setup_F1F4_Clear_NIR();
  } else {
    setup_F5F8_Clear_NIR();
  }
  IIC_Write_Byte(0x80,0x10); 
  IIC_Write_Byte(0x80,0x03);
  Systick_Delay_us(200);
  while(!IIC_Read_Byte_(0xA3)>>6){
  printf("A3_notready\r\n");
    Systick_Delay_us(1);
  }
}
/////
void setup_F1F4_Clear_NIR() {
  // SMUX Config for F1,F2,F3,F4,NIR,Clear
  IIC_Write_Byte(0x00,0x30); // F3 left set to ADC2
  IIC_Write_Byte(0x01,0x01); // F1 left set to ADC0
  IIC_Write_Byte(0x02,0x00); // Reserved or disabled
  IIC_Write_Byte(0x03,0x00); // F8 left disabled
  IIC_Write_Byte(0x04,0x00); // F6 left disabled
  IIC_Write_Byte(0x05,0x42); // F4 left connected to ADC3/f2 left connected to ADC1
  IIC_Write_Byte(0x06,0x00); // F5 left disbled
  IIC_Write_Byte(0x07,0x00); // F7 left disbled
  IIC_Write_Byte(0x08,0x50); // CLEAR connected to ADC4
  IIC_Write_Byte(0x09,0x00); // F5 right disabled
  IIC_Write_Byte(0x0A,0x00); // F7 right disabled
  IIC_Write_Byte(0x0B,0x00); // Reserved or disabled
  IIC_Write_Byte(0x0C,0x20); // F2 right connected to ADC1
  IIC_Write_Byte(0x0D,0x04); // F4 right connected to ADC3
  IIC_Write_Byte(0x0E,0x00); // F6/F8 right disabled
  IIC_Write_Byte(0x0F,0x30); // F3 right connected to AD2
  IIC_Write_Byte(0x10,0x01); // F1 right connected to AD0
  IIC_Write_Byte(0x11,0x50); // CLEAR right connected to AD4
  IIC_Write_Byte(0x12,0x00); // Reserved or disabled
  IIC_Write_Byte(0x13,0x06); // NIR connected to ADC5
}

/**
 * @brief Configure SMUX for sensors F5-8, Clear and NIR
 *
 */
void setup_F5F8_Clear_NIR() {
  // SMUX Config for F5,F6,F7,F8,NIR,Clear
  IIC_Write_Byte(0x00,0x00); // F3 left disable
  IIC_Write_Byte(0x01,0x00); // F1 left disable
  IIC_Write_Byte(0x02,0x00); // reserved/disable
  IIC_Write_Byte(0x03,0x40); // F8 left connected to ADC3
  IIC_Write_Byte(0x04,0x02); // F6 left connected to ADC1
  IIC_Write_Byte(0x05,0x00); // F4/ F2 disabled
  IIC_Write_Byte(0x06,0x10); // F5 left connected to ADC0
  IIC_Write_Byte(0x07,0x03); // F7 left connected to ADC2
  IIC_Write_Byte(0x08,0x50); // CLEAR Connected to ADC4
  IIC_Write_Byte(0x09,0x10); // F5 right connected to ADC0
  IIC_Write_Byte(0x0A,0x03); // F7 right connected to ADC2
  IIC_Write_Byte(0x0B,0x00); // Reserved or disabled
  IIC_Write_Byte(0x0C,0x00); // F2 right disabled
  IIC_Write_Byte(0x0D,0x00); // F4 right disabled
  IIC_Write_Byte(
      0x0E,
      0x24); // F8 right connected to ADC2/ F6 right connected to ADC1
  IIC_Write_Byte(0x0F,0x00); // F3 right disabled
  IIC_Write_Byte(0x10,0x00); // F1 right disabled
  IIC_Write_Byte(0x11,0x50); // CLEAR right connected to AD4
  IIC_Write_Byte(0x12,0x00); // Reserved or disabled
  IIC_Write_Byte(0x13,0x06); // NIR connected to ADC5
}
/*********************************************************************************************************//**
  * @brief  Read and write EEPROM to check the entire data.
  * @retval None
  ***********************************************************************************************************/

static void I2C_CKCU_Config()
{
	CKCU_PeripClockConfig_TypeDef CCLOCK;
	
	CCLOCK.Bit.PA     = 1;
	CCLOCK.Bit.AFIO  = 1;
	
	CKCU_PeripClockConfig(CCLOCK, ENABLE);
}
 
static void I2C_GPIO_Config()
{
	AFIO_GPxConfig(I2C_SCL_GPIO_ID, I2C_SCL_PIN, I2C_SCL_AFIO_MODE);
	AFIO_GPxConfig(I2C_SDA_GPIO_ID, I2C_SDA_PIN, I2C_SDA_AFIO_MODE);
	
	/* Configure the GPIO pin                                                                                 */
	GPIO_PullResistorConfig(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PR_DISABLE);
	GPIO_DriveConfig(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_DV_8MA);
	GPIO_DirectionConfig(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_DIR_OUT);
	
	GPIO_PullResistorConfig(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PR_DISABLE);
	GPIO_DriveConfig(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_DV_8MA);
	GPIO_DirectionConfig(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_DIR_OUT);
}
 
void I2C_Initia()
{
	I2C_CKCU_Config();
	I2C_GPIO_Config();
	
	/* 一定要先一遍停止 */
	I2C_Stop();
}
 
void I2C_Start()
{
	 I2C_SDA_Direction_Output();   //sda書出
	 I2C_SCL_0();  //SCL=1
   Systick_Delay_us(DELAYTIME);//10 
	 I2C_SDA_1();  //SDA =1
   Systick_Delay_us(DELAYTIME);
   I2C_SCL_1();  //SCL=1
   Systick_Delay_us(DELAYTIME);
   I2C_SDA_0();//SDA=0
   Systick_Delay_us(DELAYTIME);
   I2C_SCL_0();//SCL=0
}
 
void I2C_Stop()
{
	 I2C_SDA_Direction_Output();  //sda??出
   I2C_SDA_0();   
   Systick_Delay_us(DELAYTIME);
   I2C_SCL_1();
   Systick_Delay_us(DELAYTIME);
   I2C_SDA_1(); 
   Systick_Delay_us(DELAYTIME);          

}
 
u8 I2C_WaitAck()
{
	u8 ack;
	
	I2C_SDA_1();
	Systick_Delay_us(DELAYTIME);
	I2C_SCL_1();
	Systick_Delay_us(DELAYTIME);
	I2C_SDA_Direction_Input();
	Systick_Delay_us(DELAYTIME);
	if( I2C_SDA_READ()==1 ) {
		ack = 1;
		printf("沒收到資料\r\n");
	}
	else {
		ack = 0;
	}
	I2C_SCL_0();
	Systick_Delay_us(DELAYTIME);
	return ack;
}

void I2C_SendByte(u8 _ucByte)
{
u8 t;
u32 add=128,sum=0;	
    I2C_SDA_Direction_Output();   
	  Systick_Delay_us(DELAYTIME);   
    for(t=0;t<8;t++)
    {              
        if(_ucByte&0x80)   
          {I2C_SDA_1();
						sum+=add;
					//printf("1");
					}//SDA=1，? 1                
        else  
          {I2C_SDA_0();
					//printf("0");
					}//SDA=0，? 0 
        _ucByte<<=1;  
			 add/=2;
       Systick_Delay_us(DELAYTIME);   
       I2C_SCL_1(); 
       Systick_Delay_us(DELAYTIME); 
       I2C_SCL_0();
       Systick_Delay_us(DELAYTIME);
    }  
/*printf("%x",sum);
printf("\r\n");*/
}
u8 I2Cc_ReadByte(u8 ack)
{
	 u8 i,receive=0;
   I2C_SDA_Direction_Input();
	I2C_SDA_InputConfig();
    for(i=0;i<8;i++ )
		{
        /*I2C_SCL_0();
        Systick_Delay_us(5);*/
        I2C_SCL_1();
        Systick_Delay_us(5);
        receive<<=1;
        if(I2C_SDA_READ()){
					receive++;   
				}
				I2C_SCL_0();
        Systick_Delay_us(5); 
    }      
    if (!ack)
        I2C_NAck();//送nACK
    else
        I2C_Ack(); //送ACK   
		return receive;
}
 u8 IIC_Read_Byte_(u8 reg){
		u8 res;
    I2C_Start();
		I2C_SendByte((EEPROM_DEV_ADDR<<1)|EEPROM_I2C_WR);
		if(I2C_WaitAck()==0){
			I2C_SendByte(reg);
			if(I2C_WaitAck()==0){
				I2C_Start();
				I2C_SendByte((EEPROM_DEV_ADDR<<1)|EEPROM_I2C_RD);
				if(I2C_WaitAck()==0){
					res=I2Cc_ReadByte(0);
				}else{
					printf("讀取不成功err3\r\n");
				}
			}
			else{
			printf("讀取不成功err2\r\n");
			}
		}
		else{
			printf("讀取不成功err1\r\n");
		}
    I2C_Stop();
 return res;
}
// 返回值0表示正确，返回1表示未探到
u8 I2C_CheckDevice(u8 _Address)
{
//	u8 time = 0;
	u8 ucAck = 0;
	
	I2C_Start();
	I2C_SendByte(_Address<<1 | EEPROM_I2C_WR);
	ucAck =I2C_WaitAck();
	I2C_SDA_Direction_Output();
	I2C_Stop();
 
	return ucAck;
}
 
void I2C_Ack()
{
	 I2C_SCL_0();
   I2C_SDA_Direction_Output();
   I2C_SDA_0();
   Systick_Delay_us(5);
   I2C_SCL_1();;
   Systick_Delay_us(5);
   I2C_SCL_0();
}
 
void I2C_NAck()
{
	I2C_SCL_0();
   I2C_SDA_Direction_Output();
   I2C_SDA_1();
   Systick_Delay_us(5);
   I2C_SCL_1();; 
   Systick_Delay_us(5);
   I2C_SCL_0();

}
//1表示正常，0表示不正常
u8 EE_CheckOK()
{
	if( I2C_CheckDevice(0x49) == 0 ){
		printf("在線\r\n");
		return 1;
	}
	else{
		printf("離線\r\n");
		I2C_Stop();
		return 0;
	}
}
 

u8 IIC_Write_Byte(u8 reg,u8 data){
 I2C_Start(); 
 I2C_SendByte((0x39<<1)|0);
 if(I2C_WaitAck()==0){
		I2C_SendByte(reg);
		if(I2C_WaitAck()==0){
			I2C_SendByte(data);
			if(I2C_WaitAck()==0){
				I2C_Stop();
			}
		}
 }
 else{
		printf("寫入不成功err1\r\n");
		I2C_Stop();
 }
 }
void CKCU_Configuration(void)
{
#if 1
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.AFIO       = 1; 
	CKCUClock.Bit.PA      = 1;
	HTCFG_OUTPUT_MOTOR1_CLK(CKCUClock) = 1;
  HTCFG_OUTPUT_MOTOR2_CLK(CKCUClock)  = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
#endif
}

/*********************************************************************************************************//**
  * @brief  Configure the GPIO ports for input.
  * @retval None
  ***********************************************************************************************************/
void GPIO_IN_Configuration(void)
{
 	/* Catch HX711 */
  AFIO_GPxConfig(Catch_HX711_DT_GPIO, Catch_HX711_DT_AF_Pin, AFIO_FUN_GPIO);
  GPIO_DirectionConfig(Catch_HX711_DT_HT, Catch_HX711_DT_GP_Pin, GPIO_DIR_IN);
  GPIO_PullResistorConfig(Catch_HX711_DT_HT, Catch_HX711_DT_GP_Pin, GPIO_PR_DOWN);
  GPIO_InputConfig(Catch_HX711_DT_HT, Catch_HX711_DT_GP_Pin, ENABLE);
	/* Weight HX711 */
	AFIO_GPxConfig(Weight_HX711_DT_GPIO, Weight_HX711_DT_AF_Pin, AFIO_FUN_GPIO);
  GPIO_DirectionConfig(Weight_HX711_DT_HT, Weight_HX711_DT_GP_Pin, GPIO_DIR_IN);
  GPIO_PullResistorConfig(Weight_HX711_DT_HT, Weight_HX711_DT_GP_Pin, GPIO_PR_DOWN);
  GPIO_InputConfig(Weight_HX711_DT_HT, Weight_HX711_DT_GP_Pin, ENABLE);
	/* 復歸-微動開關 */
	AFIO_GPxConfig(Reset_Btn_GPIO, Reset_Btn_AF_Pin, AFIO_FUN_GPIO);
  GPIO_DirectionConfig(Reset_Btn_HT, Reset_Btn_GP_Pin, GPIO_DIR_IN);
  GPIO_PullResistorConfig(Reset_Btn_HT, Reset_Btn_GP_Pin, GPIO_PR_UP);
  GPIO_InputConfig(Reset_Btn_HT, Reset_Btn_GP_Pin, ENABLE);
}

/*********************************************************************************************************//**
  * @brief  Configure the GPIO ports for output.
  * @retval None
  ***********************************************************************************************************/
void GPIO_OUT_Configuration(void)
{
	/* Catch HX711 */
	AFIO_GPxConfig(Catch_HX711_SCK_GPIO, Catch_HX711_SCK_AF_Pin, AFIO_FUN_GPIO);
	GPIO_DirectionConfig(Catch_HX711_SCK_HT, Catch_HX711_SCK_GP_Pin, GPIO_DIR_OUT);
	/* Weight HX711 DT */
	AFIO_GPxConfig(Weight_HX711_SCK_GPIO, Weight_HX711_SCK_AF_Pin, AFIO_FUN_GPIO);
	GPIO_DirectionConfig(Weight_HX711_SCK_HT, Weight_HX711_SCK_GP_Pin, GPIO_DIR_OUT);
}

/*********************************************************************************************************//**
  * @brief  GPIO Main Routine.
  * @retval None
  ***********************************************************************************************************/
void GPIO_MainRoutine(void)
{
	FruitReset();
	/* 1. display Can Put Fruit  */
	LCD_StringLineDisplay(Line1, "Can Put Fruit");
	//HX711Count = 2;
	int putWeight = 0;				//	放置重量
	
	printf("===============================啟動檢測===============================\r\n");
	do {
		int catchBefore = GHV(1, 1);
		printf("硬度測試：%d\r\n", catchBefore);
		int weightAfter = ABS(GHV(2, 11));				//	取得應變規原始數據11次平均(重量)
		printf("應變規原始數據：%d\r\n",weightAfter);
		weightAfter = weightAfter / scale;				//	穩定應變規數據(將低位數浮動值銷除)
		printf("應變規原始數據/%d：%d\r\n", scale, weightAfter);
		weightAfter = weightAfter - coefficient;	//	應變規數據歸零
		printf("(應變規原始數據/%d)-(%d)：%d\r\n", scale, coefficient, weightAfter);
		if (weightAfter < 1) {
			weightAfter = 0;
		}
		weightAfter = (int)(weightAfter / multiple);
		printf("重量感測值：%d\r\n",weightAfter);
		putWeight = weightAfter;
		Systick_Delay_ms(1);
	} while (putWeight <= 100);
	
	printf("===============================開始檢測===============================\r\n");
	LCD_StringLineDisplay(Line1, "Fruit testing...");
	printf("===============================檢測重量===============================\r\n");
	Systick_Delay_ms(2000);										//	等待1秒，防止未放置完成
	/*
	int fruitWeight = GHV(2, 11);							//	取得應變規原始數據11次平均
	printf("應變規原始數據：%d\r\n",fruitWeight);
	fruitWeight = fruitWeight / scale;				//	穩定應變規數據(將低位數浮動值銷除)
	printf("應變規原始數據/%d：%d\r\n", scale, fruitWeight);
	fruitWeight = fruitWeight - coefficient;	//	應變規數據歸零
	printf("(應變規原始數據/%d)-(%d)：%d\r\n", scale, coefficient, fruitWeight);
	if (fruitWeight < 1) {
		fruitWeight = 0;
	}
	*/
	int fruitWeight = ABS(GHV(2, 11));				//	取得應變規原始數據11次平均(重量)
	printf("應變規原始數據：%d\r\n",fruitWeight);
	fruitWeight = fruitWeight / scale;				//	穩定應變規數據(將低位數浮動值銷除)
	printf("應變規原始數據/%d：%d\r\n", scale, fruitWeight);
	fruitWeight = fruitWeight - coefficient;	//	應變規數據歸零
	printf("(應變規原始數據/%d)-(%d)：%d\r\n", scale, coefficient, fruitWeight);
	if (fruitWeight < 1) {
		fruitWeight = 0;
	}
	fruitWeight = fruitWeight / multiple;
	printf("重量感測值：%d\r\n", fruitWeight);
	printf("===============================顯示重量===============================\r\n");
	char showWeight[14] = {'W', 'e', 'i', 'g', 'h', 't', ':', ' ', ' ', ' ', ' ', ' ', ' ', ' '};	//	預設顯示字元陣列
	char weightVal[4];	//	重量字元陣列
	int lengh = sprintf(weightVal,"%d",fruitWeight);
	for (int i = 0; i < lengh; i++) {
		showWeight[7 + i] = weightVal[i];
	}
	showWeight[7 + lengh] = 'g';
	LCD_StringLineDisplay(Line2, showWeight);
	Systick_Delay_ms(3000);
	printf("=================================抓住=================================\r\n");
	int catchBefore = GHV(1, 1) / 100;
	int catchAfter = catchBefore;
	int catchVal = catchAfter - catchBefore;
	int catchCount = 0;
	printf("先前數據：%d，量測數據：%d，相差：%d\r\n", catchBefore, catchAfter, (catchAfter - catchBefore));
	//while (!((catchAfter - catchBefore) > 1000)) {
	//while (1) {
	MOTOR1_1();
	MOTOR2_0();
	while (!(catchVal >= 1000)) {
		catchAfter = GHV(1, 1) / 100;				//	取得應變規原始數據1次平均(抓力)
		catchVal = catchAfter - catchBefore;
		printf("應變規數據：%d\r\n", catchVal);
		//printf("先前數據：%d，量測數據：%d，相差：%d\r\n", catchBefore, catchAfter, (catchAfter - catchBefore));
		catchCount++;
		Systick_Delay_ms(1);
	}
	MOTOR1_0();
	MOTOR2_0();
	printf("抓到芭樂總共\t%d\t筆\r\n", catchCount);
	Systick_Delay_ms(1000);
	printf("===============================檢測甜度===============================\r\n");
	int sweetnessResult = GSD(1);
	printf("===============================顯示甜度===============================\r\n");
	if (sweetnessResult == 1) {
		printf("甜度：不甜\r\n");
		LCD_StringLineDisplay(Line3, "Sweetness: non-sweet");
	} else if (sweetnessResult == 2) {
		printf("甜度：甜\r\n");
		LCD_StringLineDisplay(Line3, "Sweetness: sweet");
	} else {
		printf("甜度：Error\r\n");
		LCD_StringLineDisplay(Line3, "Sweetness: error");
	}
	Systick_Delay_ms(1000);
	printf("=================================抓緊=================================\r\n");
	int catchData[10];
	for (int i = 0; i < 10; i++) {
		int val = GHV(1, 1) / 100;
		catchData[i] = val - catchBefore;
		printf("%d\r\n", catchData[i]);
		MOTOR1_1();
		MOTOR2_0();
	}
	MOTOR1_0();
	MOTOR2_0();
	int hardness = GHD (catchData);
	printf("Hardness: %d", hardness);
	if (hardness == 1) {
		printf("硬度：軟\r\n");
		LCD_StringLineDisplay(Line4, "Hardness: soft");
	} else if (hardness == 2) {
		printf("硬度：硬\r\n");
		LCD_StringLineDisplay(Line4, "Hardness: hard");
	} else {
		printf("硬度：Error\r\n");
		LCD_StringLineDisplay(Line4, "Hardness: error");
	}
	printf("=================================復歸=================================\r\n");
	FruitReset();
	printf("=============================等待水果離開=============================\r\n");
	do {
		int weightAfter = ABS(GHV(2, 11));				//	取得應變規原始數據11次平均(重量)
		printf("應變規原始數據：%d\r\n",weightAfter);
		weightAfter = weightAfter / scale;				//	穩定應變規數據(將低位數浮動值銷除)
		printf("應變規原始數據/%d：%d\r\n", scale, weightAfter);
		weightAfter = weightAfter - coefficient;	//	應變規數據歸零
		printf("(應變規原始數據/%d)-(%d)：%d\r\n", scale, coefficient, weightAfter);
		if (weightAfter < 1) {
			weightAfter = 0;
		}
		weightAfter = (int)(weightAfter / multiple);
		printf("重量感測值：%d\r\n",weightAfter);
		putWeight = weightAfter;
		Systick_Delay_ms(1);
	} while (putWeight >= 100);
	FruitReset();
	return;
}

/*--- 2020/11/17 Calculation hardness and sweetness 計算軟硬、甜度 ---*/
/****************************
	*	@mode	1	=>	計算甜度		*
	*	@mode 2 =>	計算軟硬度	*
	****************************/
int CHS(int min[], int max[], int data[], int mode) {
	s64 minDistance = 0;
	s64 maxDistance = 0;
	int loopCount = 0;
	switch (mode) {
		case 1:
			loopCount = 6;
			break;
		case 2:
			loopCount = 10;
			break;
	}
	/*
	if (mode == 2) {
		int i = 14;
		minDistance += (min[i] - data[i]) < 0 ? (min[i] - data[i]) * -1 : (min[i] - data[i]);
		maxDistance += (max[i] - data[i]) < 0 ? (max[i] - data[i]) * -1 : (max[i] - data[i]);
		if (minDistance < maxDistance){
			return 1;
		} else if(minDistance > maxDistance){
			return 2;
		} else if(minDistance == maxDistance){
			return 3;
		} else {
			return -1;
		}
	} else {
		for(int i = 0; i < loopCount; i++){
			minDistance += (min[i] - data[i]) < 0 ? (min[i] - data[i]) * -1 : (min[i] - data[i]);
			maxDistance += (max[i] - data[i]) < 0 ? (max[i] - data[i]) * -1 : (max[i] - data[i]);
			printf("data= %d, mindata= %d, minDistance= %d\r\n", data[i], min[i], (min[i] - data[i]) < 0 ? (min[i] - data[i]) * -1 : (min[i] - data[i]));
			printf("data= %d, maxdata= %d, maxDistance= %d\r\n", data[i], max[i], (max[i] - data[i]) < 0 ? (max[i] - data[i]) * -1 : (max[i] - data[i]));
		}
	}
	*/
	for(int i = 0; i < loopCount; i++){
		/*
		if (mode == 2) {
			if (i < 10) {
				continue;
			}
		}
		*/
		minDistance += (min[i] - data[i]) < 0 ? (min[i] - data[i]) * -1 : (min[i] - data[i]);
		maxDistance += (max[i] - data[i]) < 0 ? (max[i] - data[i]) * -1 : (max[i] - data[i]);
		printf("data= %d, mindata= %d, minDistance= %d\r\n", data[i], min[i], (min[i] - data[i]) < 0 ? (min[i] - data[i]) * -1 : (min[i] - data[i]));
		printf("data= %d, maxdata= %d, maxDistance= %d\r\n", data[i], max[i], (max[i] - data[i]) < 0 ? (max[i] - data[i]) * -1 : (max[i] - data[i]));
	}
	printf("minDistance= %lld\r\n", minDistance);
	printf("maxDistance= %lld\r\n", maxDistance);
	if (minDistance < maxDistance){
		return 1;
	} else if(minDistance >= maxDistance){
		return 2;
	} else if(minDistance == maxDistance){
		return 3;
	} else {
		return -1;
	}
}
/*--- end ---*/

/*--- 2020/11/24 Get HX711 Value 取得HX711讀到參數 ---*/
int GHV (int mode, int frequency) {
	HX711Count = mode;
	int val = 0;
	int i = 0;
	for (i = 0; i < frequency; i++) {
		val += HX711Is_Read();
	}
	val /= frequency;
	return val;
}
/*--- end ---*/

/*--- 2020/11/24 Get sweetness data 取得甜度資料 ---*/
int GSD (int frequency) {
	IIC_Write_Byte(0x80,0x01);
	IIC_Write_Byte(0x70,0x08);
	IIC_Write_Byte(0x74,0x88);
	IIC_Write_Byte(0xA9,0x10);
	Systick_Delay_ms(1000);
	int sweetData[6] = {0, 0, 0, 0, 0, 0};
	for (int i = 0; i < frequency; i++) {
		loop();
		IIC_Read_Byte_(0x95);
		sweetData[0] += IIC_Read_Byte_(0x96)<<8|IIC_Read_Byte_(0x95);
		sweetData[1] += IIC_Read_Byte_(0x98)<<8|IIC_Read_Byte_(0x97);
		sweetData[2] += IIC_Read_Byte_(0x9A)<<8|IIC_Read_Byte_(0x99);
		sweetData[3] += IIC_Read_Byte_(0x9C)<<8|IIC_Read_Byte_(0x9B);
		sweetData[4] += IIC_Read_Byte_(0x9E)<<8|IIC_Read_Byte_(0x9D);
		sweetData[5] += IIC_Read_Byte_(0xA0)<<8|IIC_Read_Byte_(0x9F);
	}
	sweetData[0] /= frequency;
	sweetData[1] /= frequency;
	sweetData[2] /= frequency;
	sweetData[3] /= frequency;
	sweetData[4] /= frequency;
	sweetData[5] /= frequency;
	printf("NIR1: %d\r\n",sweetData[0]);
	printf("NIR2: %d\r\n",sweetData[1]);
	printf("NIR3: %d\r\n",sweetData[2]);
	printf("NIR4: %d\r\n",sweetData[3]);
	printf("NIR5: %d\r\n",sweetData[4]);
	printf("NIR6: %d\r\n",sweetData[5]);
	int minSData[6] = {	1030,
											1220,
											860,
											1035,
											873,
											3932};
	int maxSData[6] = {	676,
											609,
											619,
											738,
											351,
											2532};
	IIC_Write_Byte(0x80,0x01);
	IIC_Write_Byte(0x70,0x00);
	IIC_Write_Byte(0x74,0x88);
	IIC_Write_Byte(0xA9,0x10);
	return CHS(minSData, maxSData, sweetData, 1);
}
/*--- end --*/

/*--- 2020/11/25 Get hardness data 取得軟硬資料 ---*/
int GHD (int catchData[]) {
	//int minHData[20] = {-5547383, -5547377, -5545690, -5533989, -5508117, -5475374, -5446675, -5422579, -5401479, -5379003, -5357531, -5342720, -5324307, -5304733, -5287055, -5270618, -5254157, -5240733, -5231076, -5224518};
	//int minHData[10] = {-6162538, -6161696, -6159353, -6142154, -6110375, -6081501, -6062620, -6053745, -6048381, -6044132};
	int minHData[10] = {2203,
											2202,
											2205,
											2278,
											2519,
											2871,
											3310,
											3775,
											4139,
											4385};
	//int maxHData[20] = {-5544878, -5544917, -5543043, -5527081, -5497381, -5469668, -5444000, -5416954, -5388938, -5361309, -5332578, -5302243, -5272982, -5244622, -5218226, -5191160, -5161005, -5130079, -5100329, -5072896};
	//int maxHData[10] = {-6024696, -6024152, -6023293, -6016491, -5992302, -5931865, -5819864, -5698704, -5622603, -5579521};
	int maxHData[10] = {2470,
											2471,
											2474,
											2566,
											2926,
											3379,
											3775,
											4276,
											4869,
											5330};
	CHS(minHData, maxHData, catchData, 2);
}
/*--- end ---*/

/*--- 2020/11/25 Reset 完成檢測後復歸 ---*/
void FruitReset (void) {
	while (GPIO_ReadInBit(Reset_Btn_HT, Reset_Btn_GP_Pin)) {
		MOTOR1_0();
		MOTOR2_1();
	}
	MOTOR1_0();
	MOTOR2_0();
}
/*--- end ---*/

/*--- 2020/11/26 ABS 絕對值 ---*/
int ABS (int val) {
	return val < 1 ? val * -1 : val;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------*/
void HX711WaitReady(long delay_ms) {
  while (!HX711Is_Ready()) {
    // Probably will do no harm on AVR but will feed the Watchdog Timer (WDT) on ESP.
    // https://github.com/bogde/HX711/issues/73
    Systick_Delay_ms(delay_ms);
  }
}

bool HX711Is_Ready(void) {
  bool result = FALSE;
  switch (HX711Count) {
		case 1:
			if(GPIO_ReadInBit(Catch_HX711_DT_HT, Catch_HX711_DT_GP_Pin) == 0) {
				result = TRUE;
			}
			break;
		case 2:
			if(GPIO_ReadInBit(Weight_HX711_DT_HT, Weight_HX711_DT_GP_Pin) == 0) {
				result = TRUE;
			}
			break;
	}
  return result;
}
u32 HX711Is_Read(void) {
  // Wait for the chip to become ready.
  u64 delay_ms = 0;
  u32 value = 0x00000000;
  u8 data[3];
  u8 filler;
	u64 i;
  HX711WaitReady(delay_ms);
  // Protect the read sequence from system interrupts.  If an interrupt occurs during
  // the time the SCK_PIN signal is high it will stretch the length of the clock pulse.
  // If the total pulse time exceeds 60 uSec this will cause the HX711 to enter
  // power down mode during the middle of the read sequence.  While the device will
  // wake up when SCK_PIN goes low again, the reset starts a new conversion cycle which
  // forces DT_PIN high until that cycle is completed.
  //
  // The result is that all subsequent bits read by shiftIn() will read back as 1,
  // corrupting the value returned by read().  The ATOMIC_BLOCK macro disables
  // interrupts during the sequence and then restores the interrupt mask to its previous
  // state after the sequence completes, insuring that the entire read-and-gain-set
  // sequence is not interrupted.  The macro has a few minor advantages over bracketing
  // the sequence between `noInterrupts()` and `interrupts()` calls.
  #if HAS_ATOMIC_BLOCK
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

  #elif IS_FREE_RTOS
  // Begin of critical section.
  // Critical sections are used as a valid protection method
  // against simultaneous access in vanilla FreeRTOS.
  // Disable the scheduler and call portDISABLE_INTERRUPTS. This prevents
  // context switches and servicing of ISRs during a critical section.
  portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
  portENTER_CRITICAL(&mux);

  #else
  // Disable interrupts.
  //noInterrupts();
  #endif
  // Pulse the clock pin 24 times to read the data.
  
  data[2] = SHIFTIN_WITH_SPEED_SUPPORT();
  data[1] = SHIFTIN_WITH_SPEED_SUPPORT();
  data[0] = SHIFTIN_WITH_SPEED_SUPPORT();
  
  // Set the channel and the gain factor for the next reading using the clock pin.
  
  for (i = 0; i < GAIN; i++) {
    switch (HX711Count) {
			case 1:
				GPIO_WriteOutBits(Catch_HX711_SCK_HT, Catch_HX711_SCK_GP_Pin, SET);
				#if ARCH_ESPRESSIF
				Systick_Delay_us(1);
				#endif
				GPIO_WriteOutBits(Catch_HX711_SCK_HT, Catch_HX711_SCK_GP_Pin, RESET);
				#if ARCH_ESPRESSIF
				Systick_Delay_us(1);
				#endif
				break;
			case 2:
				GPIO_WriteOutBits(Weight_HX711_SCK_HT, Weight_HX711_SCK_GP_Pin, SET);
				#if ARCH_ESPRESSIF
				Systick_Delay_us(1);
				#endif
				GPIO_WriteOutBits(Weight_HX711_SCK_HT, Weight_HX711_SCK_GP_Pin, RESET);
				#if ARCH_ESPRESSIF
				Systick_Delay_us(1);
				#endif
				break;
		}
  }

  #if IS_FREE_RTOS
  // End of critical section.
  portEXIT_CRITICAL(&mux);

  #elif HAS_ATOMIC_BLOCK
  }

  #else
  // Enable interrupts again.
  //interrupts();
  #endif

  // Replicate the most significant bit to pad out a 32-bit signed integer
  if (data[2] & 0x80) {
    filler = 0xFF;
  } else {
    filler = 0x00;
  }

  // Construct a 32-bit signed integer
  
  value = ( filler << 24
      | data[2] << 16
      | data[1] << 8
      | data[0] );
  
  return value;
}

u8 shiftInSlow(void) {
  u8 value = 0x00;
  u8 i;
  for(i = 0; i < 8; ++i) {
    switch (HX711Count) {
			case 1:
				GPIO_WriteOutBits(Catch_HX711_SCK_HT, Catch_HX711_SCK_GP_Pin, SET);
				Systick_Delay_us(1);
				value |= GPIO_ReadInBit(Catch_HX711_DT_HT, Catch_HX711_DT_GP_Pin) << (7 - i);
				GPIO_WriteOutBits(Catch_HX711_SCK_HT, Catch_HX711_SCK_GP_Pin, RESET);
				Systick_Delay_us(1);
				break;
			case 2:
				GPIO_WriteOutBits(Weight_HX711_SCK_HT, Weight_HX711_SCK_GP_Pin, SET);
				Systick_Delay_us(1);
				value |= GPIO_ReadInBit(Weight_HX711_DT_HT, Weight_HX711_DT_GP_Pin) << (7 - i);
				GPIO_WriteOutBits(Weight_HX711_SCK_HT, Weight_HX711_SCK_GP_Pin, RESET);
				Systick_Delay_us(1);	
				break;
		}
  }
  return value;
}
void GPIO_OUT_motor(void)
{
	AFIO_GPxConfig(HTCFG_OUTPUT_MOTOR1_ID, HTCFG_OUTPUT_MOTOR1_AFIO_PIN, AFIO_FUN_GPIO);
  AFIO_GPxConfig(HTCFG_OUTPUT_MOTOR1_ID, HTCFG_OUTPUT_MOTOR1_AFIO_PIN, AFIO_FUN_GPIO);
  GPIO_DirectionConfig(HTCFG_MOTOR2, HTCFG_OUTPUT_MOTOR2_GPIO_PIN, GPIO_DIR_OUT);
  GPIO_DirectionConfig(HTCFG_MOTOR1, HTCFG_OUTPUT_MOTOR1_GPIO_PIN, GPIO_DIR_OUT);
}
#if (HT32_LIB_DEBUG == 1)
/*********************************************************************************************************//**
  * @brief  Report both the error name of the source file and the source line number.
  * @param  filename: pointer to the source file name.
  * @param  uline: error line source number.
  * @retval None
  ***********************************************************************************************************/
void assert_error(u8* filename, u32 uline)
{
  /*
     This function is called by IP library that the invalid parameters has been passed to the library API.
     Debug message can be added here.
     Example: printf("Parameter Error: file %s on line %d\r\n", filename, uline);
  */

  while (1)
  {
  }
}
#endif


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
