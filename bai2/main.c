#include "stm32f10x.h" // Thư viện lõi cho dòng STM32F10x

/* Các biến đếm thời gian cho Systick */
volatile uint32_t count_0_1Hz = 0;
volatile uint32_t count_1Hz = 0;
volatile uint32_t count_10Hz = 0;

/* Khai báo nguyên mẫu hàm */
void GPIO_Configuration(void);

int main(void)
{
    /* 1. Khởi tạo GPIO (PA0, PA1, PA2) */
    GPIO_Configuration();

    /* 2. Cấu hình Systick ngắt mỗi 1ms (1/1000 giây)
     * SystemCoreClock đã được cập nhật tự động nhờ file startup (thường là 72MHz) 
     */
    if (SysTick_Config(SystemCoreClock / 1000))
    { 
        /* Nếu lỗi cấu hình Systick thì treo ở đây */
        while (1);
    }

    while (1)
    {
        // Vòng lặp main để trống, mọi việc xử lý ở ngắt Systick
    }
}

/* Hàm khởi tạo GPIO */
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Cấp xung nhịp cho PORT A
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Cấu hình chân PA0, PA1, PA2 là Output Push-Pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // Tắt các LED ban đầu
    GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
}

/* 
 * Trình phục vụ ngắt Systick (SysTick Handler).
 * Hàm này được gọi tự động mỗi 1ms.
 */
void SysTick_Handler(void)
{
    // Tăng các biến đếm lên 1 (tương ứng 1ms)
    count_0_1Hz++;
    count_1Hz++;
    count_10Hz++;

    // --- 1. Nháy LED chân PA0 tần số 0.1Hz (Đảo trạng thái mỗi 5000ms) ---
    if (count_0_1Hz >= 5000)
    {
        GPIOA->ODR ^= GPIO_Pin_0; // Đảo trạng thái bằng cách XOR thanh ghi ODR
        count_0_1Hz = 0;
    }

    // --- 2. Nháy LED chân PA1 tần số 1Hz (Đảo trạng thái mỗi 500ms) ---
    if (count_1Hz >= 500)
    {
        GPIOA->ODR ^= GPIO_Pin_1;
        count_1Hz = 0;
    }

    // --- 3. Nháy LED chân PA2 tần số 10Hz (Đảo trạng thái mỗi 50ms) ---
    if (count_10Hz >= 50)
    {
        GPIOA->ODR ^= GPIO_Pin_2;
        count_10Hz = 0;
    }
}
