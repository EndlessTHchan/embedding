#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "misc.h"
//使能GPIO的过程略


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//开启AFIO时钟
	void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource); //将GPIO和EXTI建立连接（映射）
void NVIC_config(){
	//下为NVIC的配置过程
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; //EXTIx 中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=m; //抢占优先级	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = (5-m); //响应优先级	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道使能	
	NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化NVIC寄存器
	
	//下为初始化EXTI，选择触发方式
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line=EXTI_Line0; //配置EXTI中断线
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt; //配置EXTI模式为中断
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising; //配置EXTI触发方式
	EXTI_InitStructure.EXTI_LineCmd=ENABLE; //使能EXTI中断线
	EXTI_Init(&EXTI_InitStructure); //根据指定的参数初始化EXTI寄存器
	
}
	//下为编写EXTI中断服务函数
	
	void EXTI0_IQRHandler(void)
	{
		if(EXTI_GetITStatus(EXTI_Line0) == 1) //读取中断标志位，确定中断真的发生
		{
		//此处编写中断函数
		}
		EXTI_ClearITPendingBit(EXTI_Line0); //清除中断标志位
	}
