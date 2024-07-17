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
    - **GPIO_Pin**: chân cần được cấu hình. Có định dạng là: "GPIO_Pin_\<chân cần cấu hình\>". Có thể cấu hình nhiều chân với cùng chế độ và tốc độ giống nhau bằng cách sử dụng toán tử OR nhiều định dạng chân lại.
    - **GPIO_Mode**: chế độ muốn cấu hình bao gồm các giá trị sau:
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
    - **GPIO_Speed**: tốc độ đáp ứng của chân bao gồm 3 tốc độ 2MHz, 10MHz và 50MHz. Có định dạng: "GPIO_Speed_\<tốc độ\>".
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
- **TIM_ClockDivision**: chia tần số gốc của CPU là 72MHz để cấp cho timer, có 3 bộ chia là chia 1, 2, 4. Định dạng cho giá trị này là: TIM_CKD_DIV\<số chia\>
- **TIM_Prescaler**: xác định bao nhiêu dao động thì timer sẽ đếm lên 1 lần. Vì là kiểu uint16_t nên giá trị mà biến này chấp nhận là từ 0 đến 65535.
- **TIM_Period**: xác định giá trị mà timer cần phải đếm tới (hoặc từ giá trị này đếm xuống 0) rồi mới tràn. Vì là kiểu uint16_t nên giá trị mà biến này chấp nhận là từ 0 đến 65535.
- **TIM_CounterMode**: xác định chế độ đếm. Có 2 chế độ thường dùng là đếm lên "TIM_CounterMode_Up" và đếm xuống "TIM_CounterMode_Down"

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
**SPI (Serial Peripheral Interface)** là chuẩn giao tiếp nối tiếp đồng bộ. SPI hoạt động ở dạng song công và có thể cho phép 1 *Master* kết nối với nhiều *Slave*. Bao gồm 4 dây:
- SCK (Serial Clock): Tạo xung tín hiệu để đồng bộ việc truyền/nhận dữ liệu với các Slave, các Slave và Master sử dụng chung 1 dây SCK.
- MISO (Master In Slave Out): Chân Master dùng để nhận các dữ liệu từ các Slave, các Slave và Master sử dụng chung 1 dây MISO.
- MOSI (Master Out Slave In): Chân Master dùng để gửi các dữ liệu tới các Slave, các Slave và Master sử dụng chung 1 dây MOSI.
- SS/CS (Slave Select/Chip Select): Chọn thiết bị Slave cụ thể để giao tiếp. Để chọn Slave giao tiếp thiết bị Master chủ động kéo đường SS tương ứng xuống mức 0 (Low). Có bao nhiêu Slave thì Master sẽ có bấy nhiêu chân SS để các Slave sử dụng riêng.

Nguyên lý hoạt động:
- Khi Master muốn giao tiếp với Slave thì sẽ hạ mức điện áp của chân SS tương ứng xuống mức 0.
- Master sẽ phát xung clock, cứ mỗi chu kỳ clock sẽ là 1 bit được Master truyền đi qua MOSI và nhận lại qua MISO.
- Các thanh ghi nhận của Slave và Master sẽ cập nhật giá trị nhận vào và dịch bit để sẵn sàng nhận bit tiếp theo.
- Lặp lại quá trình cho đến khi truyền/nhận xong 8 bit trong thanh ghi.

SPI có 4 chế độ hoạt động phụ thuộc Clock Polarity – CPOL và  Phase - CPHA:
- CPOL quyết định cực tính (hình dạng) của xung clock.
	- CPOL = 0: khi không truyền/nhận, SCK sẽ ở mức 0. Khi muốn truyền/nhận thì Master sẽ kéo chân SCK lên mức 1 theo chu kỳ.
 	- CPOL = 1: khi không truyền/nhận, SCK sẽ ở mức 1. Khi muốn truyền/nhận thì Master sẽ kéo chân SCK lên mức 0 theo chu kỳ (ngược lại với CPOL = 0).
- CPHA quyết định pha của xung clock, nơi mà bit sẽ được truyền đi trong một chu kỳ xung clock.
	- CPHA = 0: bit được truyền/nhận ở cạnh đầu tiên trong chu kỳ xung clock, VD khi CPOL = 0 thì cạnh đầu tiên là cạnh lên, CPOL = 1 là cạnh xuống.
	- CPHA = 1: bit được truyền/nhận ở cạnh thứ hai trong chu kỳ xung clock.

