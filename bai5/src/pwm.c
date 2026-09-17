#include "pwm.h"

/* =====================================================
   RCC
   ===================================================== */

#define RCC_APB2ENR     (*(volatile uint32_t *)0x40021018)
#define RCC_APB1ENR     (*(volatile uint32_t *)0x4002101C)


/* =====================================================
   GPIOA
   ===================================================== */

#define GPIOA_CRL       (*(volatile uint32_t *)0x40010800)


/* =====================================================
   TIM2
   ===================================================== */

#define TIM2_CR1        (*(volatile uint32_t *)0x40000000)
#define TIM2_CCMR1      (*(volatile uint32_t *)0x40000018)
#define TIM2_CCER       (*(volatile uint32_t *)0x40000020)
#define TIM2_PSC        (*(volatile uint32_t *)0x40000028)
#define TIM2_ARR        (*(volatile uint32_t *)0x4000002C)
#define TIM2_CCR1       (*(volatile uint32_t *)0x40000034)


/* =====================================================
   Biến trạng thái
   ===================================================== */

static uint8_t pwm_percent = 50;
static uint8_t pwm_state = 0;


/* =====================================================
   PWM INIT
   PA0 = TIM2_CH1
   ===================================================== */

void PWM_Init(void)
{
    /* Enable GPIOA clock */
    RCC_APB2ENR |= (1 << 2);

    /* Enable TIM2 clock */
    RCC_APB1ENR |= (1 << 0);


    /*
       PA0:
       Alternate Function Push-Pull
       50 MHz

       MODE = 11
       CNF  = 10

       => 1011 = 0xB
    */

    GPIOA_CRL &= ~(0xF << 0);
    GPIOA_CRL |=  (0xB << 0);


    /*
       Timer = 72 MHz

       72 MHz / 72 = 1 MHz
    */

    TIM2_PSC = 71;


    /*
       1 MHz / 1000 = 1 kHz
    */

    TIM2_ARR = 999;


    /*
       PWM Mode 1
    */

    TIM2_CCMR1 &= ~(0xFF);

    TIM2_CCMR1 |= (6 << 4);

    /* OC1 preload */
    TIM2_CCMR1 |= (1 << 3);


    /* Enable CH1 */
    TIM2_CCER |= (1 << 0);


    /* Ban đầu OFF */
    TIM2_CCR1 = 0;


    /* Auto reload preload */
    TIM2_CR1 |= (1 << 7);


    /* Start timer */
    TIM2_CR1 |= (1 << 0);
}


/* =====================================================
   SET PWM
   ===================================================== */

void PWM_SetPercent(uint8_t percent)
{
    if (percent > 100)
        percent = 100;

    /*
       Luôn lưu mức PWM mới
    */

    pwm_percent = percent;


    /*
       Nếu LED đang ON
       thì thay đổi độ sáng ngay
    */

    if (pwm_state)
    {
        TIM2_CCR1 = percent * 10;
    }
}


/* =====================================================
   ON
   ===================================================== */

void PWM_On(void)
{
    pwm_state = 1;

    /*
       Dùng mức PWM gần nhất
    */

    TIM2_CCR1 = pwm_percent * 10;
}


/* =====================================================
   OFF
   ===================================================== */

void PWM_Off(void)
{
    pwm_state = 0;

    /*
       Tắt LED nhưng vẫn giữ
       pwm_percent
    */

    TIM2_CCR1 = 0;
}


/* =====================================================
   GET PWM
   ===================================================== */

uint8_t PWM_GetPercent(void)
{
    return pwm_percent;
}


/* =====================================================
   GET STATE
   ===================================================== */

uint8_t PWM_GetState(void)
{
    return pwm_state;
}
