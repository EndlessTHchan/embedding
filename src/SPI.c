#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "stdio.h"
/*******************************************************************************
//STM32F207VGT6 FOR ENC28J60
//SPI2鍒濆鍖?IO鍒濆鍖栫瓑
//SPI configuration
// JUST FOR STM32F2XX
*******************************************************************************/
void ENC25Q80_SPI2_Init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;

     /*!< Enable the SPI2 clock */        
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
 
    /*!< Enable GPIO clocks */
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO , ENABLE); 	
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); //Disable jtag	,Enable SWD
		GPIO_PinRemapConfig(GPIO_Remap_SPI1 , ENABLE); //Disable jtag	,Enable SWD
    /*!< SPI2 pins configuration */
    
   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // also 100Mhz
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);// PB10/14/15-SCK,MISO,MOSI
      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; // CS for enc28j60
    GPIO_Init(GPIOC, &GPIO_InitStructure);
  
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//SPI璁剧疆涓哄弻绾垮弻鍚戝叏鍙屽伐
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//璁剧疆涓轰富SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//璁剧疆SPI鐨勬暟鎹ぇ灏?SPI鍙戦€佹帴鏀?浣嶅抚缁撴瀯
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//閫夋嫨浜嗕覆琛屾椂閽熺殑绋虫€?鏃堕挓鎮┖楂?
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//鏁版嵁鎹曡幏浜庣浜屼釜鏃堕挓娌?
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//(SPI_NSS_Soft)姝ゆ椂NSS寮曡剼鍙互閰嶇疆鎴愭櫘閫欸PIO鍘绘帶鍒朵粠璁惧
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//Fclk/2
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; /* Initialize the SPI_FirstBit member */
    SPI_InitStructure.SPI_CRCPolynomial=7;
    SPI_Init(SPI2, &SPI_InitStructure);
    SPI_Cmd(SPI2, ENABLE);
	SPI2 -> CR2 |= 1<<6;		 //鎺ユ敹缂撳啿鍖洪潪绌轰腑鏂娇鑳?
}

/*******************************************************************************
* Function Name  : SPI_ReadWriteByte
* Description    : SPI璇诲啓涓€涓瓧鑺傦紙鍙戦€佸畬鎴愬悗杩斿洖鏈閫氳璇诲彇鐨勬暟鎹級
* Input          : unsigned char TxData 
* Output         : None
* Return         : unsigned char RxData
*******************************************************************************/
unsigned char SPI2_ReadWriteByte(unsigned char TxData)
{
    unsigned char RxData = 0;
	
		GPIO_ResetBits(GPIOC,GPIO_Pin_3);
    /* Wait till Transmit buffer is empty */ 
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    /* Send A data */        
    SPI_I2S_SendData(SPI2, TxData);
    // while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
    //绛夊緟鏁版嵁鎺ユ敹
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    //鍙栨暟鎹?
    RxData = SPI_I2S_ReceiveData(SPI2);
	
		GPIO_SetBits(GPIOC,GPIO_Pin_3);
	
    return (unsigned char)RxData;
}

#define W25X_DeviceID 0xAB
#define SPI_FLASH_CS_LOW() GPIO_ResetBits(GPIOC,GPIO_Pin_3)
#define SPI_FLASH_CS_HIGH() GPIO_SetBits(GPIOC,GPIO_Pin_3)

u8 SPI_FLASH_SendByte(u8 data)
{
	SPI_I2S_SendData(SPI2,data);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI2);
}


u32 SPI_FLASH_ReadDeviceID(void) 
{ 
	u32 Temp = 0; 
	//浣跨敤鐨勬椂鍊欏氨鎷変綆/ 
	SPI_FLASH_CS_LOW(); 
	/* Send 鈥淩DID 鈥?instruction */ 
	SPI_FLASH_SendByte(0xAB); 
	SPI_FLASH_SendByte(0); 
	SPI_FLASH_SendByte(0); 
	SPI_FLASH_SendByte(0); 
	/* Read a byte from the FLASH */ 
	Temp = SPI_FLASH_SendByte(0); 
	/* Deselect the FLASH: Chip Select high */ 
	SPI_FLASH_CS_HIGH(); 
	printf("%d\n",Temp);
return Temp; 
} 

void SPI2_IRQHandler(){
	if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != RESET){
		
	}
	if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != RESET){
		
	}
}