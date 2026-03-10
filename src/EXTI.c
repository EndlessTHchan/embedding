#include "stm32f10x.h"

//设置NVIC分组
//NVIC_Group:NVIC分组 0~4 总共5组 		   
//void MY_NVIC_PriorityGroupConfig()	 
//{ 
//	/*该寄存器的[10:8]3 位就是 PRIGROUP 的定义位，它的值规定了系统中
//	有多少个抢先级中断和子优先级中断。而 STM32 只使用高 4 位 bits，  其可能的值如下（来
//	自 ST 的函数库头文件中的定义）*/

//	//#define NVIC_PriorityGroup_0 ((uint32_t)0x700) /*!< 0 bits for pre-emption priority
//	//4 bits for subpriority */
//	//#define NVIC_PriorityGroup_1 ((uint32_t)0x600) /*!< 1 bits for pre-emption priority
//	//3 bits for subpriority */
//	//#define NVIC_PriorityGroup_2 ((uint32_t)0x500) /*!< 2 bits for pre-emption priority
//	//2 bits for subpriority */
//	//#define NVIC_PriorityGroup_3 ((uint32_t)0x400) /*!< 3 bits for pre-emption priority
//	//1 bits for subpriority */
//	//#define NVIC_PriorityGroup_4 ((uint32_t)0x300) /*!< 4 bits for pre-emption priority
//	//0 bits for subpriority */
//	
//	//设置NVIC分组为2
//		SCB_AIRCR |=(0x05FA0000 | NVIC_PriorityGroup_2);  //为了防止误操作（写），因此当改写这个寄存器的内容时，必须要同时向这个寄存器的高 16 位[31：16]写验证字（Register key） 0x05FA
//}

//设置NVIC 
//NVIC_PreemptionPriority:抢占优先级
//NVIC_SubPriority       :响应优先级
//NVIC_Channel           :中断编号
//NVIC_Group             :中断分组 0~4
//注意优先级不能超过设定的组的范围!否则会有意想不到的错误
//组划分:
//组0:0位抢占优先级,4位响应优先级
//组1:1位抢占优先级,3位响应优先级
//组2:2位抢占优先级,2位响应优先级
//组3:3位抢占优先级,1位响应优先级
//组4:4位抢占优先级,0位响应优先级
//NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先	   
//void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
//{ 
//	u32 temp;	
//	MY_NVIC_PriorityGroupConfig();//设置分组
//	temp = NVIC_PreemptionPriority<<(4-NVIC_Group);	  
//	temp |= NVIC_SubPriority&(0x0f>>NVIC_Group);
//	temp&=0xf;								//取低四位  
//  *(NVIC_ISER+NVIC_Channel/32) |=(1<<NVIC_Channel%32);//使能中断位(要清除的话,相反操作就OK) 	
//	*(NVIC_IP+NVIC_Channel)|=temp<<4;		//设置响应优先级和抢断优先级   
//} 

//外部中断配置函数
//只针对GPIOA~G;不包括PVD,RTC和USB唤醒这三个
//参数:
//GPIOx:0~6,代表GPIOA~G
//BITx:需要使能的位;
//TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
//该函数一次只能配置1个IO口,多个IO口,需多次调用
//该函数会自动开启对应中断,以及屏蔽线   	    
//void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
//{
//	u8 EXTADDR;
//	u8 EXTOFFSET;
//	EXTADDR=BITx/4;//得到中断寄存器组的编号
//	EXTOFFSET=(BITx%4)*4; 
//	RCC_APB2ENR|=0x01;//使能io复用时钟	
//	*(AFIO_EXTICR+EXTADDR)&=~(0x000F<<EXTOFFSET);//清除原来设置！！！
//	*(AFIO_EXTICR+EXTADDR)|=GPIOx<<EXTOFFSET;//EXTI.BITx映射到GPIOx.BITx 
//	//自动设置
//	EXTI_IMR|=1<<BITx;//  开启line BITx上的中断
//	if(TRIM&0x01)EXTI_FTSR|=1<<BITx;//line BITx上事件下降沿触发
//	if(TRIM&0x02)EXTI_RTSR|=1<<BITx;//line BITx上事件上升沿触发
//} 


//外部中断初始化程序
//初始化PB6中断输入.
#include "stm32f10x_exti.h"
//void EXTIX_Init_RegMod(void)
//{

//	
//	RCC->APB2ENR |=0x01;         //使能复用io时钟，AFIO	
//	AFIO->EXTICR[2] &= 0x00;
//	AFIO->EXTICR[2] |= 8 | 8 << 4 | 8 << 8 | 8 << 12;
//	EXTI->RTSR |= 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7;          //开启line BIT6上事件为下降沿触发模式	
//	EXTI->IMR |= 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7;           //开启line BIT6上的中断

//	// CM3 中可以有 240 对使能位／除能位，每个中断拥有一对。
//	//这 240 个对子分布在 8 对 32 位寄存器中（最后一对没有用完）。
//	//欲使能一个中断，需要写1到NVIC_ISER寄存器组的对应位中
//	*(NVIC->ISER+EXTI9_5_IRQn/32) |=(1<<(EXTI9_5_IRQn%32)); //使能对应中断位 SETENA
//	
//	//SCB_AIRCR寄存器的[10:8]3 位就是 PRIGROUP 的定义位，它的值规定了系统中
//	//有多少个抢先级中断和子优先级中断。而 STM32 只使用高 4 位 bits
//	//设置NVIC分组为2
//	SCB->AIRCR =(0x05FA0000 | NVIC_PriorityGroup_2);  //为了防止误操作（写），因此当改写这个寄存器的内容时，必须要同时向这个寄存器的高 16 位[31：16]写验证字（Register key） 0x05FA
//	*(NVIC->IP+EXTI9_5_IRQn)|=0xB<<4;		//设置抢断优先级为2,响应优先级为3 (0b1011)
//	
//}
void EXTIX_Init_StdLibMod(void)
{
	NVIC_InitTypeDef  NVIC_InitStruct;	//
	EXTI_InitTypeDef  EXTI_InitStruct;



	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);//?

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);  

	EXTI_InitStruct.EXTI_Line=EXTI_Line4;  
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;    
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;    
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;   
	EXTI_Init(& EXTI_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel=EXTI4_IRQn;    
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;  
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;  
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;     
	NVIC_Init(& NVIC_InitStruct);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource5);  

	EXTI_InitStruct.EXTI_Line=EXTI_Line5;  
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;    
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;    
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;   
	EXTI_Init(& EXTI_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;    
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;  
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;  
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;     
	NVIC_Init(& NVIC_InitStruct);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource6);  

	EXTI_InitStruct.EXTI_Line=EXTI_Line6;  
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;    
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;    
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;   
	EXTI_Init(& EXTI_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;    
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;  
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;  
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;     
	NVIC_Init(& NVIC_InitStruct);
	
	#ifdef datatube
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource7);  

	EXTI_InitStruct.EXTI_Line=EXTI_Line7;  
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;    
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;    
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;   
	EXTI_Init(& EXTI_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;    
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;  
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;  
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;     
	NVIC_Init(& NVIC_InitStruct);
	#endif
	
}





	
		
//	if (EXTI_GetITStatus(EXTI_Line6))
//	{
//		EXTI_ClearITPendingBit(EXTI_Line6);
//		temps ++ ;
//	}
