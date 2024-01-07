#ifndef __WifiController_H
#define __WifiController_H
#include <Arduino.h>
#include <WiFi.h>
#include "TimeController.h"

#define CONNECTION_TIMEOUT 20

class WifiController
{
public:
  WifiController();

  static void connectToWifi(const char *ssid, const char *password);
  static void disconnectFromWifi();
};

#endif