Tương tự các ngoại vi khác, các tham số SPI được cấu hình trong Struct SPI_InitTypeDef:
- **SPI_Mode**: Chế độ hoạt động của thiết bị SPI, bao gồm *SPI_Mode_Master* và *SPI_Mode_Slave*
- **SPI_Direction**: Kiểu truyền của thiết bị, bao gồm:
```
	SPI_Direction_2Lines_Fullduplex		// Song công
	SPI_Direction_2Lines_RxOnly			// 2 dây nhưng chỉ nhận
	SPI_Direction_1Line_Rx				// Sử dụng MOSI hoặc MISO chỉ để nhận
	SPI_Direction_1Line_Tx				// Sử dụng MOSI hoặc MISO chỉ để truyền
```
- **SPI_BaudRatePrescaler**: Hệ số chia clock cấp cho Module SPI. Thông số này chia clock nguồn trên bus tương ứng của SPI để cấp cho chân SCK hoạt động và có 8 giá trị từ 2 đến 256. Cú pháp: *SPI_BaudRatePrescaler_\<hệ số chia\>*
- **SPI_CPOL**: Hệ số CPOL của thiết bị, bao gồm *SPI_CPOL_Low* và *SPI_CPOL_High*
- **SPI_CPHA**: Hệ số CPHA của thiết bị, bao gồm *SPI_CPHA_1Edge* (CPHA = 0) và *SPI_CPHA_2Edge* (CPHA = 1)
- **SPI_DataSize**: Số bit được truyền, bao gồm *SPI_DataSize_8b* hoặc *SPI_DataSize_16b*
- **SPI_FirstBit**: Bit đầu tiên được truyền, baoo gồm *SPI_FirstBit_MSB* và *SPI_FirstBit_LSB*
- **SPI_CRCPolynomial**: Đặt giá trị đa thức kiểm tra chu kỳ tuần hoàn cho việc tính toán CRC, Master và Slave phải sử dụng chung 1 giá trị SPI_CRCPolynomial. Sử dụng hàm *SPI_CalculateCRC(SPI1, ENABLE)* để bật tính năng tính CRC
- **SPI_NSS**: Cấu hình chân SS là điều khiển bằng thiết bị hay phần mềm, bao gồm *SPI_NSS_Soft* và *SPI_NSS_Hard*.

Để ghi các giá trị đã cấu hình vào các thanh ghi của SPI thì ta sử dụng hàm *SPI_Init* với 2 tham số:
- Tham số đầu là kênh SPI ta sử dụng. STM32F103C8 có 3 bộ SPI, SPI1 nằm trên bus APB2, 2 bộ còn lại nằm trên APB1.
- Tham số thứ hai là con trỏ đến struct "SPI_InitTypeDef" đã cài đặt ở trên.

Sau khi đã hoàn thành cấu hình cho SPI thì ta phải cho phép SPI hoạt động bằng hàm *SPI_Cmd* với 2 tham số:
- Tham số đầu tiên: kênh SPI mà ta sử dụng.
- Tham thứ hai: có cho phép ngoại vi này hoạt động hay không. Có là **ENABLE**, không là **DISABLE**.

```
	SPI_InitTypeDef SPI_InitStructure;

	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
```
### I2C
**I2C (Inter-Intergrated Circuit)** là chuẩn giao tiếp nối tiếp, đồng bộ. I2C hoạt động ở dạng bán song công và có thể cho phép 1 *Master* kết nối với nhiều *Slave*. Bao gồm 2 dây:
- SCL (Serial Clock): Tạo xung tín hiệu để đồng bộ việc truyền/nhận dữ liệu với các Slave, các Slave và Master sử dụng chung 1 dây SCL.
- SDA (Serial Data): Chân chứa dữ liệu được truyền đi, các Slave và Master sử dụng chung 1 dây SDA.

I2C là một giao thức kiểu open-drain, tức là các đường dây chỉ được kéo xuống mức 0 khi muốn và sẽ không có mức điện áp cố định mà là sẽ là mức *Floating* ở những trường hợp còn lại. Vì thế, cần phải có điện trở kéo lên nguồn cho mỗi dây để chúng có trạng thái là mức 1 khi chưa bị kéo xuống mức 0.

