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

#define ServoTypePos 0
#define ServoTypeSpeed 1

volatile mavlink_upper_t *UpperTaskData;
mavlink_upper_t UpperInitData = {0};
mavlink_upper_t *UpperRxData = NULL;

CLAW_OC_t claw_oc = {
	.max_current_init = 2000,
	.max_current_normal = 1500,
	.max_speed = 3000,
	// .jimu[0][0] = ,
	// .jimu[0][1] = ,
	.jimu[1][0] = 4283.6,
	// .jimu[1][1] = ,
	// .jimu[2][0] = ,
	// .jimu[2][1] = ,
	// .jimu[3][0] = ,
	// .jimu[3][1] = ,
	// .jimu[4][0] = ,
	// .jimu[4][1] = ,
};

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
	hDJI[2].speedPID.outputMax = claw_oc.max_current_init;
	hDJI[2].posPID.outputMax = claw_oc.max_speed;

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
}

/**
 * @brief 机械初始化
 * 
 * @param argument 
 */
void Mec_InitTask(void const *argument)
{
	osDelay(500);
	mavlink_upper_t *UpperRxData = (mavlink_upper_t *)argument;
	UpperInitData.servo_type = (ServoTypePos << 0) |
							   (ServoTypePos << 1) |
							   (ServoTypePos << 2);
	
	// 爪子开闭 DJI
	hDJI[2].speedPID.outputMax = claw_oc.max_current_init;
	int counter = 0;

	// // 闭合
	// counter = 0;
	// UpperInitData.claw_OC_DJI = -1000;

	// while (counter < 200)
	// {
	// 	if (fabs(hDJI[2].FdbData.rpm) < 20)
	// 	{
	// 		counter++;
	// 	}
	// 	else
	// 	{
	// 		counter = 0;
	// 	}
	// 	osDelay(5);
	// }
	// claw_oc.min_pos = hDJI[2].Calculate.RotorAngle_all;

	// 张开
	UpperInitData.claw_OC_DJI = 590;

	counter = 0;
	while (counter < 200)
	{
		if (fabs(hDJI[2].FdbData.rpm) < 500)
		{
			counter++;
		}
		else
		{
			counter = 0;
		}
		osDelay(5);
	}
	claw_oc.max_pos = hDJI[2].Calculate.RotorAngle_all;

	osDelay(1000);

	UpperInitData.claw_OC_DJI = 10;

	while (1)
	{
		osDelay(10);
	}
	

	// UpperTaskData = UpperRxData;
	vTaskDelete(NULL);
}

void UpperTask(void const *argument)
{
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

void UpperTestTask(void const *argument)
{
	while (1)
	{
		UD_printf("RotorAngle_all:%f\n", hDJI[2].Calculate.RotorAngle_all);
		osDelay(500);
	}
	
}

void UpperTaskStart(mavlink_upper_t *UpperData)
{
	UpperTaskData = &UpperInitData;

	osThreadDef(upper, UpperTask, osPriorityNormal, 0, 1024);
	osThreadCreate(osThread(upper), NULL);

	osThreadDef(mec_init, Mec_InitTask, osPriorityNormal, 0, 1024);
	osThreadCreate(osThread(mec_init), UpperRxData);

	osThreadDef(upper_test, UpperTestTask, osPriorityBelowNormal, 0, 256);
	osThreadCreate(osThread(upper_test), NULL);
}
