#include "stm32f10x.h"                  // Device header
#include "stm32f10x_flash.h"            // Keil::Device:StdPeriph Drivers:Flash
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC

#define PAGE4_ADDR 0x08001000
#define PAGE0_ADDR 0x08000000
#define ADDR_STR_BLINK 0x08008000

void Flash_Erase(uint32_t pageAddress){
	FLASH_Unlock();
	while(FLASH_GetFlagStatus(FLASH_FLAG_BSY));
	
	FLASH->CR |= FLASH_CR_PER;	//ENABLE erase page
	FLASH->AR = pageAddress;
	FLASH->CR |= FLASH_CR_STRT;
	
	while(FLASH->SR & FLASH_SR_BSY);
	
	FLASH->CR &= ~FLASH_CR_PER;	//DISABLE erase page
	FLASH_Lock();
}

void Flash_Write(uint16_t data){
	FLASH_Unlock();
	while(FLASH->SR & FLASH_SR_BSY);
	
	FLASH->CR |= FLASH_CR_PG;
	*((uint32_t *)PAGE0_ADDR) = data;
	FLASH->CR &= ~FLASH_CR_PG;
	
	while(FLASH->SR & FLASH_SR_BSY);
	FLASH_Lock();
}

int main(){
	Flash_Erase(PAGE4_ADDR);
	Flash_Write(0xAB);
	while(1){}
}

