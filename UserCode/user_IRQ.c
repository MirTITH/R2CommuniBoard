/**
 * @file user_IRQ.c
 * @brief 中断回调函数都写在这里
 * 
 */

#include "main.h"
#include "uart_device.h"
#include "uart_com.h"
#include "chassis_driver.h"

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	UD_TxCpltCallback(huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UD_RxCpltCallback(huart);
	UC_RxCpltCallback(huart);
}
