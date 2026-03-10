#include "stm32f10x.h"
#include <stm32f10x_usart.h>
#include "stdio.h"
#include "stdint.h"


#define MaxTxBuffLength 100

char* TxBuff;
int SetTxBuff;
int TxIndicator;


void RS232SendByte(char data);
void RS232SendStr(char* data);

void UART1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	USART_InitTypeDef USART_InitStructure; 
	NVIC_InitTypeDef   NVIC_InitStructure;
	//引脚时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//串口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


  /*
  *  USART1_TX -> PA9 , USART1_RX ->	PA10
  */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);		   

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		//串口1初始化
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	//配置发送中断和接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_TC, ENABLE);//空闲中断
	USART_Cmd(USART1, ENABLE);
	USART_ClearITPendingBit(USART1, USART_IT_TC);//清除中断TC位

	//NVIC_Config
	


  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
  
  /* Enable the Ethernet global Interrupt */
  
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
/***********************************************************************
函数名称：void USART1_IRQHandler(void) 
功    能：完成SCI的数据的接收，并做标识
输入参数：
输出参数：
编写时间：2012.11.22
编 写 人：
注    意  RS485用的是USART3.
***********************************************************************/

//#define Check_Sum // 使用校验和检验
//#define RCR //使用RCR验证

u8 ReceiveData;
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define USART_TX_LEN			200
char USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 RX_count=0;
u16 FrameFlag = 0;
char USART_TX_BUF[USART_TX_LEN];
uint8_t Flag = 0;

uint8_t Transmit_state_OK = 1;
uint8_t Receive_state_OK = 1;
#ifdef Check_Sum
uint8_t checksum = 0;
uint8_t sendsum = 0;
#endif
	//接收状态
	//bit15，	接收完成标志
	//bit14，	接收到0x0d
	//bit13~0，	接收到的有效字节数目c
u16 USART_RX_STA=0;       //接收状态标记

void USART1_IRQHandler(void)  
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		if(Receive_state_OK){
			Receive_state_OK = 0;
			// 开启TIM3定时器
			TIM3->CNT = 0;
			TIM3->CR1 |= 1;
		}
		if(!Receive_state_OK){
			ReceiveData =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
			USART_RX_BUF[RX_count]=ReceiveData ;
			#ifdef Check_Sum
			checksum = checksum ^ ReceiveData;
			#endif
			RX_count++;
			// 计数器归零
			TIM3->CNT = 0;
		}
		if(RX_count>=USART_REC_LEN)	{
			RX_count=0;//接收超出最大长度，重新开始			
			RS232SendStr("\nReceive string is out of range:200\n");
		}
	}
	if (USART_GetITStatus(USART1, USART_IT_TC) != RESET) 
	{
			USART_ClearITPendingBit(USART1, USART_IT_TC);           /* Clear the USART transmit interrupt       */
		if(TxBuff[TxIndicator]!='\0' && TxIndicator < MaxTxBuffLength)//数组的索引max永远小于数组元素的个数
		{
			RS232SendByte(TxBuff[TxIndicator]);
			TxIndicator++;
		}
		if(TxIndicator >= MaxTxBuffLength){
			RS232SendStr("\nTransmit string is out of range:100\n");
		}
		if(TxBuff[TxIndicator]=='\0'){
			Transmit_state_OK = 1;
		}
	}
}


void RS232SendByte(char data)
{
		USART1->DR = data;
}

void RS232SendStr(char* data){
	int i = 0;
	#ifdef Check_Sum
	sendsum = 0;
	#endif
	while(!Transmit_state_OK){}
	TxIndicator = 1;
	Transmit_state_OK = 0;
	while(data[i] != '\0'){
		USART_TX_BUF[i] = data[i];
		#ifdef Check_Sum
		sendsum = sendsum ^ data[i];
		#endif
		i = i+ 1;
	}
	#ifdef Check_Sum
	USART_TX_BUF[i] = sendsum;
	USART_TX_BUF[i + 1] = '\0';
	#endif
	USART_TX_BUF[i] = '\0';
	TxBuff = USART_TX_BUF;
	RS232SendByte(USART_TX_BUF[0]);
}

void TIM3_IRQHandler(){
	if(TIM3->SR & (1<<0))      //溢出中断
    {
        TIM3->SR &= ~(1<<0);  //清除中断标志位
        //执行相应操作
		#ifdef CHECK
		if(checksum != 0){
			// 校验失败
			Receive_state_OK = 1;
			/* 关闭定时器 */
			TIM3->CR1 &= (0xFFFF - 0x0001);
			// 计数器归零
			TIM3->CNT = 0;
			Flag = 0;
			RS232SendStr("校验码错误");
			RX_count = 0;
			checksum = 0;
		}
		else {
			Receive_state_OK = 1;
			/* 关闭定时器 */
			TIM3->CR1 &= (0xFFFF - 0x0001);
			// 计数器归零
			TIM3->CNT = 0;
			Flag = 1; //置位
		}
		
		#else 
		
		Receive_state_OK = 1;
			/* 关闭定时器 */
			TIM3->CR1 &= (0xFFFF - 0x0001);
			// 计数器归零
			TIM3->CNT = 0;
			Flag = 1; //置位
		#endif
    }
}