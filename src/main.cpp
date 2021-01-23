#include "modules/communication.h"
#include "modules/controll.h"

#define LED_PIN 27
void setup()
{
  // esp_task_wdt_init(10,false);
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("start running...");
  CTRL_Init();
  COMM_BLE_Init();
}

void loop()
{
  // put your main code here, to run repeatedly:
  CTRL_CtrlProcessing();

  static u32 ledToggle_c = millis();
  if (millis() - ledToggle_c > 100)
  {
    digitalRead(LED_PIN) == 0 ? digitalWrite(LED_PIN, HIGH) : digitalWrite(LED_PIN, LOW);
    ledToggle_c = millis();
  }
}