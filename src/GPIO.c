/***********************************************************************
文件名称：LED.C
功    能：led  IO初始化
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
//#include "stm32f10x.h"
#include "sys.h"
#include "stm32f10x_gpio.h"
#define Base_Control

#define RCC_APB2ENR         (*((volatile unsigned int*)0x40021018))   // APB2 外设时钟使能寄存器
	
#define GPIOC_base			0x40011000
#define GPIOC_CRH			(*((volatile unsigned int*)(GPIOC_base + 0x04)))
#define GPIOC_CRL           (*((volatile unsigned int*)(GPIOC_base + 0x00)))	// 端口配置低寄存器
#define GPIOC_BSRR			(*((volatile unsigned int*)(GPIOC_base + 0x10)))   // 端口位设置/复位寄存器
#define GPIOC_IDR			(*((volatile unsigned int*)(GPIOC_base + 0x08)))
#define GPIOC_ODR			(*((volatile unsigned int*)(GPIOC_base + 0x0C)))

#define GPIOD_base			0x40011400
#define GPIOD_CRH			(*((volatile unsigned int*)(GPIOD_base + 0x04)))
#define GPIOD_CRL           (*((volatile unsigned int*)(GPIOD_base + 0x00)))	// 端口配置低寄存器
#define GPIOD_BSRR			(*((volatile unsigned int*)(GPIOD_base + 0x10)))   // 端口位设置/复位寄存器
#define GPIOD_IDR			(*((volatile unsigned int*)(GPIOD_base + 0x08)))
#define GPIOD_ODR			(*((volatile unsigned int*)(GPIOD_base + 0x0C)))
	


#define GPIOE_CRH           (*((volatile unsigned int*)0x40011804))   // 端口配置高寄存器
#define GPIOE_BSRR          (*((volatile unsigned int*)0x40011810))   // 端口位设置/复位寄存器
#define GPIOE_IDR           (*((volatile unsigned int*)0x40011808))   // 端口输入数据寄存器	
#define GPIOE_ODR           (*((volatile unsigned int*)0x4001180C))   // 端口输出数据寄存器
	
#define GPIOB_CRL           (*((volatile unsigned int*)0x40010C00))   // 端口配置低寄存器
#define GPIOB_IDR           (*((volatile unsigned int*)0x40010C08))   // 端口输入数据寄存器	

#define GPIOE_PIN_8 8
#define GPIOE_PIN_9 9
#define GPIOE_PIN_10 10
#define GPIOE_PIN_11 11

void gpio_Init(void)
{
	#ifdef Base_Control
//		RCC_APB2ENR |= 1 << 6 | 1 << 5 | 1 << 4;          //使能PORTE,PORTD,PORTC时钟	

//		GPIOE_CRH &= 0xFFFF0000;       //清除PE8，9，10，11引脚原来设置  
//		GPIOE_CRH |= 0x3333;			        //设置CNF8[1:0]为0x00：通用推挽输出模式，MODE8[1:0]为0x11：输出模式，其他同理
//		GPIOE_BSRR |= 15 << 8;           //端口位设置


//		GPIOD_CRL &= 0x00000000;
//		GPIOD_CRL |= 0x33333333;
//		GPIOD_BSRR |= ((1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7) << 16);
//		
//		GPIOC_CRH &= 0xFF0000FF;
//		GPIOC_CRH |= 0x00333300;
//		GPIOC_BSRR |= ((1 << 10 | 1 << 11 | 1 << 12 | 1 << 13) << 16);
	#endif
	#ifdef StdLib_Control
		#ifdef led
		GPIO_InitTypeDef GPIO_Instance;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
		
		GPIO_Instance.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Instance.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
		GPIO_Instance.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_SetBits(GPIOE,GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11);
		GPIO_Init(GPIOE,&GPIO_Instance);
		#endif
		#ifdef datatube
		
		GPIO_Instance.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Instance.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_Instance.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_ResetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
		GPIO_Init(GPIOD,&GPIO_Instance);
		
		GPIO_Instance.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Instance.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
		GPIO_Instance.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_ResetBits(GPIOC,GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13);
		GPIO_Init(GPIOC,&GPIO_Instance);
		#endif
	#endif
}


/*
PE4~7下拉输入，0~3上拉输出
*/
int key_line(){
	#ifdef Base_Control
		GPIOE->CRL &= 0xF0000000;
		GPIOE->CRL |= 0x08883333;
		GPIOE_BSRR |= 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3;
		GPIOE_BSRR |= (1 << 4 | 1 << 5 | 1 << 6) << 16;//  | 1 << 7
		EXTI->IMR |= 1 << 4 | 1 << 5 | 1 << 6; //  | 1 << 7
		return 1;
	#endif
	#ifdef StdLib_Control
		GPIO_InitTypeDef GPIO_Instance;
		
		GPIO_Instance.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Instance.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
		GPIO_Instance.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_SetBits(GPIOE,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
		GPIO_Init(GPIOE,&GPIO_Instance);
	
		GPIO_Instance.GPIO_Mode = GPIO_Mode_IPD;
		GPIO_Instance.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;//  | GPIO_Pin_7
		GPIO_Instance.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOE,&GPIO_Instance);
		EXTI->IMR |= 1 << 4 | 1 << 5 | 1 << 6; //  | 1 << 7
		return 1;
	#endif
}
/*
PE4~7下拉输出，0~3下拉输入
*/
int key_column(){
	#ifdef Base_Control
		GPIOE->CRL &= 0xF0000000;
		GPIOE->CRL |= 0x03338888;
		GPIOE_BSRR |= (1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6) << 16;
		EXTI->IMR &= ~(1 << 4) & ~(1 << 5) & ~(1 << 6);//  & ~(1 << 7)
		return 2;
	#endif
	#ifdef StdLib_Control
		GPIO_InitTypeDef GPIO_Instance;
		GPIO_Instance.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Instance.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6; // | GPIO_Pin_7
		GPIO_Instance.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_ResetBits(GPIOE,GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6); //  | GPIO_Pin_7
		GPIO_Init(GPIOE,&GPIO_Instance);
		
		GPIO_Instance.GPIO_Mode = GPIO_Mode_IPD;
		GPIO_Instance.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
		GPIO_Instance.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOE,&GPIO_Instance);
		EXTI->IMR &= ~(1 << 4) & ~(1 << 5) & ~(1 << 6) ; // & ~(1 << 7)
		return 2;
	#endif
}

