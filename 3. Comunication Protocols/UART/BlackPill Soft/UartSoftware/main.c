#include "software_uart.h"

// Hàm delay với đơn vị micro giây sử dụng TIM2
static void delay_us(uint32_t us) {
    for(int i = 0; i < us; i++);
}

char data;
int main(void) {
    // Cấu hình Software UART với chân PA9, PA10, baud rate 9600, không có parity, và 1 bit stop
    SoftwareUART_Config uart_config = {
        .tx_port = GPIOA,
        .tx_pin = GPIO_Pin_9,
        .rx_port = GPIOA,
        .rx_pin = GPIO_Pin_10,
        .baudrate = BAUD_115200,
        .parity = PARITY_NONE,
        .stop_bits = STOP_BITS_1 // 1 bit stop
    };
		
    SoftwareUART_Init(uart_config); // Khởi tạo UART
		
		//SoftwareUART_SendString(uart_config, "Hello, World!\n");
    char receiveBuffer[100]; // Buffer để nhận chuỗi

    while (1) {
        SoftwareUART_SendString(uart_config, "Hello\n"); // Truyền chuỗi
        //delay_us(100000); // Đợi 100ms
        
        //SoftwareUART_ReceiveString(uart_config, receiveBuffer, sizeof(receiveBuffer)); // Nhận chuỗi
        //SoftwareUART_SendString(uart_config, "Received: "); // Truyền chuỗi xác nhận
        //SoftwareUART_SendString(uart_config, receiveBuffer); // Truyền lại chuỗi đã nhận
        //SoftwareUART_SendChar(uart_config, '\n'); // Truyền ký tự xuống dòng
			//SoftwareUART_SendChar(uart_config, 'a');
			//SoftwareUART_SendChar(uart_config, '\n');
			delay_us(10000000);
    }
}
