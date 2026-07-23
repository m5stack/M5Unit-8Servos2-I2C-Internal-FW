/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "user_i2c_callback.h"
#include "user_i2c_addr.h"
#include <string.h>  // for memcpy

// Internal variable for timeout logic
static volatile uint32_t i2c_stop_timeout_delay = 0;
static volatile uint8_t i2c_addr_refresh_pending = 0;
static uint8_t g_uid[UID_REG_LENGTH]             = {0};

// --- Global Variable Definitions ---
// --- 全局变量定义 ---

volatile uint8_t gpio_mode_changed[8]         = {0};  // Flag for each PIN mode change / 每个引脚的模式变化标志
volatile uint8_t gpio_mode_changed_any        = 0;    // Flag for ANY PIN mode change / 任意引脚模式变化标志
volatile uint8_t gpio_servo_angle_changed[8]  = {0};  // Flag for Servo angle change / 舵机角度变化标志
volatile uint8_t gpio_servo_angle_changed_any = 0;    // Flag for ANY Servo angle change / 任意舵机角度变化标志
volatile uint8_t gpio_pwm_duty_changed[8]     = {0};  // Flag for PWM duty change / PWM 占空比变化标志
volatile uint8_t gpio_pwm_duty_changed_any    = 0;    // Flag for ANY PWM duty change / 任意 PWM 占空比变化标志
volatile uint8_t gpio_rgb_buf_changed_any     = 0;    // Flag for RGB buffer update / RGB 缓冲区更新标志
volatile uint8_t tim_freq_changed[2]          = {0};  // Flag for Timer freq change / 定时器频率变化标志
volatile uint8_t tim_freq_changed_any         = 0;    // Flag for ANY Timer freq change / 任意定时器频率变化标志

// Default Register Values
// 默认寄存器值
volatile uint8_t gpio_mode_reg[8]               = {USER_GPIO_INPUT_MODE, USER_GPIO_INPUT_MODE, USER_GPIO_INPUT_MODE,
                                                   USER_GPIO_INPUT_MODE, USER_GPIO_INPUT_MODE, USER_GPIO_INPUT_MODE,
                                                   USER_GPIO_INPUT_MODE, USER_GPIO_INPUT_MODE};
volatile uint8_t gpio_gpio_input_pu_pd_reg[8]   = {0};
volatile uint8_t gpio_gpio_input_status_reg[8]  = {0};
volatile uint8_t gpio_gpio_output_status_reg[8] = {0};
volatile uint16_t gpio_adc_value_reg[8]         = {0};
volatile uint8_t gpio_servo_angle_reg[8]        = {0};
volatile rgb_config_t gpio_rgb_reg[8]           = {0};
volatile uint8_t gpio_pwm_duty_reg[8]           = {0};
volatile uint32_t gpio_rgb_color_buf_reg[16]    = {0};
volatile user_sys_time_config_t tim_freq_reg[2] = {0};
volatile uint16_t vref_mv_reg                   = SYS_VREF_MV;
volatile uint16_t grove_voltage_mv_reg          = 0;
volatile uint16_t dc_voltage_mv_reg             = 0;
volatile uint16_t sys_current_ma_reg            = 0;
volatile uint8_t fw_version_reg                 = FIRMWARE_VERSION;
volatile uint8_t i2c_addr_reg                   = 0;

void read_uid(void)
{
    uint32_t uid0 = HAL_GetUIDw0();
    uint32_t uid1 = HAL_GetUIDw1();
    uint32_t uid2 = HAL_GetUIDw2();

    memcpy(&g_uid[0], &uid0, sizeof(uid0));
    memcpy(&g_uid[4], &uid1, sizeof(uid1));
    memcpy(&g_uid[8], &uid2, sizeof(uid2));
}

/**
 * @brief Helper function to parse I2C write data into a temporary buffer.
 *        辅助函数：将 I2C 写入数据解析到临时缓冲区。
 * @note  Handles non-contiguous writes logic (though simple I2C usually writes contiguously).
 *        处理非连续写入逻辑（尽管简单的 I2C 通常是连续写入）。
 *
 * @param rx_buf Temporary data buffer. / 临时数据缓冲区。
 * @param rx_mark Mask buffer to indicate which bytes were updated. / 掩码缓冲区，指示哪些字节已更新。
 * @param rx_data Raw I2C received data (Address + Payload). / 原始 I2C 接收数据（地址 + 负载）。
 * @param base_addr The base address of the current register block. / 当前寄存器块的基地址。
 * @param len Total length of received data. / 接收数据的总长度。
 */
