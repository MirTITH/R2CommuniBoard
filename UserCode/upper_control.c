// /**
//  * @file upper_control.c
//  * @author TITH (1023515576@qq.com)
//  * @brief 上层控制
//  * @version 0.1
//  * @date 2022-07-11
//  * 
//  * @copyright Copyright (c) 2022
//  * 
//  */

// void UpperTask(void const *argument)
// {
// 	const UC_Data_t *RxData = argument;
// 	Chassis_Init(wheels);
// 	double joystick_lx, joystick_ly, joystick_rx, joystick_ry;

// 	uint32_t PreviousWakeTime = osKernelSysTick();

// 	for (;;)
// 	{
// 		joystick_lx = RxData->Leftx;
// 		joystick_ly = RxData->Lefty;
// 		joystick_rx = RxData->Rightx;
// 		joystick_ry = RxData->Righty;

// 		if (fabs(joystick_lx) < 500) joystick_lx = 0;
// 		if (fabs(joystick_ly) < 500) joystick_ly = 0;
// 		if (fabs(joystick_rx) < 500) joystick_rx = 0;
// 		if (fabs(joystick_ry) < 500) joystick_ry = 0;

// 		Chassis_SetSpeed(wheels, 4, joystick_lx / 2048.0, joystick_ly / 2048.0, joystick_rx / 2048.0);

// 		osDelayUntil(&PreviousWakeTime, 2);
// 	}	
// }

// void UpperTaskStart(&RxData)
// {
// 	osThreadDef(upper, UpperTask, osPriorityBelowNormal, 0, 256);
// 	osThreadCreate(osThread(chassis), RxData);

// 	osThreadDef(upper_test, UpperTestTask, osPriorityBelowNormal, 0, 256);
// 	osThreadCreate(osThread(chassis_test), NULL);
// }
