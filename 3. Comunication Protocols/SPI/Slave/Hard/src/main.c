#include "main.h"

#define SPI_SS GPIO_Pin_4
#define SPI_SCK GPIO_Pin_5
#define SPI_MISO GPIO_Pin_6
#define SPI_MOSI GPIO_Pin_7
#define SPI_GPIO GPIOA

void RCC_Config(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9 | RCC_APB2Periph_SPI1, ENABLE);
}

void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin 	= SPI_SS | SPI_SCK | SPI_MOSI | SPI_MISO;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
}

void TIM_Config(){
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStruct.TIM_Prescaler = 16000 - 1;
	TIM_InitStruct.TIM_Period = 0xFFFF;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM9, &TIM_InitStruct);
	TIM_Cmd(TIM9, ENABLE);
}

void SPI_Config(){
	SPI_InitTypeDef SPI_InitStruct;
	
	SPI_InitStruct.SPI_Mode = SPI_Mode_Slave;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	
	SPI_Init(SPI1, &SPI_InitStruct);
	SPI_Cmd(SPI1, ENABLE);
}

void delay_ms(uint32_t time){
	TIM_SetCounter(TIM9, 0);
	while(TIM_GetCounter(TIM9) < time){}
}

uint8_t SPI_Receive1Byte(void){
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET){}
		uint8_t temp = (uint8_t)SPI_I2S_ReceiveData(SPI1);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){}
    return temp;
}
uint8_t data;
int main(void)
{
	RCC_Config();
	GPIO_Config();
	TIM_Config();
	SPI_Config();
  while(1)
  {
		while(GPIO_ReadInputDataBit(GPIOA, SPI_SS) == 1){}
		if(GPIO_ReadInputDataBit(GPIOA, SPI_SS) == 0){
			data = SPI_Receive1Byte();
		}
  }
}
