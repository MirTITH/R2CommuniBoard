/**
 * @file wtr_mavlink.h
 * @author TITH (1023515576@qq.com)
 * @brief 移植到 stm32 的 mavlink，只需要包含这个头文件就够了
 * @version 0.2
 * @date 2022-07-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "main.h"
#include "mavlink_types.h"
#include "usart.h"

static mavlink_system_t mavlink_system = {
	1, // System ID (1-255)
	1  // Component ID (a MAV_COMPONENT value)
};

typedef struct
{
	UART_HandleTypeDef *huart;
	uint8_t rx_ch;
	mavlink_channel_t chan;
	mavlink_message_t msg;
	mavlink_status_t status;
} WTR_MAVLink_handle_t;


void WTR_MAVLink_Init(UART_HandleTypeDef *huart, mavlink_channel_t chan);
void WTR_MAVLink_RcvStart(mavlink_channel_t chan);
void WTR_MAVLink_Msg_RxCpltCallback(mavlink_message_t *msg);

extern WTR_MAVLink_handle_t hMAVLink[MAVLINK_COMM_NUM_BUFFERS];

/**
 * @brief 查找 hMAVLink
 *
 * @param huart
 * @return WTR_MAVLink_handle_t* 找不到返回 NULL
 */
static inline WTR_MAVLink_handle_t *Find_MAVLink_COMM(UART_HandleTypeDef *huart)
{
	for (int i = 0; i < MAVLINK_COMM_NUM_BUFFERS; i++)
	{
		if (huart == hMAVLink[i].huart)
			return &hMAVLink[i];
	}
	return NULL;
}

#define MAVLINK_USE_CONVENIENCE_FUNCTIONS
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

/**
 * @brief mavlink 串口发送。mavlink 默认有4个通道，在这里指定每个通道用哪个串口发送
 *
 * @param chan
 * @param buf
 * @param len
 */
static inline void WTR_MAVLINK_SEND_UART_BYTES(mavlink_channel_t chan, const uint8_t *buf, uint16_t len)
{
	UART_HandleTypeDef *huart = NULL;

	if (chan < MAVLINK_COMM_NUM_BUFFERS)
	{
		huart = hMAVLink[chan].huart;
	}

	if (huart != NULL)
	{
		HAL_UART_Transmit(huart, (uint8_t *)buf, len, HAL_MAX_DELAY);
	}
}

#define MAVLINK_SEND_UART_BYTES(chan, buf, len) WTR_MAVLINK_SEND_UART_BYTES(chan, buf, len)
#endif // MAVLINK_USE_CONVENIENCE_FUNCTIONS

#include "mavlink.h"

/**
 * @brief 接收回调，放在串口接收回调函数中
 *
 * @param huart
 */
static inline void WTR_MAVLink_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	WTR_MAVLink_handle_t *hmav = Find_MAVLink_COMM(huart);
	if (hmav == NULL)
		return;

	if (mavlink_parse_char(hmav->chan, hmav->rx_ch, &hmav->msg, &hmav->status))
	{
		WTR_MAVLink_Msg_RxCpltCallback(&hmav->msg);
	}

	HAL_UART_Receive_IT(hmav->huart, &hmav->rx_ch, 1);
}
