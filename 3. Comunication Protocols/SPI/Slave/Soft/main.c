#include "main.h"

#define SPI_CS_Pin GPIO_Pin_4
#define SPI_SCK_Pin GPIO_Pin_5
#define SPI_MISO_Pin GPIO_Pin_6
#define SPI_MOSI_Pin GPIO_Pin_7
#define SPI_GPIO GPIOA
#define SPI_RCC RCC_AHB1Periph_GPIOA

void RCC_Config(){
	RCC_AHB1PeriphClockCmd(SPI_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = SPI_SCK_Pin | SPI_MOSI_Pin | SPI_CS_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(SPI_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SPI_MISO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(SPI_GPIO, &GPIO_InitStructure);
}

void TIM_Config(){
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStruct.TIM_Prescaler = 16000 - 1;
	TIM_InitStruct.TIM_Period = 0xFFFF;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
	TIM_Cmd(TIM2, ENABLE);
}

void delay_ms(uint32_t time){
	TIM_SetCounter(TIM2, 0);
	while(TIM_GetCounter(TIM2) < time){}
}

uint8_t SPI_Slave_Receive(void){
	uint8_t dataReceive = 0x00;	//0b0000 0000
	uint8_t temp = 0x00;
	while(GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin));
	while(!GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin));
	for(int i = 0; i < 8; i++){ 
		if(GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin)){
			while (GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin)){
				temp = GPIO_ReadInputDataBit(SPI_GPIO, SPI_MOSI_Pin);
			}
			dataReceive <<= 1;
			dataReceive |= temp;
    }
		while(!GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin));
	}
	while(!GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin));
	return dataReceive;
}

uint8_t DataReceive;
int main(){
	RCC_Config();
	GPIO_Config();
	TIM_Config();
	while(1){	
		DataReceive = SPI_Slave_Receive();
	}
}
