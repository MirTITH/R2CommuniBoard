/**
 * @file user_code.c
 * @author TITH (1023515576@qq.com)
 * @brief 用户代码主文件
 * @version 0.1
 * @date 2022-01-12
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "user_code.h"
#include "CLI.h"
#include "uart_device.h"
#include <math.h>
#include <stdio.h>

#include "DJI.h"
#include "wtr_can.h"
#include "Caculate.h"
#include "wtr_mavlink.h"
#include "string.h"
#include <stdbool.h>
#include "upper_control.h"

void TestTask(void const *argument);

mavlink_upper_t UpperData = {0};

int can_rx_count = 0;

void StartDefaultTask(void const *argument)
{
	osDelay(500);

	// CLI_Init(&huart3);
	// UD_SetPrintfDevice(UD_Find(&huart3));

	osThreadDef(testTask, TestTask, osPriorityNormal, 0, 256);
	osThreadCreate(osThread(testTask), NULL);

	DJI_PID_Init();

	UpperTaskInit();

	DJI_motorType_Init();
	CANFilterInit(&hcan1);

	WTR_MAVLink_Init(&huart1, MAVLINK_COMM_0);
	WTR_MAVLink_RcvStart(MAVLINK_COMM_0);

	UpperTaskStart(&UpperData);

	// ADS1256_Init();

	while (1)
	{
		// UD_printf("YAW:%6d Wz_Raw:%6d Wz_C:%6d V:%6d\n", hhwt1.raw_data.Yaw, hhwt1.raw_data.Wz_Raw, hhwt1.raw_data.Wz_Corrected, hhwt1.raw_data.V);
		// UD_printf("YAW:%f Wz:%f YAW_rad:%f Wz_rad:%f \n", HWT_ReadYawDeg(&hhwt1), HWT_ReadWzDeg(&hhwt1), HWT_ReadYawRad(&hhwt1), HWT_ReadWzRad(&hhwt1));
		osDelay(1000);
	}
}

bool pnt_UpperData = false;
bool pnt_can_rx_count = false;
void TestTask(void const *argument)
{
	for (;;)
	{
		if (pnt_UpperData)
		{
			UD_printf("servo_type:%x claw_OC_DJI:%5g claw_OC_L:%4d claw_OC_R:%4d claw_spin:%5g lift:%5g vice_lift:%d\n",
					  UpperData.servo_type,
					  UpperData.claw_OC_DJI,
					  UpperData.claw_OC_L,
					  UpperData.claw_OC_R,
					  UpperData.claw_spin,
					  UpperData.lift,
					  UpperData.vice_lift);
		}

		if (pnt_can_rx_count)
		{
			UD_printf("can_rx_count:%d\n", can_rx_count);
		}

		osDelay(500);
	}
}