Nguyên lý hoạt động (cách mà các bit dữ liệu được truyền đi giống với SPI):
- Dữ liệu được truyền trong I2C sẽ có 1 khung cố định.
- Khi muốn truyền/nhận dữ liệu nào đó, Master trước tiên sẽ tạo **Start Condition** bằng cách hạ đường SDA trước hạ đường SCL xuống mức 0.
- Tiếp theo, Master sẽ gửi 7 bit địa chỉ (dữ liệu 8 bit). Các bit địa chỉ được dùng để chọn Slave mà Master muốn giao tiếp. Với 7 bit địa chỉ thì trong I2C có thể chứa tối đa 127 Slave khác nhau (địa chỉ 0x00 là của Master).
- Sau khi gửi xong 7 bit địa chỉ thì Master tiếp tục gửi 1 bit R/W ngay sau bit địa chỉ cuối cùng. Bit này cho biết Master đang muốn gửi dữ liệu cho Slave (Write - 1) hay đọc dữ liệu từ Slave (Read - 0) có địa chỉ tương ứng.
- Theo sau mỗi một khung dữ liệu 8 bit sẽ là một bit xác nhận ACK/NACK. Bit này được bên nhận gửi đi cho bên gửi, cho biết rằng bên nhận đã nhận thành công (ACK - 0) hoặc không (NACK - 1).
- Khi Master gửi xong 8 bit trong khung dữ liệu, nó sẽ cho chạy timer 1 khoảng thời gian nhỏ, nếu không có Slave nào kéo chân SDA xuống mức 0 (chưa có ACK) khi timer chưa tràn thì Master sẽ hiểu là dữ liệu đã gửi không thành công (hoặc là không có Slave nào có địa chỉ tương ứng mà Master đã gửi).
- Sau khi Master phát hiện ACK từ Slave, nó sẽ tiến hành truyền/nhận khung dữ liệu 8 bit đầu tiên (MSB gửi trước, LSB gửi sau) tuỳ vào bit R/W ở khung dữ liệu đầu tiên. Sau đó, bên nhận sẽ gửi bit ACK.
- Sau khi đã phát hiện ACK thì sẽ tiếp tục truyền các khung dữ liệu cho tới khi hết dữ liệu.
- Khi muốn kết thúc quá trình truyền/nhận dữ liệu nào đó, Master sẽ tạo **Stop Condition** bằng cách thả đường SCL trước thả đường SDA lên mức 1.

### UART
**UART (Universal Asynchronous Receiver-Transmitter)** là một giao thức truyền thông phần cứng dùng giao tiếp nối tiếp không đồng bộ. UART hoạt động ở dạng song công và chỉ cho 1 máy gửi kết nối với 1 máy nhận. Bao gồm 2 dây:
- TX (Transmitt): Chân dùng để gửi dữ liệu đi
- RX (Receive): Chân dùng để nhận tín hiệu

Vì UART không có chân xung clock để đồng bộ dữ liệu giữa bên gửi và bên nhận, nên nó sẽ sử dụng timer để xác định khoảng thời gian giữa 2 bit được truyền/nhận. Bên gửi và bên nhận sẽ đồng nhất sau khoảng thời gian bao nhiêu thì bit tiếp theo sẽ được truyền/nhận. Khoảng thời gian này được tính sử dụng thông số là **Baud rate**.

**Baud rate** là thông số cho biết bao nhiêu bit được gửi đi trong vòng 1s (đơn vị: bps - bits per second). Nhờ vào số bit được truyền trong 1s, ta tam suất để tính được sau bao nhiêu giây thì bit tiếp theo sẽ được gửi. Từ đó, bên gửi và bên nhận sẽ chỉnh sửa timer của mình sao cho khớp với khoảng thời gian này để đồng bộ được dữ liệu với nhau.

Nguyên lý hoạt động:
- Khi muốn truyền dữ liệu nào đó, bên gửi trước tiên sẽ tạo **Start Condition** bằng cách hạ đường TX từ mức 1 xuống mức 0 trong 1 khoảng thời gian đã đồng nhất ban đầu.
- Sau đó, bên gửi và bên nhận sẽ khởi động timer, bên gửi sau khi timer tràn sẽ gửi 1 bit, bên nhận sau khi timer tràn sẽ nhận 1 bit. Quá trình này diễn ra tới khi bit dữ liệu cuối cùng được gửi.
- Ngay sau khi các bit dữ liệu được gửi, có hoặc không 1 bit Parity được gửi. Bit Parity được dùng để kiểm tra lỗi trong các bit dữ liệu. Có 2 loại bit Parity là Parity chẵn (đảm bảo tổng số bit 1 trong các bit dữ liệu và bit Parity là số chẵn) và Parity lẻ (đảm bảo tổng số bit 1 trong các bit dữ liệu và bit Parity là số lẻ).
- Sau khi hoàn thành gửi Parity bit, bên gửi sẽ tạo **Stop Condition** bằng cách kéo đường TX từ 0 lên mức 1 trong khoảng thời gian từ 1 đến 2 lần khoảng thời gian đã đồng nhất ban đầu.

