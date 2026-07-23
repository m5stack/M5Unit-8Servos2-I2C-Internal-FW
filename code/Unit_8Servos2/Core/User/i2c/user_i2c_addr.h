/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __USER_I2C_ADDR_H__
#define __USER_I2C_ADDR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "user_sys.h"
#include "user_i2c_callback.h"

/**
 * @brief Initialize the I2C slave address.
 *        初始化 I2C 从机地址。
 * @note  Reads external GPIO pins to calculate the address offset and adds it
 *        to the default base address.
 *        读取外部 GPIO 引脚以计算地址偏移量，并将其添加到默认基地址。
 */
void i2c_addr_init(void);

/**
 * @brief Read the current hardware address locator value (0-15).
 *        读取当前硬件地址定位器值（0-15）。
 */
uint8_t i2c_addr_get_offset(void);

#ifdef __cplusplus
}
#endif

#endif /* __USER_I2C_ADDR_H__ */
