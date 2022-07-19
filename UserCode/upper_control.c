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
	hDJI[2].posPID.outputMax = 2000;

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
}

void UpperTask(void const *argument)
{
	mavlink_upper_t *UpperTaskData = (mavlink_upper_t *)argument;
	uint32_t PreviousWakeTime = osKernelSysTick();
	for (;;)
	{
		// 升降架
		if (UpperTaskData->servo_type & 1)
		{
			speedServo(UpperTaskData->lift, &hDJI[0]);
		}
		else
		{
			positionServo(UpperTaskData->lift, &hDJI[0]);
		}

		// 爪子开合（大疆电机）
		if (UpperTaskData->servo_type & (1 << 1))
		{
			speedServo(UpperTaskData->claw_OC_DJI, &hDJI[2]);
		}
		else
		{
			positionServo(UpperTaskData->claw_OC_DJI, &hDJI[2]);
		}

		// 爪子舵机
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, UpperTaskData->claw_OC_L); // PA5
		__HAL_TIM_SetCompare(&htim16, TIM_CHANNEL_1, UpperTaskData->claw_OC_R); // PA6

		// 爪子旋转
		if (UpperTaskData->servo_type & (1 << 2))
		{
			speedServo(UpperTaskData->claw_spin, &hDJI[1]);
		}
		else
		{
			positionServo(UpperTaskData->claw_spin, &hDJI[1]);
		}

		switch (UpperTaskData->vice_lift)
		{
		case 0:
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0); // 降下来
			break;
		case 1:
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1); // 升上去
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

void UpperTestTask(void const *argument)
{
	while (1)
	{
		// UD_printf("RotorAngle_all:%f\n", hDJI[2].Calculate.RotorAngle_all);
		osDelay(500);
	}
	
}

void UpperTaskStart(mavlink_upper_t *UpperData)
{
	osThreadDef(upper, UpperTask, osPriorityNormal, 0, 1024);
	osThreadCreate(osThread(upper), UpperData);

	osThreadDef(upper_test, UpperTestTask, osPriorityBelowNormal, 0, 256);
	osThreadCreate(osThread(upper_test), NULL);
}
