/* Includes ------------------------------------------------------------------*/
#include "user_i2c.h"

// --- Global Variables ---
// --- 全局变量 ---

// Rx Buffer: Stores data received from Master
// 接收缓冲区：存储从主机接收的数据
volatile uint8_t aReceiveBuffer[I2C_RECEIVE_BUFFER_LEN];

// Tx Buffer: Stores data waiting to be sent to Master
// 发送缓冲区：存储等待发送给主机的数据
volatile uint8_t tx_buffer[I2C_RECEIVE_BUFFER_LEN];

volatile uint16_t ubReceiveIndex  = 0;  // Current RX index / 当前接收索引
volatile uint8_t i2c_addr         = 0;  // Local copy of Slave Address (Left aligned) / 从机地址本地副本 (左对齐)
volatile uint16_t tx_buffer_index = 0;  // Current TX index / 当前发送索引
volatile uint16_t tx_len          = 0;  // Total length to send / 发送总长度

// Timeout Watchdogs / 超时看门狗
volatile uint32_t i2c_timeout_counter      = 0;
volatile uint32_t i2c_stop_timeout_flag    = 0;
volatile uint32_t i2c_stop_timeout_counter = 0;

/**
 * @brief Set the internal slave address variable.
 *        设置内部从机地址变量。
 * @param addr 7-bit I2C address. / 7位 I2C 地址。
 */
void set_i2c_slave_address(uint8_t addr)
{
    // STM32 LL Library expects address left-shifted by 1 (8-bit format)
    // STM32 LL 库需要地址左移 1 位（8 位格式）
    i2c_addr = (addr << 1);
}

/**
 * @brief Weak implementation of the callback. Should be defined in user code.
 *        回调的弱实现。应在用户代码中定义。
 */
__weak void Slave_Complete_Callback(uint8_t *rx_data, uint16_t len)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(rx_data);
    UNUSED(len);
}

/**
 * @brief Initialize I2C1 hardware.
 *        初始化 I2C1 硬件。
 * @note  Configures GPIO (PB6/PB7), Clocks, Interrupts, and I2C Timing.
 *        配置 GPIO (PB6/PB7)、时钟、中断和 I2C 时序。
 */
void user_i2c_init(void)
{
    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    LL_I2C_InitTypeDef I2C_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct    = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the peripherals clocks
     */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
    PeriphClkInit.I2c1ClockSelection   = RCC_I2C1CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }

    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    /**I2C1 GPIO Configuration
    PB6   ------> I2C1_SCL
    PB7   ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_6;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate  = LL_GPIO_AF_6;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin        = LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate  = LL_GPIO_AF_6;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    /* I2C1 interrupt Init */
    NVIC_SetPriority(I2C1_IRQn, 0);
    NVIC_EnableIRQ(I2C1_IRQn);

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */

    /** I2C Initialization
     */
    I2C_InitStruct.PeripheralMode  = LL_I2C_MODE_I2C;
    I2C_InitStruct.Timing          = 0x00910B1C;
    I2C_InitStruct.AnalogFilter    = LL_I2C_ANALOGFILTER_ENABLE;
    I2C_InitStruct.DigitalFilter   = 0;
    I2C_InitStruct.OwnAddress1     = (i2c_addr_reg << 1);
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize     = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(I2C1, &I2C_InitStruct);
    LL_I2C_EnableAutoEndMode(I2C1);
    LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);
    LL_I2C_DisableOwnAddress2(I2C1);
    LL_I2C_DisableGeneralCall(I2C1);
    LL_I2C_EnableClockStretching(I2C1);

    /** I2C Fast mode Plus enable
     */
    LL_SYSCFG_EnableFastModePlus(LL_SYSCFG_I2C_FASTMODEPLUS_I2C1);
    /* USER CODE BEGIN I2C1_Init 2 */
    set_i2c_slave_address(i2c_addr_reg);
    /* USER CODE END I2C1_Init 2 */
}

void i2c1_it_enable(void)
{
    LL_I2C_Enable(I2C1);
    LL_I2C_EnableIT_ADDR(I2C1);  // Address Match / 地址匹配
    LL_I2C_EnableIT_NACK(I2C1);  // NACK received / 接收到 NACK
    LL_I2C_EnableIT_ERR(I2C1);   // Error / 错误
    LL_I2C_EnableIT_STOP(I2C1);  // Stop condition / 停止条件
}

void i2c1_it_disable(void)
{
    LL_I2C_DisableIT_ADDR(I2C1);
    LL_I2C_DisableIT_NACK(I2C1);
    LL_I2C_DisableIT_ERR(I2C1);
    LL_I2C_DisableIT_STOP(I2C1);
}

void Error_Callback(void)
{
    // Placeholder for error handling
    // 错误处理占位符
    // i2c1_it_enable();
    // LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
}

