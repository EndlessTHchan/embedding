#include "sys.h"
#include "stm32f10x.h"
#define 	TIM2_base 	0x40000000
#define 	TIM2_CR 	(*((volatile unsigned int *)(TIM2_base + 0x00)))
extern void number_set(int num,int index);

#define My_BASIC_TIM                  TIM6
#define My_RCC_TIM6_EN     (uint32_t)0x00000010
#define My_RCC_TIM3_EN     (uint32_t)0x00000002
#define My_RCC_TIM2_EN     (uint32_t)0x00000001
#define My_BASIC_UIF       (uint32_t)0x00000001           // Update Interrupt Flag
#define My_BSIC_UIE        (uint32_t)0x00000001           // Update Interrupt Enable
#define My_BASIC_CEN       (uint32_t)0x00000001           // Counter Enable


void Base_Timer6_Config(uint16_t arr, uint16_t psc)
{
	/* 定时器时钟使能 */
	RCC->APB1ENR |= My_RCC_TIM6_EN;
	
	/* 设置自动装载值 = arr */
	My_BASIC_TIM->ARR = arr-1;
	
	/* 设置预分频系数 = psc*/
	My_BASIC_TIM->PSC = psc-1;
	
	/* 清除中断标志位 */
	My_BASIC_TIM->SR &= ~My_BASIC_UIF;
	
	/* TIM6抢占优先级 = 0，响应优先级 = 0*/
	NVIC->IP[TIM6_IRQn] = (uint8_t)0x00; 

	/* 使能基本定时器中断 */
	My_BASIC_TIM->DIER |= My_BSIC_UIE;
	
	/*使能基本定时器中断*/
    NVIC_EnableIRQ(TIM6_IRQn);
	
	/* 开启定时器 */
	My_BASIC_TIM->CR1 |= My_BASIC_CEN;
}

void Base_Timer2_Config(uint16_t arr, uint16_t psc)
{
	/* 定时器时钟使能 */
	RCC->APB1ENR |= My_RCC_TIM2_EN;
	
	/* 设置自动装载值 = arr */
	TIM2->ARR = arr-1;
	
	/* 设置预分频系数 = psc*/
	TIM2->PSC = psc-1;
	
	/* 清除中断标志位 */
	TIM2->SR &= ~My_BASIC_UIF;
	
	/* TIM6抢占优先级 = 0，响应优先级 = 0*/
	NVIC->IP[TIM2_IRQn] = (uint8_t)0x01; 

	/* 使能基本定时器中断 */
	TIM2->DIER |= My_BSIC_UIE;
	
	/*使能基本定时器中断*/
    NVIC_EnableIRQ(TIM2_IRQn);
	
	/* 开启定时器 */
	TIM2->CR1 |= My_BASIC_CEN;
}

void Base_Timer3_Config(uint16_t arr, uint16_t psc)
{
	/* 定时器时钟使能 */
	RCC->APB1ENR |= My_RCC_TIM3_EN;
	
	/* 设置自动装载值 = arr */
	TIM3->ARR = arr-1;
	
	/* 设置预分频系数 = psc*/
	TIM3->PSC = psc-1;
	
	/* 清除中断标志位 */
	TIM3->SR &= ~My_BASIC_UIF;
	
	/* TIM6抢占优先级 = 0，响应优先级 = 0*/
	NVIC->IP[TIM3_IRQn] = (uint8_t)0x01; 

	/* 使能基本定时器中断 */
	TIM3->DIER |= My_BSIC_UIE;
	
	/*使能基本定时器中断*/
    NVIC_EnableIRQ(TIM3_IRQn);
	
	/* 关闭定时器 */
	TIM3->CR1 &= (0xFFFF - 0x0001);
}
