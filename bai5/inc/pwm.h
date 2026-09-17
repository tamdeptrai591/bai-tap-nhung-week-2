#ifndef PWM_H
#define PWM_H

#include <stdint.h>

void PWM_Init(void);

void PWM_SetPercent(uint8_t percent);

void PWM_On(void);

void PWM_Off(void);

uint8_t PWM_GetPercent(void);

uint8_t PWM_GetState(void);

#endif
