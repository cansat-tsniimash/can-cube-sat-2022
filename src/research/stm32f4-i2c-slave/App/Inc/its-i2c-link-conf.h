
#ifndef ITS_I2C_LINK_CONF_H_
#define ITS_I2C_LINK_CONF_H_

#include <stm32f4xx_hal.h>

//! Адрес ведомого в 7мибитном формате, с выравниванием по правому краю
#define I2C_LINK_ADDR (0x1a)

// Проритеты I2C прерываний
#define I2C_LINK_EV_IRQ_PREEM_PRIORITY 1
#define I2C_LINK_EV_IRQ_SUB_PRIORITY 0

#define I2C_LINK_ER_IRQ_PREEM_PRIORITY 1
#define I2C_LINK_ER_IRQ_SUB_PRIORITY 0

//! Размер пакета передаваемого/принимаемого за одну транзакцию
#define I2C_LINK_MAX_PACKET_SIZE	(279)

//! Количество приёмных буферов (каждый по I2C_LINK_PACKET_SIZE байт)
#define I2C_LINK_RXBUF_COUNT	(5)
//! Количество отправных буферов (каждый по I2C_LINK_PACKET_SIZE байт)
#define I2C_LINK_TXBUF_COUNT (5)

//! LL i2c-handle (I2C_Typedef)
#define I2C_LINK_BUS_HANDLE (I2C1)
#define I2C_LINK_DMA_HANDLE (DMA1)
#define I2C_LINK_DMA_STREAM_TX (LL_DMA_STREAM_6)
#define I2C_LINK_DMA_CHANNEL_TX (LL_DMA_CHANNEL_1)
#define I2C_LINK_DMA_STREAM_RX (LL_DMA_STREAM_0)
#define I2C_LINK_DMA_CHANNEL_RX (LL_DMA_CHANNEL_1)

#define I2C_LINK_SDA_PORT GPIOB
#define I2C_LINK_SDA_PIN GPIO_PIN_7

#define I2C_LINK_SCL_PORT GPIOB
#define I2C_LINK_SCL_PIN GPIO_PIN_6


#endif /* ITS_I2C_LINK_CONF_H_ */
