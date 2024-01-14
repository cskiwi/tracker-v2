#ifndef __TimeController_H
#define __TimeController_H
#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>


class TimeController
{
public:
  static void setInternalTime();
  static String getFormattedDateTime();
  static String getFormattedFile();
  static unsigned long getEpochTime();

private:
    static WiFiUDP udp;
    static NTPClient ntpClient;
};

#endif