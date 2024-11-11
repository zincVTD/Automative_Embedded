#include "software_uart.h"

static uint32_t bit_duration; // Biến toàn cục lưu thời gian cho một bit

// Hàm khởi tạo GPIO (nội bộ)
static void GPIO_Config(GPIO_TypeDef* port, uint16_t tx_pin, uint16_t rx_pin) {
    // Bật clock cho GPIO tương ứng
    if (port == GPIOA) RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    if (port == GPIOB) RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    if (port == GPIOC) RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    // Cấu hình TX_PIN làm Output Push-Pull
    GPIO_InitStruct.GPIO_Pin = tx_pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(port, &GPIO_InitStruct);

    // Cấu hình RX_PIN làm Input Floating
    GPIO_InitStruct.GPIO_Pin = rx_pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(port, &GPIO_InitStruct);
}

// Hàm khởi tạo timer để tạo delay với đơn vị micro giây
static void TIM_Delay_Init(void) {
    // Bật clock cho TIM2 (hoặc TIM bạn muốn dùng)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // Cấu hình TIM2 với clock base là 1MHz (1us per tick)
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF; // Giá trị lớn nhất cho bộ đếm 16 bit
    TIM_TimeBaseInitStruct.TIM_Prescaler = (SystemCoreClock / 1000000) - 1; // Đặt prescaler sao cho 1us per tick
    TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    
    // Bắt đầu đếm
    TIM_Cmd(TIM2, ENABLE);
}

// Hàm delay với đơn vị micro giây sử dụng TIM2
static void delay_us(uint32_t us) {
    TIM_SetCounter(TIM2, 0); // Đặt lại giá trị bộ đếm về 0
    while (TIM_GetCounter(TIM2) < us); // Đợi cho bộ đếm đạt đến giá trị yêu cầu
}

// Hàm khởi tạo Software UART
void SoftwareUART_Init(SoftwareUART_Config config) {
    GPIO_Config(config.tx_port, config.tx_pin, config.rx_pin); // Cấu hình GPIO
		TIM_Delay_Init(); // Cấu hình timer
	
		// Tính toán thời gian cho một bit dựa trên baud rate
		// Truyền "Baudrate"	bit trong 1000000	us
		// Truyền 		1				bit trong 		?		us
		// Tam suất
    bit_duration = (int)(1000000 / config.baudrate);
		GPIO_SetBits(config.tx_port, config.tx_pin);
		delay_us(1);
}

// Hàm truyền một ký tự qua UART
void SoftwareUART_SendChar(SoftwareUART_Config config, char c) {
    // Start bit
    GPIO_ResetBits(config.tx_port, config.tx_pin);
    delay_us(bit_duration);

    // Truyền các bit dữ liệu (LSB trước)
    for (int i = 0; i < 8; i++) {
        if (c & (1 << i)) {
            GPIO_SetBits(config.tx_port, config.tx_pin);
        } else {
            GPIO_ResetBits(config.tx_port, config.tx_pin);
        }
        delay_us(bit_duration);
    }

    // Bit kiểm tra chẵn lẻ nếu có
    if (config.parity != PARITY_NONE) {
        uint8_t parity_bit = 0;
        for (int i = 0; i < 8; i++) {
            parity_bit ^= (c >> i) & 1; // Tính toán parity
        }
        if (config.parity == PARITY_EVEN) {
            parity_bit = !parity_bit; // Đảo bit nếu là parity chẵn
        }
        if (parity_bit) {
            GPIO_SetBits(config.tx_port, config.tx_pin);
        } else {
            GPIO_ResetBits(config.tx_port, config.tx_pin);
        }
        delay_us(bit_duration);
    }

    // Stop bit
    GPIO_SetBits(config.tx_port, config.tx_pin);
    delay_us(bit_duration);

    // Thêm stop bit nếu cấu hình là 2 bit stop
    if (config.stop_bits == STOP_BITS_2) {
        delay_us(bit_duration);
    }
}

// Hàm nhận một ký tự qua UART
char SoftwareUART_ReceiveChar(SoftwareUART_Config config) {
    char c = 0;

    // Đợi Start bit
    while (GPIO_ReadInputDataBit(config.rx_port, config.rx_pin) == 1);

    // Chờ một nửa thời gian bit để vào giữa start bit
    delay_us(bit_duration + bit_duration / 2);

    // Đọc các bit dữ liệu (LSB trước)
    for (int i = 0; i < 8; i++) {
				
        if (GPIO_ReadInputDataBit(config.rx_port, config.rx_pin)) {
            c |= (1 << i);
        }
				delay_us(bit_duration); // Đợi đến giữa bit tiếp theo
    }

    // Kiểm tra parity nếu có
    if (config.parity != PARITY_NONE) {
        delay_us(bit_duration); // Đợi bit chẵn lẻ
    }

    // Đợi Stop bit
    //delay_us(bit_duration);

    // Đợi bit stop thứ hai nếu cấu hình là 2 bit stop
    if (config.stop_bits == STOP_BITS_2) {
        delay_us(bit_duration);
    }

    return c;
}

// Hàm truyền một chuỗi qua UART
void SoftwareUART_SendString(SoftwareUART_Config config, const char* str) {
    while (*str) { // Lặp cho đến khi gặp ký tự kết thúc chuỗi '\0'
        SoftwareUART_SendChar(config, *str++);
    }
}

// Hàm nhận một chuỗi qua UART cho đến khi gặp ký tự kết thúc hoặc số lượng ký tự nhất định
void SoftwareUART_ReceiveString(SoftwareUART_Config config, char* buffer, uint16_t maxLength) {
    uint16_t count = 0;
    while (count < maxLength - 1) { // Dừng trước khi đầy buffer
        char c = SoftwareUART_ReceiveChar(config);
        if (c == '\n' || c == '\r') { // Dừng khi gặp ký tự xuống dòng hoặc kết thúc chuỗi
            break;
        }
        buffer[count++] = c;
    }
    buffer[count] = '\0'; // Kết thúc chuỗi
}
