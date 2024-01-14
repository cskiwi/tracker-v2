#include <Arduino.h>
#include <Ticker.h>
#include "LoggerController.h"
#include "TimeController.h"
#include "WifiController.h"
#include "ApiController.h"

#define PROD false
LoggerController loggerController;
ApiController apiController;
Tracker tracker = Tracker("tracker-00");

#if PROD
const char *server = "https://gull.purr.dev"; // Server URL
const int recordInterval = (10 * 60 * 1000);
const int recordDuration = (2 * 60 * 1000);
const int apiInterval = (10 * 1000);
#else
const char *server = "http://192.168.1.235:3001"; // Server URL
const int recordInterval = (1 * 60 * 1000);
const int recordDuration = (30 * 1000);
const int apiInterval = (3 * 1000);
#endif
#define LED_1 LED_BUILTIN
#define BLINK_INTERVAL 300

void recordAudio();
void checkApi();
void blinkLed(int times, uint8_t pin = LED_1)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(pin, HIGH);
    delay(BLINK_INTERVAL);
    digitalWrite(pin, LOW);
    delay(BLINK_INTERVAL);
  }
}

// tick every 10 minutes
Ticker recordTicker(recordAudio, recordInterval);
// tick every 10 seconds
Ticker apiTicker(checkApi, apiInterval);

void setup()
{
  pinMode(LED_1, OUTPUT);

  Serial.begin(112500);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  const char *ssid = "iPhone van Jolien";
  const char *password = "wifiJolien";

  WifiController::connectToWifi(ssid, password);
  TimeController::setInternalTime();
  apiController.setEndpoint(server, tracker, "1234567890");

  apiTicker.start();

  blinkLed(1);

  Serial.println("[" + TimeController::getFormattedDateTime() + "] Setup done");
}

void loop()
{
  recordTicker.update();
  apiTicker.update();
}

// real ticker functions
void recordAudio()
{
  Serial.println("[" + TimeController::getFormattedDateTime() + "] Record Audio start");

  try
  {
    loggerController.log(recordDuration);
  }
  catch (const std::exception &e)
  {
    blinkLed(10);
    ESP.deepSleep(0);
  }

  Serial.println("[" + TimeController::getFormattedDateTime() + "] Record Audio end");
}

void checkApi()
{
  Serial.println("[" + TimeController::getFormattedDateTime() + "] Checking api");

  apiController.pingTrackerStatus(tracker);

  Serial.println("[" + TimeController::getFormattedDateTime() + "] Should log: " + tracker.shouldLog);

  if (tracker.shouldLog)
  {
    tracker.shouldLog = false;
    tracker.startedLogOn = TimeController::getEpochTime();
    apiController.updateTrackerStatus(tracker);

    blinkLed(3);

    WifiController::disconnectFromWifi();

    // start the interval
    recordTicker.start();
    // start recording emmediately
    recordAudio();

    apiTicker.stop();
  }

  Serial.println("[" + TimeController::getFormattedDateTime() + "] Check api end");
}