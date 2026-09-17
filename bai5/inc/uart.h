#ifndef UART_H
#define UART_H

#include <stdint.h>

void UART_Init(void);

void UART_SendChar(char c);
void UART_SendString(const char *str);

/* Đọc 1 ký tự nhận được từ UART */
char UART_ReadChar(void);

/* Kiểm tra UART có nhận được dữ liệu chưa */
uint8_t UART_RxReady(void);

#endif