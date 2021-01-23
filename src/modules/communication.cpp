/***********************************************************************
 * @file 	:	communication.cpp
 * @author	:	ch
 * @brief	:	
 * @version:	v1.0
 * @Copyright (C)  2021-01-20  .cdWFVCEL. all right reserved
***********************************************************************/

/* 包含头文件 -------------------------------------------------------------------*/
#include "communication.h"
#include "controll.h"

/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleWrite.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
/* 宏定义 -----------------------------------------------------------------------*/

std::string ctrlStatue = "NONE";
/* 类型定义 ---------------------------------------------------------------------*/

class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0)
        {
            Serial.printf("received CMD:");
            if (value == CTRL_CMD_NONE)
            {
                Serial.println(CTRL_CMD_NONE);
            }
            else if (value == CTRL_CMD_UP)
            {
                upload_f = true;
                Serial.println(CTRL_CMD_UP);
            }
            else if (value == CTRL_CMD_DOWN)
            {
                download_f = true;
                Serial.println(CTRL_CMD_DOWN);
            }
            else if (value == CTRL_CMD_STOP)
            {
                upload_f = false, download_f = false;
                Serial.println(CTRL_CMD_STOP);
            }
            ctrlStatue = value;
        }

        // uint8_t *pCtrl = pCharacteristic->getData();
        // if (*pCtrl != NULL)
        // {
        //   switch (*pCtrl)
        //   {
        //   case NONE:
        //     Serial.println("NONE");
        //     break;
        //   case UP:
        //     Serial.println("UP");
        //     break;
        //   case DOWN:
        //     Serial.println("DOWN");
        //     break;
        //   case STOP:
        //     Serial.println("STOP");
        //     break;
        //   }
        // }
    }
};
/* 私有变量 ---------------------------------------------------------------------*/
/* 扩展变量 ---------------------------------------------------------------------*/
/* 私有函数声明 -----------------------------------------------------------------*/
/* 函数声明 ---------------------------------------------------------------------*/
/* 函数实现 ---------------------------------------------------------------------*/
void COMM_BLE_Init()
{
    Serial.println("1- Download and install an BLE scanner app in your phone");
    Serial.println("2- Scan for BLE devices in the app");
    Serial.println("3- Connect to MyESP32");
    Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
    Serial.println("5- See the magic =)");

    BLEDevice::init("sync_motor");
    BLEServer *pServer = BLEDevice::createServer();

    BLEService *pService = pServer->createService(SERVICE_UUID);

    BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

    pCharacteristic->setCallbacks(new MyCallbacks());

    // pCharacteristic->setValue("Hello World");
    pService->start();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
}
//communication.cpp