void Delay(unsigned int nCount)
{ 
  while(nCount > 0)
  { 
  	  nCount --;   
  }
}

void LED_Set(int PIN)
{
	#ifdef Base_Control
		switch(PIN){
			case GPIOE_PIN_8:
				GPIOE_BSRR &= ~(1 << 24); 
				GPIOE_BSRR |= 1 << 8; 
				break;
			case GPIOE_PIN_9:
				GPIOE_BSRR &= ~(1 << 25);
				GPIOE_BSRR |= (1 << 9);
				break;
			case GPIOE_PIN_10:
				GPIOE_BSRR &= ~(1 << 26);
				GPIOE_BSRR |= (1 << 10);
				break;
			case GPIOE_PIN_11:
				GPIOE_BSRR &= ~(1 << 27);
				GPIOE_BSRR |= (1 << 11);
				break;
		}
	#endif
	#ifdef StdLib_Control
		switch(PIN) {
			case GPIOE_PIN_8:
				GPIO_ResetBits(GPIOE,GPIO_Pin_8);
				break;
			case GPIOE_PIN_9:
				GPIO_ResetBits(GPIOE,GPIO_Pin_9);
				break;
			case GPIOE_PIN_10:
				GPIO_ResetBits(GPIOE,GPIO_Pin_10);
				break;
			case GPIOE_PIN_11:
				GPIO_ResetBits(GPIOE,GPIO_Pin_11);
				break;
		}
	#endif
}

