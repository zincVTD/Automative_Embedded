#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM

void RCC_Config(){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void GPIO_Config(){
		GPIO_InitTypeDef GPIO_InitStruct;
	
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	
    GPIO_Init(GPIOA, &GPIO_InitStruct);
	
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void UART_Config(){
    USART_InitTypeDef USARTInitStruct;
    USARTInitStruct.USART_BaudRate = 115200;
    USARTInitStruct.USART_WordLength = USART_WordLength_8b;
    USARTInitStruct.USART_StopBits = USART_StopBits_1;
    USARTInitStruct.USART_Parity = USART_Parity_No;
    USARTInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USARTInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USARTInitStruct);
    USART_Cmd(USART1, ENABLE);
}

void TIM_Config(){
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	//72MHz
	TIM_InitStruct.TIM_Prescaler = 7200 - 1;					
	TIM_InitStruct.TIM_Period = 0xFFFF;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
	TIM_Cmd(TIM2, ENABLE);
}

void delay_ms(uint32_t timedelay)
{
	TIM_SetCounter(TIM2, 0);
	while(TIM_GetCounter(TIM2) < timedelay * 10){}
}

uint8_t USART1_ReceiveByte(void) {
	uint8_t temp = 0x00;
    // Wait until data is received (RXNE flag is set)
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

    // Read the received data
	temp = USART_ReceiveData(USART1);
	return temp;
}

void USART1_TransmitByte(uint8_t byte) {
    // Wait until the transmit data register is empty (TXE flag is set)
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

    // Transmit the byte
    USART_SendData(USART1, byte);

    // Wait until transmission is complete (TC flag is set)
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

char data[] = {'d', 'a', 'n', 'h', 'z', 'n', '!'};
char buffer[100];
int main(){
	RCC_Config();
	GPIO_Config();
	TIM_Config();
	UART_Config();
	while(1){
		for(int i = 0; i < 7; i++){
			USART1_TransmitByte(data[i]);
			delay_ms(1000);
		}
		USART1_TransmitByte('\n');
//		for(int i = 0; i < 100; i++){
//			buffer[i] = USART1_ReceiveByte();
//		}
	}
}
