/**
 * @file user_IRQ.c
 * @brief 中断回调函数都写在这里
 * 
 */

#include "main.h"
#include "uart_device.h"
#include "wtr_mavlink.h"

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	UD_TxCpltCallback(huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UD_RxCpltCallback(huart);
	WTR_MAVLink_UART_RxCpltCallback(huart);
}

extern mavlink_upper_t UpperData;
void WTR_MAVLink_Msg_RxCpltCallback(mavlink_message_t *msg)
{
	switch (msg->msgid)
	{
	case 1:
		// id = 1 的消息对应的解码函数
		break;

	case 2:
		mavlink_msg_upper_decode(msg, &UpperData);
		break;
	// ......
	default:
		break;
	}
}