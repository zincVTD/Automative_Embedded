# Automative_Embedded
## Bài 1: GPIO
Cấu hình, điều khiển các ngoại vi sử dụng thư viện **SPL (Standard Peripheral Firmware Library)**. Thư viện SPL cung cấp các hàm và các định nghĩa giúp việc cấu hình và sử dụng ngoại vi dễ dàng và rõ ràng. Ở trong thư viện này, các cấu hình được chia thành các trường và định nghĩa bằng các cấu trúc như struct và enum.
Trước khi sử dụng các ngoại vi, ta phải:
- Cấp clock cho bus chứa ngoại vi. Hàm "RCC_\<bus\>ClockCmd" giúp cung cấp clock cho ngoại vi nằm trên đường bus tương ứng trong tên hàm. Hàm này chứa 2 tham số:
    - Tham số đầu tiên: ngoại vi trên đường bus tương ứng. Tham số này có định dạng chung như sau: "RCC_\<bus\>Periph_\<tên ngoại vi\>"
    - Tham thứ hai: có cho phép cấp clock cho ngoại vi này không. Có là **ENABLE**, không là **DISABLE**.
    ```
        RCC_APB2ClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    ```
        RCC_APB1ClockCmd(RCC_APB1Periph_TIM2, DISABLE);
- Cấu hình cho ngoại vi. Các tham số cấu hình cho GPIO được tổ chức trong 1 struct "GPIO_InitTypeDef" với các tham số sau:
    - GPIO_Pin: chân cần được cấu hình. Có định dạng là: "GPIO_Pin_\<chân cần cấu hình\>". Có thể cấu hình nhiều chân với cùng chế độ và tốc độ giống nhau bằng cách sử dụng toán tử OR nhiều định dạng chân lại.
    - GPIO_Mode: chế độ muốn cấu hình bao gồm các giá trị sau:
    ```
        typedef enum {
            GPIO_Mode_AIN = 0x00,            //Analog Input
            GPIO_Mode_IN_FLOATING = 0x04,    //Input bình thường
            GPIO_Mode_IPD = 0x28,            //Input có điện trở kéo xuống    
            GPIO_Mode_IPU = 0x48,            //Input có điện trở kéo lên
            GPIO_Mode_Out_OD = 0x14,         //Output dạng open-drain
            GPIO_Mode_Out_PP = 0x10,         //Output dạng push-pull
            GPIO_Mode_AF_OD = 0x1C,          //Chế độ ngoại vi khác dạng open-drain
            GPIO_Mode_AF_PP = 0x18           //Chế độ ngoại vi khác dạng push-pull
        } GPIOMode_TypeDef;
    ```
    - GPIO_Speed: tốc độ đáp ứng của chân bao gồm 3 tốc độ 2MHz, 10MHz và 50MHz. Có định dạng: "GPIO_Speed_\<tốc độ\>".
- Để gắn các giá trị muốn cấu hình vào các thanh ghi thì ta sử dụng hàm "GPIO_Init" có 2 tham số:
    - Tham số đầu là tên ngoại vi muốn cấu hình
    - Tham số thứ hai là con trỏ đến struct "GPIO_InitTypeDef"
```
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
    GPIO_Init(GPIOC, &GPIO_InitStruct);
```
Để điều khiển các GPIO ta có các hàm sau:
```
    uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);            \\Đọc giá trị 1 bit trong cổng GPIO được cấu hình là INPUT (IDR), có thể đọc nhiều pin nhờ toán tử OR
    uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);                                 \\Đọc giá trị nguyên cổng GPIO được cấu hình là INPUT (IDR)
    uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);           \\Đọc giá trị 1 bit trong cổng GPIO được cấu hình là OUTPUT (ODR), có thể đọc nhiều pin nhờ toán tử OR
    uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);                                \\Đọc giá trị nguyên cổng GPIO được cấu hình là OUTPUT (ODR)
    void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);                        \\Cho giá trị 1 bit trong cổng GPIO = 1, có thể ghi nhiều pin nhờ toán tử OR
    void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);                      \\Cho giá trị 1 bit trong cổng GPIO = 0, có thể ghi nhiều pin nhờ toán tử OR
    void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);     \\Ghi giá trị "BitVal" vào 1 bit trong cổng GPIO, có thể ghi nhiều pin nhờ toán tử OR
    void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);                           \\Ghi giá trị "PortVal" vào nguyên cổng GPIO
```
## Bài 2: Ngắt và Timer
### Ngắt
### Timer
## Bài 3: Giao thức truyền nhận dữ liệu
### SPI
### I2C
### UART
