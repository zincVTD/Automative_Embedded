#include "main.h"
#include "software_uart.h"

char data;
int main(void) {
    // C?u hình Software UART v?i chân PA9, PA10, baud rate 9600, không có parity, và 1 bit stop
    SoftwareUART_Config uart_config = {
        .tx_port = GPIOA,
        .tx_pin = GPIO_Pin_9,
        .rx_port = GPIOA,
        .rx_pin = GPIO_Pin_10,
        .baudrate = BAUD_115200,
        .parity = PARITY_NONE,
        .stop_bits = STOP_BITS_1 // 1 bit stop
    };
		
    SoftwareUART_Init(uart_config); // Kh?i t?o UART
		
		//SoftwareUART_SendString(uart_config, "Hello, World!\n");
    char receiveBuffer[100]; // Buffer d? nh?n chu?i

    while (1) {
        //SoftwareUART_SendString(uart_config, "Hello\n"); // Truy?n chu?i
        //delay_us(100000); // Ð?i 100ms
        
        SoftwareUART_ReceiveString(uart_config, receiveBuffer, sizeof(receiveBuffer)); // Nh?n chu?i
        //SoftwareUART_SendString(uart_config, "Received: "); // Truy?n chu?i xác nh?n
        //SoftwareUART_SendString(uart_config, receiveBuffer); // Truy?n l?i chu?i dã nh?n
        //SoftwareUART_SendChar(uart_config, '\n'); // Truy?n ký t? xu?ng dòng
			//SoftwareUART_SendChar(uart_config, 'a');
			//SoftwareUART_SendChar(uart_config, '\n');
			//delay_us(10000000);
    }
}
