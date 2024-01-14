#ifndef __ApiController_H
#define __ApiController_H
#include <Arduino.h>
#include <WiFi.h>
#include "TimeController.h"
#include "Tracker.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

class ApiController
{
public:
  ApiController();
  void setEndpoint(const char *server, const Tracker &tracker, const char *apiKey);
  void updateTrackerStatus(const Tracker &tracker);
  void pingTrackerStatus(Tracker &tracker);

private:
  const char *_apiKey;
  String _serverPath;
  HTTPClient _client;
};

#endif