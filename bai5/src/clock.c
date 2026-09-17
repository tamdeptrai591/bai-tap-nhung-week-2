#include "clock.h"

/* =========================================================
   STM32F103 Clock
   HSE 8 MHz -> PLL x9 -> SYSCLK 72 MHz

   HCLK  = 72 MHz
   PCLK1 = 36 MHz
   PCLK2 = 72 MHz
   ========================================================= */

/* RCC registers */
#define RCC_CR          (*(volatile uint32_t *)0x40021000)
#define RCC_CFGR        (*(volatile uint32_t *)0x40021004)

/* Flash registers */
#define FLASH_ACR       (*(volatile uint32_t *)0x40022000)

/* RCC_CR bits */
#define RCC_CR_HSEON    (1U << 16)
#define RCC_CR_HSERDY   (1U << 17)
#define RCC_CR_PLLON    (1U << 24)
#define RCC_CR_PLLRDY   (1U << 25)

/* RCC_CFGR bits */

/* SW */
#define RCC_CFGR_SW_HSI     0x00000000U
#define RCC_CFGR_SW_HSE     0x00000001U
#define RCC_CFGR_SW_PLL     0x00000002U

/* SWS */
#define RCC_CFGR_SWS_MASK   0x0000000CU
#define RCC_CFGR_SWS_PLL    0x00000008U

/* HPRE = SYSCLK / 1 */
#define RCC_CFGR_HPRE_DIV1  0x00000000U

/* PPRE1 = HCLK / 2 */
#define RCC_CFGR_PPRE1_DIV2 0x00000400U

/* PPRE2 = HCLK / 1 */
#define RCC_CFGR_PPRE2_DIV1 0x00000000U

/* PLL source = HSE */
#define RCC_CFGR_PLLSRC_HSE 0x00010000U

/* PLL = HSE x9 */
#define RCC_CFGR_PLLMULL9   0x001C0000U


void Clock_Init(void)
{
    /* -----------------------------------------------------
       1. Bật HSE 8 MHz
       ----------------------------------------------------- */

    RCC_CR |= RCC_CR_HSEON;

    /* Chờ HSE ổn định */
    while (!(RCC_CR & RCC_CR_HSERDY))
    {
    }


    /* -----------------------------------------------------
       2. Cấu hình Flash
       72 MHz cần 2 wait states
       ----------------------------------------------------- */

    FLASH_ACR = 0x12;


    /* -----------------------------------------------------
       3. Cấu hình bus clock

       SYSCLK = 72 MHz
       AHB    = 72 MHz
       APB1   = 36 MHz
       APB2   = 72 MHz
       ----------------------------------------------------- */

    RCC_CFGR &= ~(
        0x000000F0U |     /* HPRE */
        0x00000700U       /* PPRE1 + PPRE2 */
    );

    RCC_CFGR |=
        RCC_CFGR_HPRE_DIV1 |
        RCC_CFGR_PPRE1_DIV2 |
        RCC_CFGR_PPRE2_DIV1;


    /* -----------------------------------------------------
       4. Chọn PLL

       PLL source = HSE
       HSE = 8 MHz
       PLL x9

       8 MHz x 9 = 72 MHz
       ----------------------------------------------------- */

    RCC_CFGR &= ~0x003D0000U;

    RCC_CFGR |=
        RCC_CFGR_PLLSRC_HSE |
        RCC_CFGR_PLLMULL9;


    /* -----------------------------------------------------
       5. Bật PLL
       ----------------------------------------------------- */

    RCC_CR |= RCC_CR_PLLON;

    /* Chờ PLL ổn định */
    while (!(RCC_CR & RCC_CR_PLLRDY))
    {
    }


    /* -----------------------------------------------------
       6. Chuyển SYSCLK sang PLL
       ----------------------------------------------------- */

    RCC_CFGR &= ~0x03U;
    RCC_CFGR |= RCC_CFGR_SW_PLL;


    /* -----------------------------------------------------
       7. Chờ hệ thống thực sự chạy bằng PLL
       ----------------------------------------------------- */

    while ((RCC_CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_PLL)
    {
    }
}
