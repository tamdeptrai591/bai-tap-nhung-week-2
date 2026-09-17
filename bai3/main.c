#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>

ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart1;

void SystemClock_Config(void);
void ADC1_Init(void);
void UART1_Init(void);
void Error_Handler(void);

int main(void) {
    // Khởi tạo thư viện HAL và Clock
    HAL_Init();
    SystemClock_Config();
    
    // Khởi tạo ngoại vi UART1 và ADC1
    UART1_Init();
    ADC1_Init();

    // Hiệu chuẩn ADC (Rất quan trọng trên dòng F1 để đo điện áp chuẩn xác)
    HAL_ADCEx_Calibration_Start(&hadc1);

    char tx_buffer[100];
    uint32_t adc_value = 0;
    float voltage = 0.0f;

    while (1) {
        // Khởi động bộ chuyển đổi ADC
        HAL_ADC_Start(&hadc1);
        
        // Đợi phần cứng chuyển đổi xong (Timeout 10ms)
        if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
            
            // Đọc giá trị ADC thô (Dải giá trị từ 0 đến 4095)
            adc_value = HAL_ADC_GetValue(&hadc1);
            
            // Tính toán điện áp thực tế (Vref = 3.3V, Độ phân giải = 12-bit)
            voltage = (adc_value * 3.3f) / 4095.0f;
            
            // Tách phần nguyên và thập phân (3 chữ số) vì printf của VĐK thường ko hỗ trợ %f
            uint8_t phan_nguyen = (uint8_t)voltage;
            uint16_t phan_thap_phan = (uint16_t)((voltage - phan_nguyen) * 1000);
            
            // Đóng gói chuỗi bản tin báo cáo LDR
            sprintf(tx_buffer, "[LDR Sensor] ADC: %lu | Dien ap: %d.%03d V\r\n", 
                    adc_value, phan_nguyen, phan_thap_phan);
            
            // Gửi lên máy tính qua UART
            HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), 100);
        }
        
        // Tạm dừng bộ ADC để tiết kiệm năng lượng
        HAL_ADC_Stop(&hadc1);
        
        // Đợi 1 giây theo đúng yêu cầu đề bài
        HAL_Delay(1000);
    }
}

// Cấu hình ADC1 đọc kênh 0 (Chân PA0)
void ADC1_Init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};

    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE; 
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT; // Căn lề phải 12-bit
    hadc1.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    // Cấu hình Kênh 0 (PA0)
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5; // Lấy mẫu lâu để tụ ổn định điện áp
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }
}

// Cấu hình UART1 (Baudrate 9600)
void UART1_Init(void) {
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX; // Bài này chỉ truyền đi nên chỉ cần TX
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
}

// Khởi tạo Clock và Chân GPIO cho các ngoại vi
void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(adcHandle->Instance == ADC1) {
        // Bật clock cho ADC1 và GPIOA
        __HAL_RCC_ADC1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        // Cấu hình PA0 làm chân ngõ vào Analog
        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(uartHandle->Instance == USART1) {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        // Cấu hình PA9 (TX)
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

// --- HÀM PHỤC VỤ NGẮT THỜI GIAN THỰC (ĐỂ HAL_DELAY HOẠT ĐỘNG) ---
void SysTick_Handler(void) {
    HAL_IncTick();
}

// Cấu hình clock cơ bản
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) { Error_Handler(); }
}

void Error_Handler(void) {
    while(1) {}
}
