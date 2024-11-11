#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_exti.h"             // Keil::Device:StdPeriph Drivers:EXTI
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART

uint32_t count = 0;

void RCC_Config(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC | RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void GPIO_Config(){
	GPIO_InitTypeDef GPIOInitStruct;
	
	//EXTI
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIOInitStruct);
	
	//LED PC13
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIOInitStruct);
	
	//Tx
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIOInitStruct);
	
	//Rx
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIOInitStruct);
}

void TIM_Config(){
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200-1;
	TIM_TimeBaseInitStruct.TIM_Period = 10-1;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

void UART_Config(){
	USART_InitTypeDef UART_InitStruct;
	UART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	UART_InitStruct.USART_BaudRate = 9600;
	UART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	UART_InitStruct.USART_WordLength = USART_WordLength_8b;
	UART_InitStruct.USART_StopBits = USART_StopBits_1;
	UART_InitStruct.USART_Parity = USART_Parity_No;

	USART_Init(USART1, &UART_InitStruct);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

void EXTI_Config(){
	EXTI_InitTypeDef EXTIInitStruct;

	EXTIInitStruct.EXTI_Line = EXTI_Line0;
	EXTIInitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTIInitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTIInitStruct.EXTI_LineCmd = ENABLE;
	
	EXTI_Init(&EXTIInitStruct);
}

void NVIC_Config(){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStruct;

	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void EXTI0_IRQHandler(){	
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		 count++;
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}

void delay(uint32_t time){
	count = 0; 
	while(count < time){
	//Wait
	}
}
void TIM2_IRQHandler(){
	if(TIM_GetITStatus(TIM2, TIM_IT_Update)){
		count++;

		// Clears the TIM2 interrupt pending bit
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

void USART1_IRQHandler(){
	uint8_t data = 0x00;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
		while(!USART_GetFlagStatus(USART1, USART_FLAG_RXNE));
		data = USART_ReceiveData(USART1);
		if(USART_GetITStatus(USART1, USART_IT_TXE) == RESET){
			USART_SendData(USART1, data);
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		}
	}
	USART_ClearITPendingBit (USART1, USART_IT_RXNE);
}

int main(){
	RCC_Config();
	GPIO_Config();
	TIM_Config();
	UART_Config();
	EXTI_Config();
	NVIC_Config();
	while(1){
		GPIO_SetBits(GPIOC, GPIO_Pin_13);
		delay(1000);
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		delay(1000);
	}
}
