/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __USER_RGB_H__
#define __USER_RGB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define gpio_low()  GPIOA->BRR = GPIO_PIN_0
#define gpio_high() GPIOA->BSRR = GPIO_PIN_0
#define delay_150ns() \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP")
#define delay_300ns() \
    delay_150ns();    \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP")
#define delay_600ns() \
    delay_300ns();    \
    delay_300ns();    \
    __asm("NOP");     \
    __asm("NOP")
#define delay_900ns() \
    delay_600ns();    \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP");     \
    __asm("NOP")

#define out_bit_low() \
    gpio_high();      \
    delay_300ns();    \
    gpio_low();       \
    delay_900ns()

#define out_bit_high() \
    gpio_high();       \
    delay_600ns();     \
    gpio_low();        \
    delay_600ns()

#define restart()                           \
    do {                                    \
        for (uint8_t i = 0; i < 255; i++) { \
            delay_900ns();                  \
        }                                   \
    } while (0)

/**
 * @brief Initialize the GPIO for RGB LED control.
 *        初始化用于 RGB LED 控制的 GPIO。
 * @param gpio GPIO index.
 *             GPIO 索引。
 */
void user_rgb_init(uint8_t gpio);

/**
 * @brief Update the internal color buffer from registers.
 *        从寄存器更新内部颜色缓冲区。
 */
void user_rgb_buffer_update(void);

/**
 * @brief Check for refresh flags and send data to RGB LEDs.
 *        检查刷新标志并向 RGB LED 发送数据。
 */
void user_sys_rgb_update(void);

#ifdef __cplusplus
}
#endif

#endif /* __USER_RGB_H__ */