## Bài 4: ADC
**ADC (Analog-to-Digital Converter)** là một mạch điện tử chuyển đổi tín hiệu tương tự (analog signal) thành tín hiệu số (digital signal). ADC làm được điều này bằng cách chia dải mức tín hiệu analog thành nhiều mức, gán giá trị nhị phân cho từng mức, so sánh giá trị analog với các mức đó để quy về mức nhị phân phù hợp.\
Có 2 thông số quan trọng trong ADC:
- Độ phân giải: Chỉ số bit cần thiết để chứa hết các mức giá trị số sau khi chuyển đổi. Có thể hiểu là độ phân giải quyết định số mức chia của dải tín hiệu analog và ta phải điền vào các mức đó các giá trị nhị phân để phân biệt các mức với nhau, độ phân giải càng cao, càng nhiều mức thì sẽ cần càng nhiều bit nhị phân để phân biệt các mức với nhau.
- Tần số lấy mẫu: là số lần mà ADC lấy mẫu và chuyển đổi thành tín hiệu số trong vòng 1s. Có thể hiểu là khoảng thời gian giữa 2 lần lấy mẫu và chuyển đổi liên tiếp của ADC. Tần số lấy mẫu càng cao thì số lượng mẫu được lấy và chuyển đổi sẽ càng nhiều, các mẫu càng sát với nhau, càng giống tín hiệu gốc.
STM32F103C8 có 3 bộ ADC, mỗi bộ gồm 9 kênh với 2 chế độ hoạt động sau:
- Regular Inversion:
	- Single: Khi được yêu cầu chuyển đổi, chỉ 1 kênh được hoạt động và đọc dữ liệu chuyển đổi 1 lần
	- Single Continuous: Khi được yêu cầu chuyển đổi, chỉ 1 kênh được hoạt động và đọc dữ liệu chuyển đổi liên tục
   	- Scan: Khi được yêu cầu chuyển đổi, quét qua nhiều kênh và đọc dữ liệu chuyển đổi 1 lần
   	- Scan Continuous: Khi được yêu cầu chuyển đổi, quét qua nhiều kênh và đọc dữ liệu chuyển đổi liên tục
- Injected Inversion: Trong trường hợp nhiều kênh hoạt động, kênh có mức độ ưu tiên cao hơn có thể tạo ra một *Injected Trigger*. Khi gặp *Injected Trigger*, nếu kênh đang hoạt động có độ ưu tiên thấp hơn thì sẽ bị ngưng lại để kênh được ưu tiên kia có thể hoạt động.

## Bài 5: DMA
Việc trao đổi và lưu dữ liệu từ ngoại vi được CPU điều khiển. CPU sẽ trao đổi dữ liệu từ các ngoại vi và dẫn các dữ liệu đến RAM thông qua các đường bus. Ngoài ra, CPU còn phải đọc các lệnh từ FLASH để thực thi các lệnh và còn nhiều việc khác. CPU không thể làm một lúc nhiều công việc nên các dữ liệu từ ngoại vi có thể bị mất. Vì thế, DMA ra đời.\

**DMA (Direct Memory Access)** là một cơ chế giúp các thành phần phần cứng hay ngoại vi có thể giao tiếp trực tiếp với bộ nhớ. DMA có thể giúp việc trao đổi dữ liệu giữa ngoại vi và bộ nhớ hoặc giữa bộ nhớ và bộ nhớ không cần thông qua CPU hay những đường bus của CPU. Điều này giúp CPU không cần phải lo về việc đọc dữ liệu từ ngoại vi và có thể làm những công việc khác.\

DMA có 2 chế độ hoạt động chính:
- Normal: DMA truyền dữ liệu cho tới khi truyền đủ 1 lượng dữ liệu giới hạn đã khai báo, sau khi đã truyền đủ thì DMA sẽ dừng hoạt động. Muốn nó tiếp tục hoạt động thì phải khởi động lại (truyền 1 lần).
- Circular: Khi DMA truyền đủ 1 lượng dữ liệu giới hạn đã khai báo thì nó sẽ truyền tiếp về vị trí ban đầu (truyền liên tục).

