/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef __USER_I2C_CALLBACK_H__
#define __USER_I2C_CALLBACK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "user_i2c_reg.h"
#include "user_i2c.h"
#include "user_sys.h"

// --- Global Register & Flag Declarations ---
// --- 全局寄存器和标志声明 ---

// Change flags (Set when I2C write occurs, cleared by system update loop)
// 变化标志（I2C 写入时置位，由系统更新循环清除）
extern volatile uint8_t gpio_mode_changed[8];
extern volatile uint8_t gpio_mode_changed_any;
extern volatile uint8_t gpio_servo_angle_changed[8];
extern volatile uint8_t gpio_servo_angle_changed_any;
extern volatile uint8_t gpio_pwm_duty_changed[8];
extern volatile uint8_t gpio_pwm_duty_changed_any;
extern volatile uint8_t gpio_rgb_buf_changed_any;
extern volatile uint8_t tim_freq_changed[2];
extern volatile uint8_t tim_freq_changed_any;

// Register Data Stores
// 寄存器数据存储
extern volatile uint8_t gpio_mode_reg[8];
extern volatile uint8_t gpio_gpio_input_pu_pd_reg[8];
extern volatile uint8_t gpio_gpio_input_status_reg[8];
extern volatile uint8_t gpio_gpio_output_status_reg[8];
extern volatile uint16_t gpio_adc_value_reg[8];
extern volatile uint8_t gpio_servo_angle_reg[8];
extern volatile rgb_config_t gpio_rgb_reg[8];
extern volatile uint8_t gpio_pwm_duty_reg[8];
extern volatile uint32_t gpio_rgb_color_buf_reg[16];
extern volatile user_sys_time_config_t tim_freq_reg[2];
extern volatile uint16_t vref_mv_reg;
extern volatile uint16_t grove_voltage_mv_reg;
extern volatile uint16_t dc_voltage_mv_reg;
extern volatile uint16_t sys_current_ma_reg;
extern volatile uint8_t fw_version_reg;
extern volatile uint8_t i2c_addr_reg;

/**
 * @brief Read the 96-bit device unique ID into the UID register buffer.
 *        将 96 位设备唯一标识符读入 UID 寄存器缓冲区。
 */
void read_uid(void);

/**
 * @brief I2C Transaction Complete Callback.
 *        I2C 事务完成回调。
 * @note  Handles both Read preparation (len=1) and Write processing (len>1).
 *        处理读准备 (len=1) 和写处理 (len>1)。
 *
 * @param rx_data Pointer to the received data buffer (first byte is reg addr).
 *                接收数据缓冲区的指针（第一个字节是寄存器地址）。
 * @param len Total length of received data.
 *            接收数据的总长度。
 */
void Slave_Complete_Callback(uint8_t *rx_data, uint16_t len);

/**
 * @brief I2C Timeout Handler.
 *        I2C 超时处理程序。
 * @note  Resets the I2C peripheral if a bus lockup/timeout is detected.
 *        如果检测到总线死锁/超时，则复位 I2C 外设。
 */
void i2c_timeout_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __USER_I2C_CALLBACK_H__ */
