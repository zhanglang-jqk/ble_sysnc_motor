/***********************************************************************
 * @file communication.h
 * COMMUNICATION
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2021-01-20  .cdWFVCEL. all right reserved
***********************************************************************/

#ifndef __COMMUNICATION_H_
#define __COMMUNICATION_H_
/* 包含头文件 ------------------------------------------------------------------*/
#include "ch/bsp.h"

/* 宏定义 ----------------------------------------------------------------------*/
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "9717eb00-c5f6-4c19-821b-8e15469fb7b1"
#define CHARACTERISTIC_UUID "067cc689-3778-43fd-aa60-a63618d88012"

#define CTRL_CMD_NONE "NONE"
#define CTRL_CMD_UP "UP"
#define CTRL_CMD_DOWN "DOWN"
#define CTRL_CMD_STOP "STOP"
/* 类型定义 --------------------------------------------------------------------*/
/* 变量声明 --------------------------------------------------------------------*/
extern std::string ctrlStatue;
/* 函数声明 --------------------------------------------------------------------*/
void COMM_BLE_Init();
#endif // __COMMUNICATION_H_
