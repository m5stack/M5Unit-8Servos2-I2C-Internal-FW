/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __USER_I2C_REG_H__
#define __USER_I2C_REG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/*
 * I2C Register Map Definition
 * I2C 寄存器映射定义
 *
 * Note: Individual register definitions (e.g., REG_GPIO0_MODE) are typically used as OFFSETS
 * relative to their group START address unless otherwise specified.
 * 注意：除非另有说明，单个寄存器定义（例如 REG_GPIO0_MODE）通常用作相对于其组 START 地址的偏移量。
 */

// --- GPIO Mode Configuration (0x00 - 0x07) ---
// --- GPIO 模式配置 (0x00 - 0x07) ---
#define REG_GPIO_MODE_START (0x00)
#define REG_GPIO_MODE_END   (0x07)
// Offsets / 偏移量
#define REG_GPIO0_MODE (0x00)
#define REG_GPIO1_MODE (0x01)
#define REG_GPIO2_MODE (0x02)
#define REG_GPIO3_MODE (0x03)
#define REG_GPIO4_MODE (0x04)
#define REG_GPIO5_MODE (0x05)
#define REG_GPIO6_MODE (0x06)
#define REG_GPIO7_MODE (0x07)

// --- GPIO Input Pull-up/Pull-down Configuration (0x10 - 0x17) ---
// --- GPIO 输入上拉/下拉配置 (0x10 - 0x17) ---
#define REG_GPIO_INPUT_PU_PD_START (0x10)
#define REG_GPIO_INPUT_PU_PD_END   (0x17)
// Offsets / 偏移量
#define REG_GPIO0_INPUT_PU_PD (0x00)
#define REG_GPIO1_INPUT_PU_PD (0x01)
#define REG_GPIO2_INPUT_PU_PD (0x02)
#define REG_GPIO3_INPUT_PU_PD (0x03)
#define REG_GPIO4_INPUT_PU_PD (0x04)
#define REG_GPIO5_INPUT_PU_PD (0x05)
#define REG_GPIO6_INPUT_PU_PD (0x06)
#define REG_GPIO7_INPUT_PU_PD (0x07)

// --- GPIO Input Status (Read-Only) (0x20 - 0x27) ---
// --- GPIO 输入状态 (只读) (0x20 - 0x27) ---
#define REG_GPIO_INPUT_STATUS_START (0x20)
#define REG_GPIO_INPUT_STATUS_END   (0x27)
// Offsets / 偏移量
#define REG_GPIO0_INPUT_STATUS (0x00)
#define REG_GPIO1_INPUT_STATUS (0x01)
#define REG_GPIO2_INPUT_STATUS (0x02)
#define REG_GPIO3_INPUT_STATUS (0x03)
#define REG_GPIO4_INPUT_STATUS (0x04)
#define REG_GPIO5_INPUT_STATUS (0x05)
#define REG_GPIO6_INPUT_STATUS (0x06)
#define REG_GPIO7_INPUT_STATUS (0x07)

// --- GPIO Output Status (0x30 - 0x37) ---
// --- GPIO 输出状态 (0x30 - 0x37) ---
#define REG_GPIO_OUTPUT_STATUS_START (0x30)
#define REG_GPIO_OUTPUT_STATUS_END   (0x37)
// Offsets / 偏移量
#define REG_GPIO0_OUTPUT_STATUS (0x00)
#define REG_GPIO1_OUTPUT_STATUS (0x01)
#define REG_GPIO2_OUTPUT_STATUS (0x02)
#define REG_GPIO3_OUTPUT_STATUS (0x03)
#define REG_GPIO4_OUTPUT_STATUS (0x04)
#define REG_GPIO5_OUTPUT_STATUS (0x05)
#define REG_GPIO6_OUTPUT_STATUS (0x06)
#define REG_GPIO7_OUTPUT_STATUS (0x07)

// --- ADC Value (Read-Only, 12-bit) (0x40 - 0x4F) ---
// --- ADC 值 (只读, 12位) (0x40 - 0x4F) ---
#define REG_GPIO_ADC_START (0x40)
#define REG_GPIO_ADC_END   (0x4F)
// Offsets (2 bytes per channel) / 偏移量 (每通道2字节)
#define REG_GPIO0_ADC_LOW  (0x00)
#define REG_GPIO0_ADC_HIGH (0x01)
#define REG_GPIO1_ADC_LOW  (0x02)
#define REG_GPIO1_ADC_HIGH (0x03)
#define REG_GPIO2_ADC_LOW  (0x04)
#define REG_GPIO2_ADC_HIGH (0x05)
#define REG_GPIO3_ADC_LOW  (0x06)
#define REG_GPIO3_ADC_HIGH (0x07)
#define REG_GPIO4_ADC_LOW  (0x08)
#define REG_GPIO4_ADC_HIGH (0x09)
#define REG_GPIO5_ADC_LOW  (0x0A)
#define REG_GPIO5_ADC_HIGH (0x0B)
#define REG_GPIO6_ADC_LOW  (0x0C)
#define REG_GPIO6_ADC_HIGH (0x0D)
#define REG_GPIO7_ADC_LOW  (0x0E)
#define REG_GPIO7_ADC_HIGH (0x0F)