void LED_Reset(int PIN)
{
	#ifdef Base_Control
		switch(PIN){
			case GPIOE_PIN_8:
				GPIOE_BSRR &= ~(1 << 8);      //清除第8bit
				GPIOE_BSRR |= 1 << 24; //清除对应的ODR8位为0，即PE8引脚输出低电平
				break;
			case GPIOE_PIN_9:
				GPIOE_BSRR &= ~(1 << 9);
				GPIOE_BSRR |= (1 << 25);
				break;
			case GPIOE_PIN_10:
				GPIOE_BSRR &= ~(1 << 26);
				GPIOE_BSRR |= (1 << 10);
				break;
			case GPIOE_PIN_11:
				GPIOE_BSRR &= ~(1 << 11);
				GPIOE_BSRR |= (1 << 27);
				break;
		}
	#endif
	#ifdef StdLib_Control
		switch(PIN) {
			case GPIOE_PIN_8:
				GPIO_SetBits(GPIOE,GPIO_Pin_8);
				break;
			case GPIOE_PIN_9:
				GPIO_SetBits(GPIOE,GPIO_Pin_9);
				break;
			case GPIOE_PIN_10:
				GPIO_SetBits(GPIOE,GPIO_Pin_10);
				break;
			case GPIOE_PIN_11:
				GPIO_SetBits(GPIOE,GPIO_Pin_11);
				break;
		}
	#endif
}


void LED_Toggel(int PIN)
{
	#ifdef Base_Control
		switch(PIN){
			case GPIOE_PIN_8:
				if (GPIOE_ODR & (1 << 8)) {
					GPIOE_BSRR &= ~(1 << 8);      //清除第8bit
					GPIOE_BSRR |= 1 << 24; //清除对应的ODR8位为0，即PE8引脚输出低电平
				} else {
					GPIOE_BSRR &= ~(1 << 24); 
					GPIOE_BSRR |= 1 << 8; 
				}
				break;
			case GPIOE_PIN_9:
				if (GPIOE_ODR & (1 << 9)) {
					GPIOE_BSRR &= ~(1 << 9);
					GPIOE_BSRR |= (1 << 25);
				} else {
					GPIOE_BSRR &= ~(1 << 25);
					GPIOE_BSRR |= (1 << 9);
				}
				break;
			case GPIOE_PIN_10:
				if (GPIOE_ODR & (1 << 10)) {
					GPIOE_BSRR &= ~(1 << 10);
					GPIOE_BSRR |= (1 << 26);
				} else {
					GPIOE_BSRR &= ~(1 << 26);
					GPIOE_BSRR |= (1 << 10);
				}
				break;
			case GPIOE_PIN_11:
				if (GPIOE_ODR & (1 << 11)) {
					GPIOE_BSRR &= ~(1 << 11);
					GPIOE_BSRR |= (1 << 27);
				} else {
					GPIOE_BSRR &= ~(1 << 27);
					GPIOE_BSRR |= (1 << 11);
				}
				break;
		}
	#endif
	#ifdef StdLib_Control
	switch(PIN){
		case GPIOE_PIN_8:
			if (GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_8)) {
				GPIO_ResetBits(GPIOE,GPIO_Pin_8);
			} else {
				GPIO_SetBits(GPIOE,GPIO_Pin_8);
			}
			break;
		case GPIOE_PIN_9:
			if (GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_9)) {
				GPIO_ResetBits(GPIOE,GPIO_Pin_9);
			} else {
				GPIO_SetBits(GPIOE,GPIO_Pin_9);
			}
			break;
		case GPIOE_PIN_10:
			if (GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_10)) {
				GPIO_ResetBits(GPIOE,GPIO_Pin_10);
			} else {
				GPIO_SetBits(GPIOE,GPIO_Pin_10);
			}
			break;
		case GPIOE_PIN_11:
			if (GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_11)) {
				GPIO_ResetBits(GPIOE,GPIO_Pin_11);
			} else {
				GPIO_SetBits(GPIOE,GPIO_Pin_11);
			}
			break;
	}
	#endif
}




