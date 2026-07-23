/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "user_i2c_addr.h"

/**
 * @brief Read hardware pins to determine address offset.
 *        读取硬件引脚以确定地址偏移量。
 * @note  Assumes Active-Low logic (Pin Grounded = 1, Pin Floating/High = 0).
 *        Logic:
 *          - GPIO_ADDR_8: Weight 8 (Bit 3)
 *          - GPIO_ADDR_4: Weight 4 (Bit 2)
 *          - GPIO_ADDR_2: Weight 2 (Bit 1)
 *          - GPIO_ADDR_1: Weight 1 (Bit 0)
 *        假设低电平有效逻辑（引脚接地 = 1，引脚悬空/高电平 = 0）。
 *        逻辑：
 *          - GPIO_ADDR_8: 权重 8 (位 3)
 *          - GPIO_ADDR_4: 权重 4 (位 2)
 *          - GPIO_ADDR_2: 权重 2 (位 1)
 *          - GPIO_ADDR_1: 权重 1 (位 0)
 *
 * @retval Calculated offset (0-15).
 *         计算出的偏移量 (0-15)。
 */
uint8_t i2c_addr_get_offset(void)
{
    // The '~' operator inverts the logic because inputs are typically pulled up
    // and switches connect to ground (Low = Active/1).
    // '~' 运算符反转逻辑，因为输入通常被上拉，开关连接到地（低电平 = 有效/1）。

    uint8_t position = ((~LL_GPIO_IsInputPinSet(GPIO_ADDR_8_GPIO_Port, GPIO_ADDR_8_Pin) & 0x01) << 3) |
                       ((~LL_GPIO_IsInputPinSet(GPIO_ADDR_4_GPIO_Port, GPIO_ADDR_4_Pin) & 0x01) << 2) |
                       ((~LL_GPIO_IsInputPinSet(GPIO_ADDR_2_GPIO_Port, GPIO_ADDR_2_Pin) & 0x01) << 1) |
                       ((~LL_GPIO_IsInputPinSet(GPIO_ADDR_1_GPIO_Port, GPIO_ADDR_1_Pin)) & 0x01);
    return position;
}

/**
 * @brief Initialize the global I2C address variable.
 *        初始化全局 I2C 地址变量。
 *
 * @param None
 * @retval None
 */
void i2c_addr_init(void)
{
    // Calculate final address: Default Address + Hardware Offset
    // 计算最终地址：默认地址 + 硬件偏移量
    i2c_addr_reg = I2C_DEFAULT_ADDR + i2c_addr_get_offset();
}
