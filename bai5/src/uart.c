#include "uart.h"


/* =====================================================
   RCC
   ===================================================== */

#define RCC_APB2ENR     (*(volatile uint32_t *)0x40021018)


/* =====================================================
   GPIOA
   ===================================================== */

#define GPIOA_CRH       (*(volatile uint32_t *)0x40010804)


/* =====================================================
   USART1
   ===================================================== */

#define USART1_SR       (*(volatile uint32_t *)0x40013800)
#define USART1_DR       (*(volatile uint32_t *)0x40013804)
#define USART1_BRR      (*(volatile uint32_t *)0x40013808)
#define USART1_CR1      (*(volatile uint32_t *)0x4001380C)


/* =====================================================
   NVIC
   ===================================================== */

#define NVIC_ISER1      (*(volatile uint32_t *)0xE000E104)


/* =====================================================
   UART INIT
   ===================================================== */

void UART_Init(void)
{
    /*
       Enable GPIOA
    */

    RCC_APB2ENR |= (1 << 2);


    /*
       Enable USART1
    */

    RCC_APB2ENR |= (1 << 14);


    /*
       PA9 = USART1_TX

       Alternate Function Push-Pull
       50 MHz

       1011 = 0xB
    */

    GPIOA_CRH &= ~(0xF << 4);
    GPIOA_CRH |=  (0xB << 4);


    /*
       PA10 = USART1_RX

       Input floating

       0100 = 0x4
    */

    GPIOA_CRH &= ~(0xF << 8);
    GPIOA_CRH |=  (0x4 << 8);


    /*
       Baudrate = 115200

       USART clock = 72 MHz

       BRR = 0x271
    */

    USART1_BRR = 0x271;


    /*
       Enable RX interrupt

       RXNEIE = bit 5
    */

    USART1_CR1 |= (1 << 5);


    /*
       Enable USART
    */

    USART1_CR1 |= (1 << 13);


    /*
       Enable TX
    */

    USART1_CR1 |= (1 << 3);


    /*
       Enable RX
    */

    USART1_CR1 |= (1 << 2);


    /*
       USART1 IRQ = 37

       ISER1:
       IRQ32 -> bit0

       IRQ37 -> bit5
    */

    NVIC_ISER1 |= (1 << 5);
}


/* =====================================================
   SEND CHARACTER
   ===================================================== */

void UART_SendChar(char c)
{
    /*
       TXE = bit 7
    */

    while (!(USART1_SR & (1 << 7)));

    USART1_DR = c;
}


/* =====================================================
   SEND STRING
   ===================================================== */

void UART_SendString(const char *str)
{
    while (*str)
    {
        UART_SendChar(*str);
        str++;
    }
}
/* =========================================================
   Kiểm tra UART có nhận dữ liệu không
   ========================================================= */
uint8_t UART_RxReady(void)
{
    return (USART1_SR & (1U << 5)) ? 1 : 0;
}


/* =========================================================
   Đọc 1 ký tự từ UART
   ========================================================= */
char UART_ReadChar(void)
{
    return (char)USART1_DR;
}