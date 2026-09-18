#include "stm32f10x.h"

// Khai báo các hàm cấu hình
void GPIO_Configuration(void);
void TIM2_PWM_Configuration(void);

int main(void)
{
    // Cấu hình các chân GPIO (PA0, PA1, PA2, PA3)
    GPIO_Configuration();
    
    // Cấu hình Timer 2 để phát PWM 1KHz
    TIM2_PWM_Configuration();

    while (1)
    {
        // Hệ thống chạy ngầm bằng phần cứng
    }
}

void GPIO_Configuration(void)
{
    // FIX: Khởi tạo giá trị bằng {0} để tránh nhận giá trị rác từ vùng nhớ
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // Cấp xung nhịp cho PORTA và chức năng Alternate Function (AFIO)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // Cấu hình PA0, PA1, PA2, PA3 ở chế độ Alternate Function Push-Pull (cho PWM)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void TIM2_PWM_Configuration(void)
{
    // FIX: Luôn gán {0} khi khai báo Struct trong SPL để tránh lỗi khi build bằng -O2
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};
    TIM_OCInitTypeDef  TIM_OCInitStructure = {0};

    // Cấp xung nhịp cho Timer 2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // Dùng hàm an toàn của thư viện để reset lại toàn bộ các trường về mặc định
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_OCStructInit(&TIM_OCInitStructure);

    // 1. Cấu hình chu kỳ Timer (PWM 1KHz với thạch anh 72MHz)
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;       
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;        // Chu kỳ = 1000
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 2. Cấu hình chế độ PWM Mode 1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // Kênh 1 (PA0) - Duty 10%
    TIM_OCInitStructure.TIM_Pulse = 100;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);

    // Kênh 2 (PA1) - Duty 30%
    TIM_OCInitStructure.TIM_Pulse = 300;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

    // Kênh 3 (PA2) - Duty 50%
    TIM_OCInitStructure.TIM_Pulse = 500;
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

    // Kênh 4 (PA3) - Duty 70%
    TIM_OCInitStructure.TIM_Pulse = 700;
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

    // Cho phép tính năng Preload của Timer 2 hoạt động
    TIM_ARRPreloadConfig(TIM2, ENABLE);

    // Kích hoạt Timer 2 đếm
    TIM_Cmd(TIM2, ENABLE);
}
