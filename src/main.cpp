#include "modules/communication.h"
#include "modules/controll.h"

#define LED_PIN 27
void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("start running...");

  CTRL_Init();
  COMM_BLE_Init();

  for (int i = 0; i < 5; i++)
  {
    OPEN_UP_LED(), OPEN_DOWN_LED();
    vTaskDelay(100);
    CLOSE_UP_LED(), CLOSE_DOWN_LED();
    vTaskDelay(100);
  }
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