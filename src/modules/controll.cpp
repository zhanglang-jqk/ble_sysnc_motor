/***********************************************************************
 * @file 	:	controll.cpp
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2021-01-20  .cdWFVCEL. all right reserved
***********************************************************************/

/* 包含头文件 -------------------------------------------------------------------*/
#include "controll.h"
#include "communication.h"
/* 宏定义 -----------------------------------------------------------------------*/
/* 类型定义 ---------------------------------------------------------------------*/
/* 私有变量 ---------------------------------------------------------------------*/

#define KEY_DELAY 5
/* 扩展变量 ---------------------------------------------------------------------*/
/* 私有函数声明 -----------------------------------------------------------------*/
void m1Topfalling();
void m1Bottomfalling();
void m2Topfalling();
void m2Bottomfalling();
void keyDownPress();
void keyUpPress();
void m1SaFalling();
void m2SaFalling();

/* 函数声明 ---------------------------------------------------------------------*/
/* 函数实现 ---------------------------------------------------------------------*/

// use first channel of 16 channels (started from zero)
#define M1_PWMA_LEDC_CHANNEL 0
#define M1_PWMB_LEDC_CHANNEL 1
#define M2_PWMA_LEDC_CHANNEL 2
#define M2_PWMB_LEDC_CHANNEL 3

// use 13 bit precission for LEDC timer
#define LEDC_TIMER_13_BIT 13

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ 2000

u8 upload_f = false;
u8 download_f = false;
bool m1TopTrigger_f = false;
bool m1BottomTrigger_f = false;
bool m2TopTrigger_f = false;
bool m2BottomTrigger_f = false;

u32 m1SaFalling_c = 0;
u32 m2SaFalling_c = 0;

bool keyUpPress_f = false;
bool keyDownPress_f = false;

enum RunStatue_E
{
    STOP,
    UPLOAD,
    DOWNLOAD
};

RunStatue_E runStatue_e = RunStatue_E::STOP;
// Arduino like analogWrite
// value has to be between 0 and valueMax
void pluesWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 100)
{
    // calculate duty, 8191 from 2 ^ 13 - 1
    uint32_t duty = (8191.0 / valueMax) * min(value, valueMax);

    // write duty to LEDC
    ledcWrite(channel, duty);
}

TaskHandle_t keyScanTaskHandle = NULL;

void keyScanTask(void *pvParameters)
{
    while (1)
    {
        // if (loopTaskWDTEnabled)
        // {
        //     esp_task_wdt_reset();
        // }

        if (keyUpPress_f)
        {
            vTaskDelay(5);
            if (digitalRead(KEY_UP_PIN) == 0)
            {
                upload_f = true;
                while (digitalRead(KEY_UP_PIN) == 0)
                {
                    vTaskDelay(10);
                }
                keyUpPress_f = false, upload_f = false;
            }
        }
        if (keyDownPress_f)
        {
            vTaskDelay(5);
            if (digitalRead(KEY_DOWN_PIN) == 0)
            {
                download_f = true;
                while (digitalRead(KEY_DOWN_PIN) == 0)
                {
                    vTaskDelay(10);
                }
                keyDownPress_f = false, download_f = false;
            }
        }

        vTaskDelay(10);
    }
}
void CTRL_Init()
{
    pinMode(LED_UP_PIN, OUTPUT);
    pinMode(LED_DOWN_PIN, OUTPUT);
    CLOSE_DOWN_LED(), CLOSE_UP_LED();

    pinMode(M1_TOP_PIN, INPUT_PULLUP);
    pinMode(M1_BOTTOM_PIN, INPUT_PULLUP);
    pinMode(M2_TOP_PIN, INPUT_PULLUP);
    pinMode(M2_BOTTOM_PIN, INPUT_PULLUP);
    attachInterrupt(M1_TOP_PIN, m1Topfalling, FALLING);
    attachInterrupt(M1_BOTTOM_PIN, m1Bottomfalling, FALLING);
    attachInterrupt(M2_TOP_PIN, m2Topfalling, FALLING);
    attachInterrupt(M2_BOTTOM_PIN, m2Bottomfalling, FALLING);
    // attachInterrupt(M1_TOP_PIN, m1Topfalling, ONLOW);
    // attachInterrupt(M1_BOTTOM_PIN, m1Bottomfalling, ONLOW);
    // attachInterrupt(M2_TOP_PIN, m2Topfalling, ONLOW);
    // attachInterrupt(M2_BOTTOM_PIN, m2Bottomfalling, ONLOW);

    pinMode(KEY_UP_PIN, INPUT_PULLUP);
    pinMode(KEY_DOWN_PIN, INPUT_PULLUP);
    attachInterrupt(KEY_UP_PIN, keyUpPress, FALLING);
    attachInterrupt(KEY_DOWN_PIN, keyDownPress, FALLING);

    xTaskCreateUniversal(keyScanTask, "keyScanTask", 1024, NULL, 1, &keyScanTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);

    // Setup timer and attach timer to a led pin
    // ledcSetup(M1_PWMA_LEDC_CHANNEL, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    // ledcSetup(M1_PWMB_LEDC_CHANNEL, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    // ledcSetup(M2_PWMA_LEDC_CHANNEL, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    // ledcSetup(M2_PWMB_LEDC_CHANNEL, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);

    // ledcAttachPin(M1_PWNA, M1_PWMA_LEDC_CHANNEL);
    // ledcAttachPin(M1_PWNB, M1_PWMB_LEDC_CHANNEL);
    // ledcAttachPin(M2_PWNA, M2_PWMA_LEDC_CHANNEL);
    // ledcAttachPin(M2_PWNB, M2_PWMB_LEDC_CHANNEL);

    pinMode(M1_PWNA, OUTPUT);
    pinMode(M1_PWNB, OUTPUT);
    pinMode(M2_PWNA, OUTPUT);
    pinMode(M2_PWNB, OUTPUT);

    pinMode(M1_SA_PIN, INPUT_PULLUP);
    pinMode(M2_SA_PIN, INPUT_PULLUP);
    attachInterrupt(M1_SA_PIN, m1SaFalling, FALLING);
    attachInterrupt(M2_SA_PIN, m2SaFalling, FALLING);
}

