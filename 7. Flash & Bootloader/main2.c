#include "stm32f10x.h"                  // Device header
#include "stm32f10x_flash.h"            // Keil::Device:StdPeriph Drivers:Flash
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC

#define PAGE4_ADDR 0x08001000
#define PAGE0_ADDR 0x08000000
#define PAGE2_ADDR 0x08000400
#define ADDR_STR_BLINK 0x08008000

uint32_t count = 0;

void Boot() {
    // Thi?t l?p l?i h? th?ng clock
    RCC_DeInit();
    
    // Vô hi?u hóa các l?i ng?t d? tránh l?i trong quá trình chuy?n giao
    SCB->SHCSR &= ~(SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk);
    
    // C?p nh?t Stack Pointer (SP) t? b?ng vector ng?t c?a chuong trình chính
    __set_MSP(*(__IO uint32_t*)(ADDR_STR_BLINK));
    
    // C?p nh?t thanh ghi SCB->VTOR d? tr? d?n b?ng vector ng?t c?a chuong trình chính
    //SCB->VTOR = ADDR_STR_BLINK;
		count++;

    // L?y d?a ch? Reset Handler c?a chuong trình chính
    uint32_t JumpAddress = *(__IO uint32_t*)(ADDR_STR_BLINK + 4);
    
    // T?o con tr? hàm d?n Reset Handler
    void (*reset_handler)(void) = (void (*)(void)) JumpAddress;
    
    // Nh?y vào Reset Handler c?a chuong trình chính
    reset_handler();
}


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
	*((uint32_t *)PAGE4_ADDR + 32) = data;
	FLASH->CR &= ~FLASH_CR_PG;
	
	while(FLASH->SR & FLASH_SR_BSY);
	FLASH_Lock();
}

int main(){
	Boot();
}