static void update_register_buffer(uint8_t *rx_buf, uint8_t *rx_mark, uint8_t *rx_data, uint8_t base_addr, uint16_t len)
{
    // rx_data[0] is the start register address
    // rx_data[1...] is the payload
    // rx_data[0] 是起始寄存器地址
    // rx_data[1...] 是负载
    for (uint8_t i = 0; i < len - 1; i++) {
        uint8_t rx_index  = rx_data[0] - base_addr + i;
        rx_buf[rx_index]  = rx_data[1 + i];
        rx_mark[rx_index] = 1;  // Mark this byte as updated / 标记此字节已更新
    }
}

/**
 * @brief Main Callback for I2C Transaction Completion.
 *        I2C 事务完成的主回调函数。
 */
void Slave_Complete_Callback(uint8_t *rx_data, uint16_t len)
{
    uint8_t rx_buf[64];         // Temp buffer for parsing writes / 用于解析写入的临时缓冲区
    uint8_t tx_buf[64];         // Temp buffer for preparing reads / 用于准备读取的临时缓冲区
    uint8_t rx_mark[64] = {0};  // Modification flags / 修改标志

    // --- CASE 1: Read Request Preparation (Master Write Address, then Read) ---
    // --- 情况 1：读请求准备（主机写地址，然后读）---
    // If len == 1, Master sent only the register address. We prepare data to send back.
    // 如果 len == 1，主机仅发送了寄存器地址。我们准备要发回的数据。
    if (len == 1) {
        // 1. GPIO Mode
        if (rx_data[0] <= REG_GPIO_MODE_END) {
            memcpy(tx_buf, (void *)gpio_mode_reg, 8);
            i2c1_set_send_data((uint8_t *)&tx_buf[rx_data[0] - REG_GPIO_MODE_START],
                               REG_GPIO_MODE_END - rx_data[0] + 1);
        }
        // 2. Input Pull-up/down
        else if (rx_data[0] >= REG_GPIO_INPUT_PU_PD_START && rx_data[0] <= REG_GPIO_INPUT_PU_PD_END) {
            memcpy(tx_buf, (void *)gpio_gpio_input_pu_pd_reg, 8);
            i2c1_set_send_data((uint8_t *)&tx_buf[rx_data[0] - REG_GPIO_INPUT_PU_PD_START],
                               REG_GPIO_INPUT_PU_PD_END - rx_data[0] + 1);
        }
        // 3. Input Status (Real-time read)
        else if (rx_data[0] >= REG_GPIO_INPUT_STATUS_START && rx_data[0] <= REG_GPIO_INPUT_STATUS_END) {
            for (uint8_t i = 0; i < 8; i++) {
                tx_buf[i] = user_input_get_level(i);
            }
            i2c1_set_send_data((uint8_t *)&tx_buf[rx_data[0] - REG_GPIO_INPUT_STATUS_START],
                               REG_GPIO_INPUT_STATUS_END - rx_data[0] + 1);
        }
        // 4. Output Status
        else if (rx_data[0] >= REG_GPIO_OUTPUT_STATUS_START && rx_data[0] <= REG_GPIO_OUTPUT_STATUS_END) {
            memcpy(tx_buf, (void *)gpio_gpio_output_status_reg, 8);
            i2c1_set_send_data((uint8_t *)&tx_buf[rx_data[0] - REG_GPIO_OUTPUT_STATUS_START],
                               REG_GPIO_OUTPUT_STATUS_END - rx_data[0] + 1);
        }
        // 5. ADC Values
        else if (rx_data[0] >= REG_GPIO_ADC_START && rx_data[0] <= REG_GPIO_ADC_END) {
            memcpy(tx_buf, (void *)gpio_adc_value_reg, 16);
            i2c1_set_send_data((uint8_t *)&tx_buf[rx_data[0] - REG_GPIO_ADC_START], REG_GPIO_ADC_END - rx_data[0] + 1);
        }
        // 6. Servo Angles
        else if (rx_data[0] >= REG_GPIO_SERVO_START && rx_data[0] <= REG_GPIO_SERVO_END) {
            memcpy(tx_buf, (void *)gpio_servo_angle_reg, 8);
            i2c1_set_send_data((uint8_t *)&tx_buf[rx_data[0] - REG_GPIO_SERVO_START],
                               REG_GPIO_SERVO_END - rx_data[0] + 1);
        }
        // 7. RGB Configuration
        else if (rx_data[0] >= REG_GPIO_RGB_START && rx_data[0] <= REG_GPIO_RGB_END) {
            for (int i = 0; i < 8; i++) tx_buf[i] = gpio_rgb_reg[i].rgb_config.value;
            i2c1_set_send_data((uint8_t *)&tx_buf[rx_data[0] - REG_GPIO_RGB_START], REG_GPIO_RGB_END - rx_data[0] + 1);
        }
        // 8. PWM Duty Cycle
        else if (rx_data[0] >= REG_GPIO_PWM_DUTYCYCLE_START && rx_data[0] <= REG_GPIO_PWM_DUTYCYCLE_END) {
            memcpy(tx_buf, (void *)gpio_pwm_duty_reg, 8);
            i2c1_set_send_data((uint8_t *)&tx_buf[rx_data[0] - REG_GPIO_PWM_DUTYCYCLE_START],
                               REG_GPIO_PWM_DUTYCYCLE_END - rx_data[0] + 1);
        }
        // 9. RGB Data Buffer
        else if (rx_data[0] >= REG_RGB_BUFFER_START && rx_data[0] <= REG_RGB_BUFFER_END) {
            memcpy(tx_buf, (void *)gpio_rgb_color_buf_reg, 64);
            i2c1_set_send_data((uint8_t *)&tx_buf[rx_data[0] - REG_RGB_BUFFER_START],
                               REG_RGB_BUFFER_END - rx_data[0] + 1);
        }
        // 10. Timer Frequencies
        else if (rx_data[0] >= REG_TIM1_FREQ_LOW && rx_data[0] <= REG_TIM2_FREQ_HIGH) {
            tx_buf[0] = tim_freq_reg[0].buffer[0];
            tx_buf[1] = tim_freq_reg[0].buffer[1];
            tx_buf[2] = tim_freq_reg[1].buffer[0];
            tx_buf[3] = tim_freq_reg[1].buffer[1];
            i2c1_set_send_data((uint8_t *)&tx_buf[rx_data[0] - REG_TIM1_FREQ_LOW], REG_TIM2_FREQ_HIGH - rx_data[0] + 1);
        }
        // 11. Device Unique ID
        else if (rx_data[0] >= UID_REG_ADDR_START && rx_data[0] <= UID_REG_ADDR_END) {
            uint8_t uid_offset = rx_data[0] - UID_REG_ADDR_START;
            i2c1_set_send_data(&g_uid[uid_offset], UID_REG_LENGTH - uid_offset);
        }
        // 12. System Monitor (Voltage/Current)
        else if (rx_data[0] >= REG_SYS_REF_VOLTAGE_LOW && rx_data[0] <= REG_SYS_CURRENT_HIGH) {
            tx_buf[0] = (uint8_t)(vref_mv_reg & 0x00FF);
            tx_buf[1] = (uint8_t)((vref_mv_reg >> 8) & 0x00FF);
            tx_buf[2] = (uint8_t)(grove_voltage_mv_reg & 0x00FF);
            tx_buf[3] = (uint8_t)((grove_voltage_mv_reg >> 8) & 0x00FF);
            tx_buf[4] = (uint8_t)(dc_voltage_mv_reg & 0x00FF);
            tx_buf[5] = (uint8_t)((dc_voltage_mv_reg >> 8) & 0x00FF);
            tx_buf[6] = (uint8_t)(sys_current_ma_reg & 0x00FF);
            tx_buf[7] = (uint8_t)((sys_current_ma_reg >> 8) & 0x00FF);
            i2c1_set_send_data((uint8_t *)&tx_buf[rx_data[0] - REG_SYS_REF_VOLTAGE_LOW],
                               REG_SYS_CURRENT_HIGH - rx_data[0] + 1);
        }
        // 13. Hardware address locator
        else if (rx_data[0] == REG_ADDR_OFFSET) {
            tx_buf[0] = i2c_addr_get_offset();
            i2c1_set_send_data(tx_buf, 1);
        }
        // 14. Version & Address
        else if (rx_data[0] >= REG_SW_VER && rx_data[0] <= REG_I2C_ADDR) {
            tx_buf[0] = fw_version_reg;
            tx_buf[1] = i2c_addr_reg;
            i2c1_set_send_data((uint8_t *)&tx_buf[rx_data[0] - REG_SW_VER], REG_I2C_ADDR - rx_data[0] + 1);
        }
    }

    // --- CASE 2: Write Request Processing ---
    // --- 情况 2：写请求处理 ---
    // If len > 1, Master wrote data. We update registers and set flags.
    // 如果 len > 1，主机写入了数据。我们更新寄存器并设置标志。
    else if (len >= 1) {
        // 1. GPIO Mode
        if (rx_data[0] <= REG_GPIO_MODE_END) {
            update_register_buffer(rx_buf, rx_mark, rx_data, REG_GPIO_MODE_START, len);
            for (uint8_t i = 0; i < 8; i++) {
                // Ensure valid mode (0-5)
                if (rx_mark[i] && gpio_mode_reg[i] != rx_buf[i] && rx_buf[i] <= USER_GPIO_PWM_MODE) {
                    gpio_mode_reg[i]      = rx_buf[i];
                    gpio_mode_changed[i]  = 1;
                    gpio_mode_changed_any = 1;
                }
            }
        }
        // 2. Input Pull-up/down
        else if (rx_data[0] >= REG_GPIO_INPUT_PU_PD_START && rx_data[0] <= REG_GPIO_INPUT_PU_PD_END) {
            update_register_buffer(rx_buf, rx_mark, rx_data, REG_GPIO_INPUT_PU_PD_START, len);
            for (uint8_t i = 0; i < 8; i++) {
                if (rx_mark[i] && gpio_gpio_input_pu_pd_reg[i] != rx_buf[i]) {
                    gpio_gpio_input_pu_pd_reg[i] = rx_buf[i];
                    // Only flag change if currently in Input mode
                    if (gpio_mode_reg[i] == USER_GPIO_INPUT_MODE) {
                        gpio_mode_changed[i]  = 1;
                        gpio_mode_changed_any = 1;
                    }
                }
            }
        }
        // 3. Output Status
        else if (rx_data[0] >= REG_GPIO_OUTPUT_STATUS_START && rx_data[0] <= REG_GPIO_OUTPUT_STATUS_END) {
            update_register_buffer(rx_buf, rx_mark, rx_data, REG_GPIO_OUTPUT_STATUS_START, len);
            for (uint8_t i = 0; i < 8; i++) {
                if (rx_mark[i] && gpio_gpio_output_status_reg[i] != rx_buf[i]) {
                    gpio_gpio_output_status_reg[i] = rx_buf[i];
                    // Immediate update if in Output mode
                    if (gpio_mode_reg[i] == USER_GPIO_OUTPUT_MODE) {
                        user_output_set_level(i);
                    }
                }
            }
        }
        // 4. Servo Angles
        else if (rx_data[0] >= REG_GPIO_SERVO_START && rx_data[0] <= REG_GPIO_SERVO_END) {
            update_register_buffer(rx_buf, rx_mark, rx_data, REG_GPIO_SERVO_START, len);
            for (uint8_t i = 0; i < 8; i++) {
                if (rx_mark[i] && gpio_servo_angle_reg[i] != rx_buf[i]) {
                    gpio_servo_angle_reg[i] = rx_buf[i];
                    if (gpio_mode_reg[i] == USER_GPIO_SERVO_MODE) {
                        gpio_servo_angle_changed[i]  = 1;
                        gpio_servo_angle_changed_any = 1;
                    }
                }
            }
        }
        // 5. RGB Configuration
        else if (rx_data[0] >= REG_GPIO_RGB_START && rx_data[0] <= REG_GPIO_RGB_END) {
            update_register_buffer(rx_buf, rx_mark, rx_data, REG_GPIO_RGB_START, len);
            for (uint8_t i = 0; i < 8; i++) {
                if (rx_mark[i]) {
                    rgb_config_t rgb_config_temp     = {0};
                    rgb_config_temp.rgb_config.value = rx_buf[i];
                    // Basic validation of LED count
                    if (rgb_config_temp.rgb_config.flags.num <= RGB_NUM_MAX) {
                        gpio_rgb_reg[i].rgb_config.flags.num     = rgb_config_temp.rgb_config.flags.num;
                        gpio_rgb_reg[i].rgb_config.flags.refresh = rgb_config_temp.rgb_config.flags.refresh;
                    }
                }
            }
        }
        // 6. PWM Duty Cycle
        else if (rx_data[0] >= REG_GPIO_PWM_DUTYCYCLE_START && rx_data[0] <= REG_GPIO_PWM_DUTYCYCLE_END) {
            update_register_buffer(rx_buf, rx_mark, rx_data, REG_GPIO_PWM_DUTYCYCLE_START, len);
            for (uint8_t i = 0; i < 8; i++) {
                if (rx_mark[i] && gpio_pwm_duty_reg[i] != rx_buf[i]) {
                    gpio_pwm_duty_reg[i] = rx_buf[i];
                    if (gpio_mode_reg[i] == USER_GPIO_PWM_MODE) {
                        gpio_pwm_duty_changed[i]  = 1;
                        gpio_pwm_duty_changed_any = 1;
                    }
                }
            }
        }
        // 7. Timer Frequencies
        else if (rx_data[0] >= REG_TIM1_FREQ_LOW && rx_data[0] <= REG_TIM2_FREQ_HIGH) {
            update_register_buffer(rx_buf, rx_mark, rx_data, REG_TIM1_FREQ_LOW, len);

            // Loop for TIM1 and TIM2
            for (int i = 0; i < 2; i++) {
                int idx_low  = i * 2;
                int idx_high = idx_low + 1;

                uint8_t new_low     = tim_freq_reg[i].buffer[0];
                uint8_t new_high    = tim_freq_reg[i].buffer[1];
                uint8_t need_update = 0;

                // Check Low Byte
                if (rx_mark[idx_low] && rx_buf[idx_low] != new_low) {
                    new_low     = rx_buf[idx_low];
                    need_update = 1;
                }
                // Check High Byte
                if (rx_mark[idx_high] && rx_buf[idx_high] != new_high) {
                    new_high    = rx_buf[idx_high];
                    need_update = 1;
                }

                if (need_update) {
                    uint16_t new_freq = (uint16_t)new_low | ((uint16_t)new_high << 8);
                    // Validate frequency > 0
                    if (new_freq != 0 && new_freq != tim_freq_reg[i].freq) {
                        tim_freq_reg[i].freq      = new_freq;
                        tim_freq_reg[i].buffer[0] = new_low;
                        tim_freq_reg[i].buffer[1] = new_high;
                        tim_freq_changed[i]       = 1;
                        tim_freq_changed_any      = 1;
                    }
                }
            }
        }
        // 8. RGB Buffer Data
        else if (rx_data[0] >= REG_RGB_BUFFER_START && rx_data[0] <= REG_RGB_BUFFER_END) {
            update_register_buffer(rx_buf, rx_mark, rx_data, REG_RGB_BUFFER_START, len);
            // Copy data directly to buffer registry
            for (uint8_t i = 0; i < REG_RGB_BUFFER_END - REG_RGB_BUFFER_START + 1; i++) {
                if (rx_mark[i] == 1) {
                    // Access byte-wise
                    ((uint8_t *)gpio_rgb_color_buf_reg)[i] = rx_buf[i];
                    gpio_rgb_buf_changed_any               = 1;
                }
            }
        }
        // 9. Refresh I2C address from the hardware locator
        else if (rx_data[0] == REG_I2C_ADDR_REFRESH && len == 2) {
            i2c_addr_refresh_pending = 1;
        }
        // 10. IAP Firmware Update Trigger
        else if (rx_data[0] == REG_IAP_UPDATE_ADDR && len == 2) {
            if (rx_data[1] == IAP_UPDATE_KEY) {
                NVIC_SystemReset();  // Trigger system reset for bootloader / 复位单片机触发升级
            }
        }
    }
}

