#include "TimeController.h"

WiFiUDP TimeController::udp;
NTPClient TimeController::ntpClient(TimeController::udp);

void TimeController::setInternalTime()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected!");
    return;
  }

  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  ntpClient.setTimeOffset(3600);
  ntpClient.begin();
  while (!ntpClient.update())
  {
    ntpClient.forceUpdate();
  }
}

// convert the epoch into a string format YYYY-MM-DD HH:MM:SS
String TimeController::getFormattedDateTime()
{
  unsigned long epochTime = ntpClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  int month = ptm->tm_mon + 1;
  int day = ptm->tm_mday;
  int year = ptm->tm_year + 1900;
  int hour = ptm->tm_hour;
  int minute = ptm->tm_min;
  int second = ptm->tm_sec;

  // create a YYYY-MM-DD HH:MM:SS formatted string
  char dateTimeBuffer[20];
  sprintf(dateTimeBuffer, "%d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
  return dateTimeBuffer;
}