void CTRL_Upload()
{
    // CLOSE_DOWN_LED(), OPEN_UP_LED();
    CTRL_M1Upload(), CTRL_M2Upload();
    runStatue_e = RunStatue_E::UPLOAD;
}
void CTRL_Download()
{
    // CLOSE_UP_LED(), OPEN_DOWN_LED();
    CTRL_M1Download(), CTRL_M2Download();
    runStatue_e = RunStatue_E::DOWNLOAD;
}
void CTRL_Stop()
{
    // CLOSE_UP_LED(), CLOSE_DOWN_LED();
    if (runStatue_e == RunStatue_E::UPLOAD)
    {
        CTRL_M1Download(), CTRL_M2Download(); //刹车制动延时
        vTaskDelay(BRAKE_DELAY);
    }
    else if (runStatue_e == RunStatue_E::DOWNLOAD)
    {
        CTRL_M1Upload(), CTRL_M2Upload(); //刹车制动延时
        vTaskDelay(BRAKE_DELAY);
    }
    CTRL_M1Stop(), CTRL_M2Stop();

    runStatue_e = RunStatue_E::STOP;
}

void CTRL_M1Upload()
{
    // pluesWrite(M1_PWMA_LEDC_CHANNEL, 0);
    // pluesWrite(M1_PWMB_LEDC_CHANNEL, CTRL_PWM_DUTY);

    digitalWrite(M1_PWNA, LOW);
    digitalWrite(M1_PWNB, HIGH);
    // Serial.println("m1 upload");
}
void CTRL_M1Download()
{
    // pluesWrite(M1_PWMA_LEDC_CHANNEL, CTRL_PWM_DUTY);
    // pluesWrite(M1_PWMB_LEDC_CHANNEL, 0);

    digitalWrite(M1_PWNA, HIGH);
    digitalWrite(M1_PWNB, LOW);

    // Serial.println("m1 download");
}
void CTRL_M1Stop()
{
    // pluesWrite(M1_PWMA_LEDC_CHANNEL, 0);
    // pluesWrite(M1_PWMB_LEDC_CHANNEL, 0);

    digitalWrite(M1_PWNA, LOW);
    digitalWrite(M1_PWNB, LOW);

    // Serial.println("m1 stop");
}
void CTRL_M2Upload()
{
    // pluesWrite(M2_PWMA_LEDC_CHANNEL, 0);
    // pluesWrite(M2_PWMB_LEDC_CHANNEL, CTRL_PWM_DUTY);

    digitalWrite(M2_PWNA, LOW);
    digitalWrite(M2_PWNB, HIGH);

    // Serial.println("m2 upload");
}
void CTRL_M2Download()
{
    // pluesWrite(M2_PWMA_LEDC_CHANNEL, CTRL_PWM_DUTY);
    // pluesWrite(M2_PWMB_LEDC_CHANNEL, 0);

    digitalWrite(M2_PWNA, HIGH);
    digitalWrite(M2_PWNB, LOW);

    // Serial.println("m2 download");
}
void CTRL_M2Stop()
{
    // pluesWrite(M2_PWMA_LEDC_CHANNEL, 0);
    // pluesWrite(M2_PWMB_LEDC_CHANNEL, 0);

    digitalWrite(M2_PWNA, LOW);
    digitalWrite(M2_PWNB, LOW);

    // Serial.println("m2 stop");
}

