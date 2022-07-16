/**
 * @file upper_control.c
 * @author TITH (1023515576@qq.com)
 * @brief 上层控制
 * @version 0.1
 * @date 2022-07-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "main.h"
#include "DJI.h"
#include "Caculate.h"
#include "upper_control.h"
#include "cmsis_os.h"
#include "math.h"
#include "uart_device.h"
#include "tim.h"
#include <stdbool.h>

void UpperTaskInit()
{
	// 升降
	hDJI[0].motorType = M3508;
	hDJI[0].speedPID.outputMax = 5000;
	hDJI[0].posPID.outputMax = 6000;
	hDJI[0].posPID.KP = 20.0f;

	// 爪子旋转
	hDJI[1].motorType = M2006;

	// 爪子夹具
	hDJI[2].motorType = M3508;
	hDJI[2].speedPID.outputMax = 2000;

	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2);
}

void UpperTask(void const *argument)
{
	const mavlink_upper_t *UpperData = argument;
	// double lift_speed;

	uint32_t PreviousWakeTime = osKernelSysTick();

	for (;;)
	{
		// 升降架
		if (UpperData->servo_type & 1)
		{
			speedServo(UpperData->lift, &hDJI[0]);
		}
		else
		{
			positionServo(UpperData->lift, &hDJI[0]);
		}

		// 爪子开合（大疆电机）
		if (UpperData->servo_type & (1 << 1))
		{
			speedServo(UpperData->claw_OC_DJI, &hDJI[2]);
		}
		else
		{
			positionServo(UpperData->claw_OC_DJI, &hDJI[2]);
		}

		// 爪子舵机
		__HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_1, UpperData->claw_OC_L);
		__HAL_TIM_SetCompare(&htim12, TIM_CHANNEL_2, UpperData->claw_OC_R);

		// 爪子旋转
		if (UpperData->servo_type & (1 << 2))
		{
			speedServo(UpperData->claw_spin, &hDJI[1]);
		}
		else
		{
			positionServo(UpperData->claw_spin, &hDJI[1]);
		}

		switch (UpperData->vice_lift)
		{
		case 0:
			// 降下来
			break;
		case 1:
			// 升上去
			break;
		default:
			break;
		}

		// if (claw_open)
		// {
		// 	Claw_Steer_pw[0] = 1076;
		// 	Claw_Steer_pw[1] = 1195;
		// }
		// else
		// {
		// 	Claw_Steer_pw[0] = 1694;
		// 	Claw_Steer_pw[1] = 1767;
		// }

		CanTransmit_DJI_1234(&hcan1,
							 hDJI[0].speedPID.output,
							 hDJI[1].speedPID.output,
							 hDJI[2].speedPID.output,
							 hDJI[3].speedPID.output);

		osDelayUntil(&PreviousWakeTime, 2);
	}
}

void UpperTaskStart(mavlink_upper_t *UpperData)
{
	osThreadDef(upper, UpperTask, osPriorityBelowNormal, 0, 1024);
	osThreadCreate(osThread(upper), UpperData);

	// osThreadDef(upper_test, UpperTestTask, osPriorityBelowNormal, 0, 256);
	// osThreadCreate(osThread(chassis_test), NULL);
}
