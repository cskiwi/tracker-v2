#include <Arduino.h>
#include <Ticker.h>
#include "LoggerController.h"
#include "TimeController.h"
#include "WifiController.h"

LoggerController loggerController;

void recordAudio()
{
  Serial.println("[" + TimeController::getFormattedDateTime() + "] Record Audio start");

  loggerController.log();

  Serial.println("[" + TimeController::getFormattedDateTime() + "] Record Audio end");
}

// tick every 4 seconds
Ticker recordTicker(recordAudio, 4000, 2);

void setup()
{
  Serial.begin(112500);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  const char *ssid = "iPhone van Jolien";
  const char *password = "wifiJolien";

  // WifiController::connectToWifi(ssid, password);
  // TimeController::setInternalTime();
  recordTicker.start();

  Serial.println("[" + TimeController::getFormattedDateTime() + "] Setup done");
}

void loop()
{
  recordTicker.update();
}