void i2c_addr_refresh_handler(void)
{
    if (!i2c_addr_refresh_pending) {
        return;
    }

    i2c_addr_refresh_pending = 0;
    i2c1_it_disable();
    LL_I2C_Disable(I2C1);
    LL_I2C_DeInit(I2C1);
    i2c_addr_init();
    user_i2c_init();
    i2c1_it_enable();
}

/**
 * @brief I2C Bus Timeout Handler.
 *        I2C 总线超时处理程序。
 * @note  Called periodically in main loop or systick callback.
 *        Attempts to recover I2C if the line gets stuck.
 *        在主循环或系统滴答回调中周期性调用。
 *        如果线路卡住，尝试恢复 I2C。
 */
void i2c_timeout_handler(void)
{
    // Reset counter if traffic is detected (logic usually in IRQ)
    i2c_timeout_counter = 0;

    // Check if stop timeout logic is active
    if (i2c_stop_timeout_flag) {
        if (i2c_stop_timeout_delay < HAL_GetTick()) {
            i2c_stop_timeout_counter++;
            i2c_stop_timeout_delay = HAL_GetTick() + 10;  // Check every 10ms
        }
    }

    // Critical Timeout: Force Reset I2C
    // 严重超时：强制复位 I2C
    if (i2c_stop_timeout_counter > 50) {  // > 500ms total
        LL_I2C_DeInit(I2C1);
        LL_I2C_DisableAutoEndMode(I2C1);
        LL_I2C_Disable(I2C1);
        LL_I2C_DisableIT_ADDR(I2C1);

        // Re-initialize I2C hardware
        user_i2c_init();
        i2c1_it_enable();

        // Short delay to allow bus to stabilize
        HAL_Delay(500);
    }
}