void m1Topfalling()
{
    m1TopTrigger_f = true;
}
void m1Bottomfalling()
{
    m1BottomTrigger_f = true;
}
void m2Topfalling()
{
    m2TopTrigger_f = true;
}
void m2Bottomfalling()
{
    m2BottomTrigger_f = true;
}
void keyUpPress()
{
    keyUpPress_f = true;
}
void keyDownPress()
{
    keyDownPress_f = true;
}

void m1SaFalling()
{
    // Serial.println("m1 sa");
    m1SaFalling_c++;
}
void m2SaFalling()
{
    // Serial.println("m2 sa");
    m2SaFalling_c++;
}

void CTRL_CtrlProcessing()
{
    if (upload_f) //{} key set upload
    {
        // Serial.println("upload precessing");
        CLOSE_DOWN_LED(), OPEN_UP_LED();
        // CTRL_M1Upload(), CTRL_M2Upload();
        CTRL_Upload();
        // while (digitalRead(KEY_UP_PIN) == 0) //wait key up
        while (upload_f)
        {
            // Serial.printf("%m1:%d m2:%d \r\n", m1SaFalling_c, m2SaFalling_c);
            if (m1TopTrigger_f && m2TopTrigger_f)
            {
                // break;
            }
            else if (m1TopTrigger_f)
            {
                CTRL_M1Stop();
            }
            else if (m2TopTrigger_f)
            {
                CTRL_M2Stop();
            }
            else //{} not top trigger
            {
                if ((m1SaFalling_c > m2SaFalling_c) && (m1SaFalling_c - m2SaFalling_c > CTRL_SYNC_WAIT_PLUSE))
                {
                    CTRL_M1Stop(), CTRL_M2Upload();
                }
                else if ((m2SaFalling_c > m1SaFalling_c) && (m2SaFalling_c - m2SaFalling_c > CTRL_SYNC_WAIT_PLUSE))
                {
                    CTRL_M2Stop(), CTRL_M1Upload();
                }
                else if (m1SaFalling_c == m2SaFalling_c)
                {
                    CTRL_M1Upload(), CTRL_M2Upload();
                }
            }
        }
        // if (m1TopTrigger_f && m2TopTrigger_f)
        // {
        //     Serial.println("upload top");
        //     CTRL_M1Upload(), CTRL_M2Upload();
        //     vTaskDelay(2000);
        // }
        CLOSE_UP_LED(), CLOSE_DOWN_LED();
        // CTRL_M1Stop(), CTRL_M2Stop();
        CTRL_Stop();
        m1SaFalling_c = 0, m2SaFalling_c = 0;
        m1TopTrigger_f = false, m2TopTrigger_f = false, m1BottomTrigger_f = false, m2BottomTrigger_f = false;
        // upload_f = false;
    }

    if (download_f) //{} key set upload
    {
        // Serial.println("download processing");
        CLOSE_UP_LED(), OPEN_DOWN_LED();
        // CTRL_M1Download(), CTRL_M2Download();
        CTRL_Download();
        // while (digitalRead(KEY_DOWN_PIN) == 0) //wait key down
        while (download_f)
        {
            // Serial.printf("%m1:%d m2:%d \r\n", m1SaFalling_c, m2SaFalling_c);
            if (m1BottomTrigger_f && m2BottomTrigger_f)
            {
                // break;
            }
            else if (m1BottomTrigger_f)
            {
                CTRL_M1Stop();
            }
            else if (m2BottomTrigger_f)
            {
                CTRL_M2Stop();
            }
            else //{} not top trigger
            {
                if ((m1SaFalling_c > m2SaFalling_c) && (m1SaFalling_c - m2SaFalling_c > CTRL_SYNC_WAIT_PLUSE))
                {
                    CTRL_M1Stop(), CTRL_M2Download();
                }
                else if ((m2SaFalling_c > m1SaFalling_c) && (m2SaFalling_c - m1SaFalling_c > CTRL_SYNC_WAIT_PLUSE))
                {
                    CTRL_M2Stop(), CTRL_M1Download();
                }
                else if (m1SaFalling_c == m2SaFalling_c)
                {
                    CTRL_M1Download(), CTRL_M2Download();
                }
            }
        }
        // if (m1BottomTrigger_f && m2BottomTrigger_f) //{} 下降到底
        // {
        //     Serial.println("download bottom");
        //     CTRL_M1Download(), CTRL_M2Download();
        //     vTaskDelay(2000);
        // }
        CLOSE_UP_LED(), CLOSE_DOWN_LED();
        CTRL_Stop();
        m1SaFalling_c = 0, m2SaFalling_c = 0;
        m1BottomTrigger_f = false, m2BottomTrigger_f = false, m1TopTrigger_f = false, m2TopTrigger_f = false;
        // download_f = false;
    }
}

//controll.cpp