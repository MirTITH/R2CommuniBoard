/**
 * @file wtr_mavlink.c
 * @author TITH (1023515576@qq.com)
 * @brief 移植到 stm32 的 mavlink
 * @version 0.2
 * @date 2022-07-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "wtr_mavlink.h"

WTR_MAVLink_handle_t hMAVLink[MAVLINK_COMM_NUM_BUFFERS];

/**
 * @brief 初始化收发通道
 *
 * @param huart
 * @param chan
 */
void WTR_MAVLink_Init(UART_HandleTypeDef *huart, mavlink_channel_t chan)
{
	hMAVLink[chan].huart = huart;
	hMAVLink[chan].chan = chan;
}

/**
 * @brief
 *
 * @param huart
 * @param chan
 * @param msg_struct
 */
void WTR_MAVLink_RcvStart(mavlink_channel_t chan)
{
	HAL_UART_Receive_IT(hMAVLink[chan].huart, &hMAVLink[chan].rx_ch, 1);
}

/**
 * @brief MAVLink 消息接收完毕回调函数，需要在这里调用解码函数
 *
 * @param msg
 * @return void
 */
__weak void WTR_MAVLink_Msg_RxCpltCallback(mavlink_message_t *msg)
{
	switch (msg->msgid)
	{
	case 1:
		// id = 1 的消息对应的解码函数
		break;

	case 2:
		// id = 2 的消息对应的解码函数
		break;
	// ......
	default:
		break;
	}
}
