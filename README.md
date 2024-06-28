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
**Ngắt** là sự kiện khẩn cấp xảy ra bên trong hoặc bên ngoài MCU, khiến MCU phải dừng chương trình chính và thực thi chương trình ngắt.
Mỗi ngắt có 1 **trình phục vụ ngắt** nằm trong địa chỉ cố định trong bộ nhớ **(vector ngắt)**, khi có điều kiện ngắt xảy ra, MCU thực thi trình phục vụ ngắt.
Mỗi ngắt có 1 **độ ưu tiên**, ngắt nào có độ ưu tiên cao hơn thì sẽ được thực thi trước nếu 2 ngắt xảy ra đồng thời.

Bên trong quá trình ngắt:
- Thanh ghi **PC (Program Counter)** là thanh ghi luôn trỏ tới lệnh kế tiếp lệnh đang thực hiện trong chương trình.
- Khi có ngắt xảy ra, hệ thống sẽ lưu giá trị hiện tại của thanh ghi PC vào **Stack Pointer**. Thanh ghi PC sẽ trỏ tới lệnh đầu tiên của trình phục vụ ngắt và thực thi chúng, chương trình tiếp tục thực thi lệnh hiện tại cho tới khi hoàn thành.
- Nếu đang thực thi trình phục vụ ngắt mà có 1 ngắt có độ ưu tiên cao hơn xảy ra thì hệ thống sẽ thực hiện lại *Bước 2*.
- Khi đang thực thi lệnh cuối của trình phục vụ ngắt thì thanh ghi PC sẽ trỏ tới lệnh đã được lưu mới nhất vào Stack Pointer (Stack Pointer hoạt động dạng *LIFO*) và sẽ thực thi chương trình ở đó.

#### Ngắt ngoài
Xảy ra khi có thay đổi điện áp trên các chân GPIO được cấu hình làm ngõ vào ngắt. Có 4 loại ngắt ngoài:
- LOW: kích hoạt ngắt liên tục khi chân ở mức thấp.
- HIGH: Kích hoạt liên tục khi chân ở mức cao.
- RISING: Kích hoạt khi trạng thái trên chân chuyển từ thấp lên cao (cạnh lên).
- FALLING: Kích hoạt khi trạng thái trên chân chuyển từ cao xuống thấp (cạnh xuống).

#### Ngắt Timer
Ngắt Timer xảy ra khi giá trị trong thanh ghi đếm của timer tràn. Giá trị tràn được xác định bởi giá trị cụ thể trong thanh ghi đếm của timer. Vì đây là ngắt nội trong MCU, nên phải reset giá trị thanh ghi timer để có thể tạo được ngắt tiếp theo.
Ngắt truyền thông
Ngắt truyền thông xảy ra khi có sự kiện truyền/nhận dữ liệu giữa MCU với các thiết bị bên ngoài hay với MCU. Ngắt này sử dụng cho nhiều phương thức như SPI, I2C, UART,... nhằm đảm bảo việc truyền nhận chính xác.
Ví dụ: Khi bên truyền bắt đầu truyền thì bên nhận sẽ lập tức dừng chương trình hiện tại để thực hiện nhận dữ liệu từ bên truyền. Nhận xong sẽ tiếp tục thực hiện chương trình chính.

### Timer
**Timer** có vai trò đếm lên hoặc xuống mỗi chu kỳ clock. Xung clock có thể từ rất nhiều nguồn khác nhau, ví dụ như khi gặp cạnh lên hoặc cạnh xuống khi nhấn nút,... 

Sau khi cấp clock cho timer, ta tiến hành cấu hình cho timer. Ta sử dụng chế độ cơ bản của timer nên ta sẽ cấu hình struct "TIM_TimeBaseInitTypeDef" gồm các biến sau:
- TIM_ClockDivision: chia tần số gốc của CPU là 72MHz để cấp cho timer, có 3 bộ chia là chia 1, 2, 4. Định dạng cho giá trị này là: TIM_CKD_DIV\<số chia\>
- TIM_Prescaler: xác định bao nhiêu dao động thì timer sẽ đếm lên 1 lần. Vì là kiểu uint16_t nên giá trị mà biến này chấp nhận là từ 0 đến 65535.
- TIM_Period: xác định giá trị mà timer cần phải đếm tới (hoặc từ giá trị này đếm xuống 0) rồi mới tràn. Vì là kiểu uint16_t nên giá trị mà biến này chấp nhận là từ 0 đến 65535.
- TIM_CounterMode: xác định chế độ đếm. Có 2 chế độ thường dùng là đếm lên "TIM_CounterMode_Up" và đếm xuống "TIM_CounterMode_Down"

