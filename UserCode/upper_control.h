/**
 * @file upper_control.h
 * @author TITH (1023515576@qq.com)
 * @brief 上层控制
 * @version 0.1
 * @date 2022-07-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "wtr_mavlink.h"

typedef struct
{
	float max_current_normal;
	float max_current_init;
	float max_speed;
	float min_pos; // 爪子到最小的位置
	float max_pos; // 爪子到最大的位置
	float jimu[5][2]; // 积木对应的大小（0号是最大积木，[x][0]是最小值）
} CLAW_OC_t;

void UpperTaskStart(mavlink_upper_t *UpperData);
void UpperTaskInit();