## Bài 6: FLASH và Bootloader
Chip STM32F1 có 2 bộ nhớ chính:
- **RAM (Random Access Memory)** là một loại bộ nhớ cho phép truy-xuất ngẫu nhiên đến một vị trí bất kỳ trong bộ nhớ thông qua địa chỉ bộ nhớ. RAM có tốc độ đọc/ghi nhanh nhưng dữ liệu sẽ mất bị mất khi ngưng cấp nguồn. RAM thường được dùng để lưu trữ tạm thời dữ liệu và các lệnh đang được thực thi.
- **FLASH** là một bộ nhớ có thể xoá và lập trình. FLASH có tốc độ ghi chậm nhưng đọc nhanh và nó không bị mất dữ liệu khi ngưng cấp nguồn. FLASH được chia nhỏ thành các *Bank*, mỗi bank gồm một hoặc nhiều *Page*, mỗi page có kích thước là 1Kb. FLASH giới hạn số lần xoá/ghi và chỉ có thể ghi theo khối 2/4 byte. Vì FLASH không thể đẩy bit 1 xuống 0 nên trước khi ghi thì phải xoá FLASH trước. FLASH thường được dùng để lưu chương trình.\
Bộ nhớ chương trình được chia thành nhiều vùng khác nhau. Trong đó, 512MB đầu tiên được dành cho vùng code (các vùng dưới đây có địa chỉ theo thứ tự giảm dần là đi từ 0x1FFF FFFF đến 0x0000 0000):
- Option Bytes: Vùng byte tùy chọn chứa một loạt cờ bit có thể được sử dụng để định cấu hình một số khía cạnh của MCU (như bảo vệ đọc ghi FLASH, v.v…)
- System Memory: Là vùng ROM dành riêng cho Bootloader được lập trình sẵn vào chip trong quá trình sản xuất. Những Bootloader này có thể được sử dụng để tải code từ một số thiết bị ngoại vi, bao gồm USART, USB và CAN Bus.
- Bộ nhớ FLASH
### FLASH
Bộ nhớ FLASH có địa chỉ bắt đầu từ 0x0000 0000. Trong dòng VĐK STM32, vùng để lưu code là từ 0x0800 0000. Khi nạp chương trình thì sẽ mặc định lưu bắt đầu từ địa chỉ này với **MSP (Main Stack Pointer)** sẽ ở địa chỉ đó luôn và **Vector Table** (phần đầu tiên trong bộ nhớ của chương trình, chứa các vector ngắt) bắt đầu từ địa chỉ 0x0800 0004 (đây là địa chỉ của Reset_Handler vì nó là vector ngắt đầu tiên trong Vector Table).\
FLASH có thể thao tác ghi trên từng Word (4 byte) hoặc Halfword (2 byte) nhưng khi xoá thì xoá từng page. Vì bộ nhớ Flash là nơi lưu chương trình, để tránh trường hợp vô tình làm thay đổi dữ liệu chương trình, Flash có 1 cơ chế đơn giản là LOCK và UNLOCK vùng nhớ.
- Sau khi reset, Flash sẽ vào trạng thái LOCK để bảo vệ dữ liệu
- Khi người dùng muốn thao tác trên Flash thì cần phải UNLOCK nó trước
- Muốn ghi dữ liệu thì phải xoá hết dữ liệu trước

### Bootloader
**Quá trình reset cảu vi điều khiển**:
- Hệ thống sẽ khởi tạo phần cứng, thiết lập các cấu hình
- CPU tìm đến Vector Table trên bộ nhớ của chương trình
- CPU sẽ nạp giá trị tại ô nhớ đầu tiên của Vector Table vào thanh ghi MSP (ở trạng thái reset). Mặc định đối với STM32F1 là 0x0800 0000
- CPU sẽ lấy địa chỉ của Reset Handler được đặt trong ô nhớ thứ 2 của Vector Table, nạp vào thanh ghi PC để thực hiện hàm Reset Handler để khởi tạo ban đầu của hệ thống
- Gọi đến hàm main() trong file main.c để thực hiện chương trình chính

**Bootloader** là chương trình đầu tiên được chạy khi chip hoạt động. Bootloader có mục tiêu chính là nâng cấp hoặc sửa đổi phần mềm hệ thống mà không cần sự can thiệp của các công cụ nâng cấp chương trình cơ sở chuyên dụng. Vị trí của chương trình Bootloader thường được bắt đầu tại địa chỉ đầu tiên của bộ nhớ Flash (STM32F1 mặc định là 0x0800 0000), đây là địa chỉ mặc định sẽ được CPU thực thi sau khi Reset.\

Chương trình Boot hoạt động như sau:
- Khi reset, VĐK nhảy đến Reset_Handler (0x0800 0000) thực thi và nhảy tới hàm main() của chương trình boot
- Chương trình boot sẽ lấy địa chỉ từ firmware mà nó muốn nhảy đến bằng cách điều khiển MSP đến địa chỉ đó
- Đặt giá trị thanh ghi PC là ô nhớ tiếp theo sau MSP để VĐK thực thi reset_handler của firmware mới sau khi nhảy địa chỉ
- Cài đặt reset_handler của firmware mới bằng địa chỉ cần phải nhảy tới để sau khi reset, thanh ghi PC sẽ nhảy tới firmware mới.
