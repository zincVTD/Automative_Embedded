#ifndef SOFTWARE_UART_H
#define SOFTWARE_UART_H

#include "main.h"

// Enum cho Baud Rate
typedef enum {
    BAUD_1200 = 1200,
    BAUD_2400 = 2400,
    BAUD_4800 = 4800,
    BAUD_9600 = 9600,
    BAUD_14400 = 14400,
    BAUD_19200 = 19200,
    BAUD_38400 = 38400,
    BAUD_57600 = 57600,
    BAUD_115200 = 115200,
    BAUD_230400 = 230400,
    BAUD_460800 = 460800,
    BAUD_921600 = 921600
} BaudRate;

// Enum cho cài đặt Parity (bit chẵn/lẻ)
typedef enum {
    PARITY_NONE,  // Không kiểm tra
    PARITY_EVEN,  // Kiểm tra chẵn
    PARITY_ODD    // Kiểm tra lẻ
} ParityType;

// Enum cho số lượng bit stop
typedef enum {
    STOP_BITS_1, // 1 bit stop
    STOP_BITS_2  // 2 bit stop
} StopBitsType;

// Struct cấu hình Software UART
typedef struct {
    GPIO_TypeDef* tx_port; // Cổng GPIO cho TX
    uint16_t tx_pin;       // Chân GPIO cho TX
    GPIO_TypeDef* rx_port; // Cổng GPIO cho RX
    uint16_t rx_pin;       // Chân GPIO cho RX
    BaudRate baudrate;     // Tốc độ Baud Rate
    ParityType parity;     // Kiểm tra chẵn/lẻ
    StopBitsType stop_bits; // Số lượng bit stop
} SoftwareUART_Config;

// Hàm khởi tạo timer để tạo delay với đơn vị micro giây
static void TIM_Delay_Init(void);

// Hàm delay với đơn vị micro giây sử dụng TIM2
static void delay_us(uint32_t us);

// Khởi tạo Software UART với cấu hình
void SoftwareUART_Init(SoftwareUART_Config config);

// Truyền một ký tự qua UART
void SoftwareUART_SendChar(SoftwareUART_Config config, char c);

// Nhận một ký tự qua UART
char SoftwareUART_ReceiveChar(SoftwareUART_Config config);

// Truyền một chuỗi qua UART
void SoftwareUART_SendString(SoftwareUART_Config config, const char* str);

// Nhận một chuỗi qua UART cho đến khi gặp ký tự kết thúc hoặc số lượng ký tự nhất định
void SoftwareUART_ReceiveString(SoftwareUART_Config config, char* buffer, uint16_t maxLength);

#endif // SOFTWARE_UART_H