void i2c1_set_send_data(uint8_t *tx_ptr, uint16_t len)
{
    if (len > I2C_RECEIVE_BUFFER_LEN) {
        len = I2C_RECEIVE_BUFFER_LEN;
    }

    if (len == 0 || tx_ptr == NULL) {
        return;
    }
    memcpy((void *)tx_buffer, tx_ptr, len);
    tx_buffer_index = 0;
    tx_len          = len;
}

void Slave_Reception_Callback(void)
{
    aReceiveBuffer[ubReceiveIndex++] = LL_I2C_ReceiveData8(I2C1);
}

void Slave_Ready_To_Transmit_Callback(void)
{
    LL_I2C_TransmitData8(I2C1, tx_buffer[tx_buffer_index]);
    tx_buffer_index++;
    if (tx_buffer_index >= tx_len) {
        tx_buffer_index = 0;
    }
}

void I2C1_IRQHandler(void)
{
    /* USER CODE BEGIN I2C1_IRQn 0 */
    i2c_timeout_counter++;
    if (i2c_timeout_counter > 10000) {
        LL_I2C_DeInit(I2C1);
        LL_I2C_DisableAutoEndMode(I2C1);
        LL_I2C_Disable(I2C1);
        LL_I2C_DisableIT_ADDR(I2C1);
        user_i2c_init();
        i2c1_it_enable();
        i2c_timeout_counter = 0;
    }
    /* Check ADDR flag value in ISR register */
    if (LL_I2C_IsActiveFlag_ADDR(I2C1)) {
        /* Verify the Address Match with the OWN Slave address */
        if (LL_I2C_GetAddressMatchCode(I2C1) == i2c_addr) {
            if (ubReceiveIndex) {
                i2c1_it_disable();
                Slave_Complete_Callback((uint8_t *)aReceiveBuffer, ubReceiveIndex);
                ubReceiveIndex = 0;
                i2c1_it_enable();
            }
            /* Verify the transfer direction, a write direction, Slave enters receiver
             * mode */
            if (LL_I2C_GetTransferDirection(I2C1) == LL_I2C_DIRECTION_WRITE) {
                /* Clear ADDR flag value in ISR register */
                LL_I2C_ClearFlag_ADDR(I2C1);

                /* Enable Receive Interrupt */
                LL_I2C_EnableIT_RX(I2C1);
                i2c_stop_timeout_flag = 1;
            }
            /* Verify the transfer direction, a read direction, Slave enters
               transmitter mode */
            else if (LL_I2C_GetTransferDirection(I2C1) == LL_I2C_DIRECTION_READ) {
                /* Clear ADDR flag value in ISR register */
                LL_I2C_ClearFlag_ADDR(I2C1);

                /* Enable Transmit Interrupt */
                LL_I2C_EnableIT_TX(I2C1);
            } else {
                /* Clear ADDR flag value in ISR register */
                LL_I2C_ClearFlag_ADDR(I2C1);

                /* Call Error function */
                Error_Callback();
            }
        } else {
            /* Clear ADDR flag value in ISR register */
            LL_I2C_ClearFlag_ADDR(I2C1);

            /* Call Error function */
            Error_Callback();
        }
    }
    /* Check NACK flag value in ISR register */
    else if (LL_I2C_IsActiveFlag_NACK(I2C1)) {
        /* End of Transfer */
        LL_I2C_ClearFlag_NACK(I2C1);
    }
    /* Check TXIS flag value in ISR register */
    else if (LL_I2C_IsActiveFlag_TXIS(I2C1)) {
        /* Call function Slave Ready to Transmit Callback */
        Slave_Ready_To_Transmit_Callback();
    }
    /* Check RXNE flag value in ISR register */
    else if (LL_I2C_IsActiveFlag_RXNE(I2C1)) {
        /* Call function Slave Reception Callback */
        Slave_Reception_Callback();
    }
    /* Check STOP flag value in ISR register */
    else if (LL_I2C_IsActiveFlag_STOP(I2C1)) {
        /* End of Transfer */
        LL_I2C_ClearFlag_STOP(I2C1);

        /* Check TXE flag value in ISR register */
        if (!LL_I2C_IsActiveFlag_TXE(I2C1)) {
            /* Flush the TXDR register */
            LL_I2C_ClearFlag_TXE(I2C1);
        }

        i2c1_it_disable();
        /* Call function Slave Complete Callback */
        Slave_Complete_Callback((uint8_t *)aReceiveBuffer, ubReceiveIndex);
        ubReceiveIndex = 0;
        i2c1_it_enable();
        i2c_stop_timeout_flag    = 0;
        i2c_stop_timeout_counter = 0;
    }
    /* Check TXE flag value in ISR register */
    else if (!LL_I2C_IsActiveFlag_TXE(I2C1)) {
        /* Do nothing */
        /* This Flag will be set by hardware when the TXDR register is empty */
        /* If needed, use LL_I2C_ClearFlag_TXE() interface to flush the TXDR
         * register  */
    } else {
        /* Call Error function */
        Error_Callback();
    }
    /* USER CODE END I2C1_IRQn 0 */

    /* USER CODE BEGIN I2C1_IRQn 1 */

    /* USER CODE END I2C1_IRQn 1 */
}
