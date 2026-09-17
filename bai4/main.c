// Tự định nghĩa kiểu số nguyên 32-bit không dấu thay cho <stdint.h>
typedef unsigned int uint32_t;

// 1. Định nghĩa địa chỉ Base của các ngoại vi
#define RCC_BASE      0x40021000
#define GPIOA_BASE    0x40010800

// 2. Định nghĩa các thanh ghi cần dùng
#define RCC_APB2ENR   (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define GPIOA_CRL     (*(volatile uint32_t *)(GPIOA_BASE + 0x00)) 
#define GPIOA_IDR     (*(volatile uint32_t *)(GPIOA_BASE + 0x08)) 
#define GPIOA_ODR     (*(volatile uint32_t *)(GPIOA_BASE + 0x0C)) 

// Hàm delay chống dội phím
void delay(volatile uint32_t count) {
    while(count--) ;
}

int main(void) {
    // Cấp xung nhịp cho Port A
    RCC_APB2ENR |= (1 << 2);

    // Cấu hình PA3 (Output 2MHz) và PA0 (Input Pull-up/Pull-down)
    GPIOA_CRL &= ~(0xF00F); 
    GPIOA_CRL |= (0x2008); 

    // Kích hoạt điện trở Pull-up cho PA0 (Ghi mức 1 vào ODR bit 0)
    GPIOA_ODR |= (1 << 0); 

    while (1) {
        // Kiểm tra xem nút có được nhấn không (PA0 == 0)
        if ((GPIOA_IDR & (1 << 0)) == 0) { 
            
            // Chờ một khoảng nhỏ để chống dội phím
            delay(20000); 
            
            // Kiểm tra lại lần nữa
            if ((GPIOA_IDR & (1 << 0)) == 0) {
                
                // Khóa chương trình tại đây, chờ cho đến khi NHẢ NÚT
                while ((GPIOA_IDR & (1 << 0)) == 0) {
                    // Không làm gì cả, chỉ chờ đợi...
                }
                
                // Đảo trạng thái LED PA3
                GPIOA_ODR ^= (1 << 3); 
                
                // Trễ thêm một chút sau khi nhả để ổn định tín hiệu
                delay(20000);
            }
        }
    }
}
