#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM
#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C

#define I2C_SCL 	GPIO_Pin_6
#define I2C_SDA		GPIO_Pin_7

#define I2C_GPIO 	GPIOB

#define WRITE_SDA_0 	GPIO_ResetBits(I2C_GPIO, I2C_SDA)
#define WRITE_SDA_1 	GPIO_SetBits(I2C_GPIO, I2C_SDA)
#define WRITE_SCL_0 	GPIO_ResetBits(I2C_GPIO, I2C_SCL)
#define WRITE_SCL_1 	GPIO_SetBits(I2C_GPIO, I2C_SCL)
#define READ_SDA_VAL 	GPIO_ReadInputDataBit(I2C_GPIO, I2C_SDA)

void delay_us(uint32_t time){}

typedef enum {
	NOT_OK, OK
} status;

typedef enum{
	NACK, ACK
} ACK_Bit;

status I2C_Master_Write(uint8_t u8Data){	
	status stRet;
	for(int i = 0; i < 8; i++){		//Write byte data.
		if (u8Data & 0x80) { //0b1000 0000
			WRITE_SDA_1;
		} else {
			WRITE_SDA_0;
		}
		delay_us(3);
		
		// Clock
		WRITE_SCL_1;
		delay_us(5);
		WRITE_SCL_0;
		delay_us(2);
		
		// Dich 1 bit
		u8Data <<= 1;
	}
	
	WRITE_SDA_1;					
	delay_us(3);
	WRITE_SCL_1;					// Receive ACK
	delay_us(3);
	
	if (READ_SDA_VAL) {	
		stRet = NOT_OK;				
	} else {
		stRet = OK;					
	}

	delay_us(2);
	WRITE_SCL_0;
	delay_us(5);
	
	return stRet;
}

uint8_t I2C_Master_Read(ACK_Bit _ACK){	
	uint8_t i;						
	uint8_t u8Ret = 0x00;
	WRITE_SDA_1;
	delay_us(3);	
	for (i = 0; i < 8; ++i) {
		u8Ret <<= 1; //0b0000 0000
		WRITE_SCL_1;
		delay_us(3);
		if (READ_SDA_VAL) {
			u8Ret |= 0x01; //0b0000 0001
		}
		delay_us(2);
		WRITE_SCL_0;
		delay_us(5);
	}
	if (_ACK) {	
		WRITE_SDA_0;
	} else {
		WRITE_SDA_1;
	}
	delay_us(3);
	
	WRITE_SCL_1;
	delay_us(5);
	WRITE_SCL_0;
	delay_us(5);

	return u8Ret;
}

void I2C_Config(){
	I2C_InitTypeDef I2C_InitStruct;
	
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_ClockSpeed = 400000;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	
	I2C_Init(I2C1, &I2C_InitStruct);
	I2C_Cmd(I2C1, ENABLE);
}

int main(){
	while(1){
	
	}
}
