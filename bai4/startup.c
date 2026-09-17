#include <stdint.h>
extern uint32_t _sdata, _edata, _etext, _sbss, _ebss;
void main(void);

void Default_Handler(void) { while(1); }

void NMI_Handler(void)      __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)__attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void EXTI0_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void)__attribute__((weak, alias("Default_Handler")));

void Reset_Handler(void) {
    uint32_t *src = &_etext;
    uint32_t *dest = &_sdata;
    while (dest < &_edata) *dest++ = *src++;
    dest = &_sbss;
    while (dest < &_ebss) *dest++ = 0;
    main();
    while(1);
}

__attribute__((section(".isr_vector")))
void (*const vector_table[])(void) = {
    (void (*)(void))0x20005000,
    Reset_Handler, NMI_Handler, HardFault_Handler,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    SysTick_Handler,
    0, 0, 0, 0, 0, 0,
    EXTI0_Handler,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    TIM2_IRQHandler,
    0, 0, 0, 0, 0, 0, 0, 0,
    USART1_IRQHandler
};
