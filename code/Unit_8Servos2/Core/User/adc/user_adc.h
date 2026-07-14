/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __USER_ADC_H__
#define __USER_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief Initialize the ADC GPIO pin configuration.
 *        初始化 ADC GPIO 引脚配置。
 * @note  Sets the specified GPIO to Analog mode.
 *        将指定的 GPIO 设置为模拟模式。
 *
 * @param gpio Index of the GPIO to initialize.
 *             要初始化的 GPIO 索引。
 * @retval None
 */
void user_adc_init(uint8_t gpio);

/**
 * @brief Perform a full update cycle of ADC measurements.
 *        执行 ADC 测量的完整更新周期。
 * @note  Updates VREF, System Voltage, System Current, and user GPIO ADCs.
 *        更新 VREF、系统电压、系统电流和用户 GPIO ADC。
 *
 * @param None
 * @retval None
 */
void user_adc_update(void);

#ifdef __cplusplus
}
#endif

#endif /* __USER_ADC_H__ */