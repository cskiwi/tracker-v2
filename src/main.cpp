#include <Arduino.h>
#include <Ticker.h>
#include "LoggerController.h"
#include "TimeController.h"
#include "WifiController.h"
#include "ApiController.h"

#define PROD true
LoggerController loggerController;
// RecordController recordController;
ApiController apiController;
Tracker tracker = Tracker("tracker-15");

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
// void recordDb();
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

float sound_pressure_refference = 0.023;

// tick every 10 minutes
Ticker recordTicker(recordAudio, recordInterval);
// tick every 10 seconds
Ticker apiTicker(checkApi, apiInterval);

// tick every second
// Ticker testTicker(recordDb, 10 * 1000);

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

  // test
  // testTicker.start();

  // recordAudio();s

  blinkLed(1);

  Serial.println("[" + TimeController::getFormattedDateTime() + "] Setup done");
}

void loop()
{
  recordTicker.update();
  apiTicker.update();
  // testTicker.update();
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

// void recordDb()
// {

//   try
//   {

//     // print sound pressure to .0000001 precision
//     Serial.println("[" + TimeController::getFormattedDateTime() + "] Running dba for pressure: " + String(sound_pressure_refference, 7) + " Pa");

//     // run 10 dba mesuremnets
//     for (int i = 0; i < 10; i++)
//     {
//       SAMPLE_T *sample = recordController.recordSample();
//       float dba = DecibelController::calculateDecibel(sample, sound_pressure_refference);
//       Serial.println("[" + TimeController::getFormattedDateTime() + "] Dba: " + String(dba) + " dB(A)");
//       // free sample memory
//       free(sample);
//     }

//     // slightly increase sound pressure
//     sound_pressure_refference += 0.00001;
//   }

//   catch (const std::exception &e)
//   {
//     blinkLed(10);
//     ESP.deepSleep(0);
//   }
// }

// void recordSoundFragment()
// {

//   try
//   {
//     loggerController.log(10 * 60 * 1000, gain);

//     // gain += 1.0;

//     Serial.println("[" + TimeController::getFormattedDateTime() + "] Gain is now: " + String(gain));
//   }

//   catch (const std::exception &e)
//   {
//     blinkLed(10);
//     ESP.deepSleep(0);
//   }
// }