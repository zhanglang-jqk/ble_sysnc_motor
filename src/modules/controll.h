/***********************************************************************
 * @file controll.h
 * CONTROLL
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2021-01-20  .cdWFVCEL. all right reserved
***********************************************************************/

#ifndef __CONTROLL_H_
#define __CONTROLL_H_
/* 包含头文件 ------------------------------------------------------------------*/
#include "ch/bsp.h"
/* 宏定义 ----------------------------------------------------------------------*/
/* 类型定义 --------------------------------------------------------------------*/
#define LED_UP_PIN      33
#define LED_DOWN_PIN    32

#define M1_TOP_PIN      22
#define M1_BOTTOM_PIN   21
#define M2_TOP_PIN      25
#define M2_BOTTOM_PIN   26

#define KEY_UP_PIN      35
#define KEY_DOWN_PIN    34

// fade LED PIN (replace with LED_BUILTIN constant for built-in LED)
#define M1_PWNA 5
#define M1_PWNB 17
#define M2_PWNA 19
#define M2_PWNB 18

#define M1_SA_PIN   16
#define M2_SA_PIN   4

#define CTRL_PWM_DUTY   100 //驱动占空比

#define OPEN_UP_LED() digitalWrite(LED_UP_PIN, LOW)
#define CLOSE_UP_LED() digitalWrite(LED_UP_PIN, HIGH)
#define OPEN_DOWN_LED() digitalWrite(LED_DOWN_PIN, LOW)
#define CLOSE_DOWN_LED() digitalWrite(LED_DOWN_PIN, HIGH)

#define CTRL_SYNC_WAIT_PLUSE 30    //同步等待脉冲数
#define BRAKE_DELAY 30              //制动延时
/* 变量声明 --------------------------------------------------------------------*/
extern u8 upload_f;
extern u8 download_f;

extern bool m1Topfalling_f;
extern bool m1Bottomfalling_f;
extern bool m2Topfalling_f;
extern bool m2Bottomfalling_f;
/* 函数声明 --------------------------------------------------------------------*/
void CTRL_Init();
void CTRL_CommunicationControllProg();
void CTRL_Upload();
void CTRL_Download();
void CTRL_Stop();

void CTRL_M1Upload();
void CTRL_M1Download();
void CTRL_M1Stop();
void CTRL_M2Upload();
void CTRL_M2Download();
void CTRL_M2Stop();

void CTRL_CtrlProcessing();
#endif // __CONTROLL_H_