// --- Servo Angle (0-180 degrees) (0x50 - 0x57) ---
// --- 舵机角度 (0-180 度) (0x50 - 0x57) ---
#define REG_GPIO_SERVO_START (0x50)
#define REG_GPIO_SERVO_END   (0x57)
// Offsets / 偏移量
#define REG_GPIO0_SERVO (0x00)
#define REG_GPIO1_SERVO (0x01)
#define REG_GPIO2_SERVO (0x02)
#define REG_GPIO3_SERVO (0x03)
#define REG_GPIO4_SERVO (0x04)
#define REG_GPIO5_SERVO (0x05)
#define REG_GPIO6_SERVO (0x06)
#define REG_GPIO7_SERVO (0x07)

// --- RGB LED Configuration (0x60 - 0x67) ---
// --- RGB LED 配置 (0x60 - 0x67) ---
#define REG_GPIO_RGB_START (0x60)
#define REG_GPIO_RGB_END   (0x67)
// Offsets / 偏移量
#define REG_GPIO0_RGB (0x00)
#define REG_GPIO1_RGB (0x01)
#define REG_GPIO2_RGB (0x02)
#define REG_GPIO3_RGB (0x03)
#define REG_GPIO4_RGB (0x04)
#define REG_GPIO5_RGB (0x05)
#define REG_GPIO6_RGB (0x06)
#define REG_GPIO7_RGB (0x07)

// --- PWM Duty Cycle (0-100%) (0x70 - 0x77) ---
// --- PWM 占空比 (0-100%) (0x70 - 0x77) ---
#define REG_GPIO_PWM_DUTYCYCLE_START (0x70)
#define REG_GPIO_PWM_DUTYCYCLE_END   (0x77)
// Offsets / 偏移量
#define REG_GPIO0_PWM_DUTYCYCLE (0x00)
#define REG_GPIO1_PWM_DUTYCYCLE (0x01)
#define REG_GPIO2_PWM_DUTYCYCLE (0x02)
#define REG_GPIO3_PWM_DUTYCYCLE (0x03)
#define REG_GPIO4_PWM_DUTYCYCLE (0x04)
#define REG_GPIO5_PWM_DUTYCYCLE (0x05)
#define REG_GPIO6_PWM_DUTYCYCLE (0x06)
#define REG_GPIO7_PWM_DUTYCYCLE (0x07)

// --- RGB Data Buffer (0x80 - 0xBF) ---
// --- RGB 数据缓冲区 (0x80 - 0xBF) ---
#define REG_RGB_BUFFER_START (0x80)
#define REG_RGB_BUFFER_END   (0xBF)

// Timer Frequencies (TIM1 - TIM2)
// 定时器频率 (TIM1 - TIM2)
#define REG_TIM1_FREQ_LOW  (0xD0)
#define REG_TIM1_FREQ_HIGH (0xD1)
#define REG_TIM2_FREQ_LOW  (0xD2)
#define REG_TIM2_FREQ_HIGH (0xD3)

// Device Unique ID (Read-Only, 96-bit)
// 设备唯一标识符（只读，96 位）
#define UID_REG_ADDR_START (0xE0)
#define UID_REG_ADDR_END   (0xEB)
#define UID_REG_LENGTH     (UID_REG_ADDR_END - UID_REG_ADDR_START + 1)

// --- System Configuration & Status (0xF0 - 0xFF) ---
// --- 系统配置与状态 (0xF0 - 0xFF) ---

// System Analog Measurements (mV/mA)
// 系统模拟测量 (mV/mA)
#define REG_SYS_REF_VOLTAGE_LOW  (0xF0)
#define REG_SYS_REF_VOLTAGE_HIGH (0xF1)
#define REG_GROVE_VOLTAGE_LOW    (0xF2)
#define REG_GROVE_VOLTAGE_HIGH   (0xF3)
#define REG_DC_VOLTAGE_LOW       (0xF4)
#define REG_DC_VOLTAGE_HIGH      (0xF5)
#define REG_SYS_CURRENT_LOW      (0xF6)
#define REG_SYS_CURRENT_HIGH     (0xF7)

// Hardware address locator (Read-Only)
// 硬件地址定位器（只读）
#define REG_ADDR_OFFSET (0xF8)

// Firmware Update & Info
// 固件更新与信息
#define REG_I2C_ADDR_REFRESH (0xFC)  // Refresh address from locator (Write-Only) / 根据定位器刷新地址（只写）
#define REG_IAP_UPDATE_ADDR (0xFD)  // IAP Update Key / IAP 更新密钥
#define REG_SW_VER          (0xFE)  // Software Version / 软件版本
#define REG_I2C_ADDR        (0xFF)  // Current I2C Address / 当前 I2C 地址

#ifdef __cplusplus
}
#endif

#endif /* __USER_I2C_REG_H__ */