void number_set(int num,int index) {
	#ifdef Base_Control
		int port = index + 9;
		GPIOC_BSRR |= 1 << 10 | 1 << 11 | 1 << 12 | 1 << 13;
		GPIOC_BSRR |= 1 << port << 16;
		switch(num){
			case 0:
				GPIOD_BSRR |= (1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5) << 16;
				GPIOD_BSRR |= 1 << 7 | 1 << 6;
				break;
			case 1:
				GPIOD_BSRR |= (1 << 1 | 1 << 2) << 16;
				GPIOD_BSRR |= 1 << 0 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7;
			break;
			case 2:
				GPIOD_BSRR |= (1 << 0 | 1 << 1 | 1 << 3 | 1 << 4 | 1 << 6) << 16;
				GPIOD_BSRR |= 1 << 2 | 1 << 5 | 1 << 7;
				break;
			case 3:
				GPIOD_BSRR |= (1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 6) << 16;
				GPIOD_BSRR |= 1 << 4 | 1 << 5 | 1 << 7;
				break;
			case 4:
				GPIOD_BSRR |= (1 << 1 | 1 << 2 | 1 << 5 | 1 << 6) << 16;
				GPIOD_BSRR |= 1 << 0 | 1 << 3 | 1 << 4 | 1 << 7;
				break;
			case 5:
				GPIOD_BSRR |= (1 << 0 | 1 << 2 | 1 << 3 | 1 << 5 | 1 << 6) << 16;
				GPIOD_BSRR |= 1 << 1 | 1 << 4 | 1 << 7;
				break;
			case 6:
				GPIOD_BSRR |= (1 << 0 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6) << 16;
				GPIOD_BSRR |= 1 << 1 | 1 << 7;
				break;
			case 7:
				GPIOD_BSRR |= (1 << 0 | 1 << 1 | 1 << 2) << 16;
				GPIOD_BSRR |= 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7;
				break;
			case 8:
				GPIOD_BSRR |= (1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6) << 16;
				GPIOD_BSRR |= 1 << 7;
				break;
			case 9:
				GPIOD_BSRR |= (1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 5 | 1 << 6) << 16;
				GPIOD_BSRR |= 1 << 4 | 1 << 7;
				break;
		}
	#endif
	#ifdef StdLib_Control
		int port = index + 9;
		GPIO_SetBits(GPIOC,GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13);
		switch(port){
			case 10:
				GPIO_ResetBits(GPIOC,GPIO_Pin_10);
				break;
			case 11:
				GPIO_ResetBits(GPIOC,GPIO_Pin_11);
				break;
			case 12:
				GPIO_ResetBits(GPIOC,GPIO_Pin_12);
				break;
			case 13:
				GPIO_ResetBits(GPIOC,GPIO_Pin_13);
				break;
		}
		switch(num){
			case 0:
				GPIO_ResetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
				GPIO_SetBits(GPIOD,GPIO_Pin_6 | GPIO_Pin_7);
				break;
			case 1:
				GPIO_ResetBits(GPIOD,GPIO_Pin_1 | GPIO_Pin_2);
				GPIO_SetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
			break;
			case 2:
				GPIO_ResetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6);
				GPIO_SetBits(GPIOD,GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_7);
				break;
			case 3:
				GPIO_ResetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6);
				GPIO_SetBits(GPIOD,GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7);
				break;
			case 4:
				GPIO_ResetBits(GPIOD,GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6);
				GPIO_SetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7);
				break;
			case 5:
				GPIO_ResetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6);
				GPIO_SetBits(GPIOD,GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_7);
				break;
			case 6:
				GPIO_ResetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
				GPIO_SetBits(GPIOD,GPIO_Pin_1 | GPIO_Pin_7);
				break;
			case 7:
				GPIO_ResetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
				GPIO_SetBits(GPIOD,GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
				break;
			case 8:
				GPIO_ResetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
				GPIO_SetBits(GPIOD,GPIO_Pin_7);
				break;
			case 9:
				GPIO_ResetBits(GPIOD,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6);
				GPIO_SetBits(GPIOD,GPIO_Pin_4 | GPIO_Pin_7);
				break;
		}
	#endif
}



//void KEY_Init(void)
//{
//		RCC_APB2ENR |=1<<3;    //使能PORTB时钟	
//		GPIOB_CRL |=0X04000000;// PB.6浮空输入	
//}

//u8 KEY_Scan(void)
//{	 
//	//u8 key_1 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6);
//	u8 key_1= 0;
//	if(GPIOB_IDR & 0x40)
//			key_1=1;
//	else 
//  		key_1=0;
//	if(key_1==0)
//	{
//		delay_ms(100);//去抖动 
//		if(key_1==0)return 1;
//	} 
// 	return 0;// 无按键按下
//}



