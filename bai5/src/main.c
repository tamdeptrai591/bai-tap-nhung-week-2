#include <stdint.h>

#include "clock.h"
#include "pwm.h"
#include "uart.h"

#define RX_BUFFER_SIZE 32

static char rx_buffer[RX_BUFFER_SIZE];
static uint8_t rx_index = 0;


/* =========================================================
   So sánh chuỗi
   ========================================================= */
static uint8_t StringEqual(const char *a, const char *b)
{
    while (*a && *b)
    {
        if (*a != *b)
            return 0;

        a++;
        b++;
    }

    return (*a == '\0' && *b == '\0');
}


/* =========================================================
   Xử lý lệnh UART
   ========================================================= */
static void ProcessCommand(char *cmd)
{
    /* ---------- ON! ---------- */
    if (StringEqual(cmd, "ON!"))
    {
        PWM_On();

        UART_SendString("OK: LED ON\r\n");
    }

    /* ---------- OFF! ---------- */
    else if (StringEqual(cmd, "OFF!"))
    {
        PWM_Off();

        UART_SendString("OK: LED OFF\r\n");
    }

    /* ---------- STATUS! ---------- */
    else if (StringEqual(cmd, "STATUS!"))
    {
        UART_SendString("STATUS: ");

        if (PWM_GetState())
            UART_SendString("ON, ");
        else
            UART_SendString("OFF, ");

        UART_SendString("PWM=");

        /* Hiện tại gửi đơn giản */
        UART_SendString("50");

        UART_SendString("%\r\n");
    }

    /* ---------- PWM:xx%! ---------- */
    else if (cmd[0] == 'P' &&
             cmd[1] == 'W' &&
             cmd[2] == 'M' &&
             cmd[3] == ':')
    {
        uint8_t percent = 0;
        uint8_t i = 4;

        while (cmd[i] >= '0' && cmd[i] <= '9')
        {
            percent = percent * 10 + (cmd[i] - '0');
            i++;
        }

        if (cmd[i] == '%' && cmd[i + 1] == '!')
        {
            if (percent <= 100)
            {
                PWM_SetPercent(percent);

                UART_SendString("OK: PWM SET\r\n");
            }
            else
            {
                UART_SendString("ERROR: PWM > 100\r\n");
            }
        }
        else
        {
            UART_SendString("ERROR: INVALID PWM\r\n");
        }
    }

    /* ---------- Lệnh không hợp lệ ---------- */
    else
    {
        UART_SendString("ERROR: UNKNOWN COMMAND\r\n");
    }
}


/* =========================================================
   USART1 Interrupt Handler
   ========================================================= */
void USART1_IRQHandler(void)
{
    if (UART_RxReady())
    {
        char c = UART_ReadChar();

        if (rx_index < RX_BUFFER_SIZE - 1)
        {
            rx_buffer[rx_index++] = c;

            /* Kết thúc command bằng ! */
            if (c == '!')
            {
                rx_buffer[rx_index] = '\0';

                ProcessCommand(rx_buffer);

                rx_index = 0;
            }
        }
        else
        {
            /* Buffer đầy → reset */
            rx_index = 0;
        }
    }
}


/* =========================================================
   MAIN
   ========================================================= */
int main(void)
{
    /* Clock = 72 MHz */
    Clock_Init();

    /* PWM TIM2_CH1 → PA0 */
    PWM_Init();

    /* USART1 → PA9 / PA10 */
    UART_Init();

    UART_SendString("STM32 READY\r\n");

    while (1)
    {
    }

    return 0;
}