#include <setjmp.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#define BLINKLED_MODE 1
#define CHASELED_MODE 2
#define OFF_LED_MODE 3
jmp_buf buff;

void delay(uint32_t time){
	for(int i;i<time;i++){}
}

uint8_t bt1(){
    return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
}


uint8_t bt2(){
    return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
}


uint8_t bt3(){
    return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2);
}

void delay_And_Check(){
    //delay moi 1 ms
    for(int i =0; i< 50000; i++){//
        if(bt1()==0){
            while(bt1()==0);
            longjmp(buff, BLINKLED_MODE);
        }
        if(bt2()==0){
            while(bt2()==0);
            longjmp(buff, CHASELED_MODE);
        }
        if(bt3()==0){
            while(bt3()==0);
            longjmp(buff, OFF_LED_MODE);
        }
        delay(100);
    }
}

void offLed(){
    GPIO_Write(GPIOC, 0x0000);
}

void blinkLed(uint8_t loop){
    offLed();
    int i;
    for(i=0; i<loop; i++){
    GPIO_SetBits(GPIOC, GPIO_Pin_13); // Ghi 1 ra PC13
    delay_And_Check();
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);// Ghi 0 ra PC13
    delay_And_Check();
    }
}

void chaseLed(uint8_t loop){
    uint16_t Ledval ;
    offLed();
    for(int j=0; j<loop; j++ ){
        Ledval = 0x0010;
        for(int i =0; i<5; i++)
        {
            Ledval = Ledval<<1;
            GPIO_Write(GPIOC, Ledval);
            delay_And_Check();
        }
    }
}

void RCC_Config(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
}

void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
}

int main(){
	RCC_Config();
	GPIO_Config();
	int mode = setjmp(buff);
	while(1){
		switch (mode){
			case BLINKLED_MODE:
				blinkLed(5);
				mode = 0;
				break;
			case CHASELED_MODE:
				chaseLed(2);
				mode =0;
				break;
			case OFF_LED_MODE:
				offLed();
				mode = 0;
				break;
			default :
				if(bt1()==0){
					while(bt1()==0){};
					longjmp(buff, BLINKLED_MODE);
				}
				if(bt2()==0){
					while(bt2()==0){};
					longjmp(buff, CHASELED_MODE);
				}
				if(bt3()==0){
					while(bt3()==0){};
					longjmp(buff, OFF_LED_MODE);
				}
				break;
		}
	}
}
