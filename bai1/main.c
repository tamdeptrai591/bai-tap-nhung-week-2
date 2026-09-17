#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>

UART_HandleTypeDef huart1;

// Bộ đệm nhận dữ liệu
#define RX_BUFFER_SIZE 256
char rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_data;
uint16_t rx_index = 0;
volatile uint8_t msg_complete = 0;

void SystemClock_Config(void);
void UART1_Init(void);
void Error_Handler(void);

int main(void) {
    // Khởi tạo thư viện HAL
    HAL_Init();
    
    // Cấu hình Clock hệ thống
    SystemClock_Config();
    
    // Cấu hình UART1 (TX, RX)
    UART1_Init();

    // Bật ngắt nhận ký tự đầu tiên
    HAL_UART_Receive_IT(&huart1, &rx_data, 1);

    char tx_buffer[RX_BUFFER_SIZE + 50];

    while (1) {
        if (msg_complete) {
            // Định dạng chuỗi gửi lại PC
            sprintf(tx_buffer, "%s%s: %s\r\n", "D23CQVM01" , "04", rx_buffer);
            
            // Gửi dữ liệu qua UART
            HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, strlen(tx_buffer), HAL_MAX_DELAY);
            
            // Reset lại bộ đệm và cờ để nhận chuỗi mới
            memset(rx_buffer, 0, RX_BUFFER_SIZE);
            rx_index = 0;
            msg_complete = 0;
            
            // Tiếp tục bật ngắt nhận
            HAL_UART_Receive_IT(&huart1, &rx_data, 1);
        }
    }
}

// Cấu hình ngoại vi UART1: 9600 baud, 8 data bits, 1 stop bit, no parity
void UART1_Init(void) {
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX; // Chế độ cả TX và RX
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
}

// Cấu hình chân GPIO và Ngắt cho UART1 (Được gọi tự động bởi HAL_UART_Init)
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(uartHandle->Instance == USART1) {
        // Bật Clock cho USART1 và GPIOA
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        // Cấu hình PA9 (TX) - Alternate Function Push Pull
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        // Cấu hình PA10 (RX) - Input Floating hoặc Pull Up
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        // Cấu hình và bật ngắt NVIC cho USART1
        HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
}

// --- HÀM PHỤC VỤ NGẮT THỜI GIAN THỰC (RẤT QUAN TRỌNG ĐỂ HAL HOẠT ĐỘNG) ---
void SysTick_Handler(void) {
    HAL_IncTick();
}

// Hàm xử lý ngắt cứng (Hardware ISR) cho USART1
void USART1_IRQHandler(void) {
    HAL_UART_IRQHandler(&huart1);
}

// Callback được gọi khi ngắt nhận hoàn tất 1 ký tự
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        if (rx_data == '!') {
            // Nếu là ký tự kết thúc, đóng chuỗi và bật cờ
            rx_buffer[rx_index] = '\0';
            msg_complete = 1; 
        } else {
            // Lưu ký tự vào buffer nếu chưa đầy
            if (rx_index < (RX_BUFFER_SIZE - 1)) {
                rx_buffer[rx_index++] = rx_data;
            }
            // Gọi lại ngắt để nhận ký tự tiếp theo
            HAL_UART_Receive_IT(&huart1, &rx_data, 1);
        }
    }
}

// Cấu hình clock cơ bản (Giả sử dùng thạch anh nội HSI cho F103)
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
