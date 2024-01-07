#include <Arduino.h>
#include <MicrophoneController.h>
#include <Ticker.h>

MicrophoneController microphoneController;

void checkMicrophones()
{
  Serial.println("Checking microphones");

  microphoneController.setData();
} 

Ticker timerMicrophoneController(checkMicrophones, 4000, 10);
void setup()
{
  Serial.begin(112500);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  timerMicrophoneController.start();

  Serial.println("Setup DONE\n\n");
}

void loop()
{
  timerMicrophoneController.update();
}