Để ghi các giá trị đã cấu hình vào các thanh ghi của timer thì ta sử dụng hàm "TIM_TimeBaseInit" với 2 tham số:
- Tham số đầu là timer ta sử dụng. STM32F103C8 có 7 timer bao gồm 1 systick timer, 2 watchdog timer và còn 4 timer cho các chức năng mong muốn.
- Tham số thứ hai là con trỏ đến struct "TIM_TimeBaseInitTypeDef" đã cài đặt ở trên.

Sau khi đã hoàn thành cấu hình cho timer thì ta phải cho phép timer hoạt động bằng hàm "TIM_Cmd" với 2 tham số:
- Tham số đầu tiên: timer mà ta sử dụng.
- Tham thứ hai: có cho phép cấp clock cho ngoại vi này không. Có là **ENABLE**, không là **DISABLE**.

```
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200-1;
	TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	TIM_Cmd(TIM2, ENABLE);
```
## Bài 3: Giao thức truyền nhận dữ liệu
- Việc truyền dữ liệu thực chất là truyền nhận các tín hiệu điện áp biểu diễn cho các bit, điện áp được truyền nhận trên các chân của MCU.
- Các dữ liệu cần truyền đi sẽ được mã hoá thành các chuỗi nhị phân rồi mới truyền đi dưới dạng các mức điện áp.
- Các chuẩn giao tiếp ở dưới là truyền nối tiếp, các bit được truyền lần lượt trên 1 đường dây. Bên nhận sẽ nhận các chuỗi mức điện áp và sẽ mã hoá thành các chuỗi bit và xử lý chúng.
- Các chuẩn giao tiếp ra đời để giải quyết vấn đề phân biệt các bit liên tiếp trong chuỗi.
### SPI
**SPI (Serial Peripheral Interface)** là chuẩn giao tiếp nối tiếp đồng bộ. SPI hoạt động chế độ song công và có thể cho phép 1 *Master* kết nối với nhiều *Slave*. Bao gồm 4 dây:
- SCK (Serial Clock): Tạo xung tín hiệu để cấp cho các Slave, các Slave và Master sử dụng chung 1 dây SCK.
- MISO (Master In Slave Out): Chân Master dùng để nhận các dữ liệu từ các Slave, các Slave và Master sử dụng chung 1 dây MISO.
- MOSI (Master Out Slave In): Chân Master dùng để gửi các dữ liệu tới các Slave, các Slave và Master sử dụng chung 1 dây MOSI.
- SS/CS (Slave Select/Chip Select): Chọn thiết bị Slave cụ thể để giao tiếp. Để chọn Slave giao tiếp thiết bị Master chủ động kéo đường SS tương ứng xuống mức 0 (Low). Có bao nhiêu Slave thì Master sẽ có bấy nhiêu chân SS để các Slave sử dụng riêng.

Nguyên lý hoạt động:
- Khi Master muốn giao tiếp với Slave thì sẽ hạ mức điện áp của chân SS tương ứng xuống mức 0.
- Master sẽ phát xung clock, cứ mỗi chu kỳ clock sẽ là 1 bit được Master truyền đi qua MOSI và nhận lại qua MISO.
- Các thanh ghi nhận của Slave và Master sẽ cập nhật giá trị nhận vào và dịch bit để sẵn sàng nhận bit tiếp theo.
- Lặp lại quá trình cho đến khi truyền/nhận xong 8 bit trong thanh ghi.

SPI có 4 chế độ hoạt động phụ thuộc Clock Polarity – CPOL và  Phase - CPHA:
- CPOL quyết định hình dạng của xung clock.
	- CPOL = 0: khi không truyền/nhận, SCK sẽ ở mức 0. Khi muốn truyền/nhận thì Master sẽ kéo chân SCK lên mức 1 theo chu kỳ.
 	- CPOL = 1: khi không truyền/nhận, SCK sẽ ở mức 1. Khi muốn truyền/nhận thì Master sẽ kéo chân SCK lên mức 0 theo chu kỳ (ngược lại với CPOL = 0).
- CPHA quyết định nơi mà bit sẽ được truyền đi trong một chu kỳ xung clock.
	- CPHA = 0: bit được truyền/nhận ở cạnh đầu tiên trong chu kỳ xung clock, VD khi CPOL = 0 thì cạnh đầu tiên là cạnh lên, CPOL = 1 là cạnh xuống.
	- CPHA = 1: bit được truyền/nhận ở cạnh thứ hai trong chu kỳ xung clock.
### I2C

### UART
