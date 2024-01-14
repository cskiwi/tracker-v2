#include "WifiController.h"

int timeout_counter = 0;

WifiController::WifiController()
{
}

void WifiController::connectToWifi(const char *ssid, const char *password)
{
  // Connect to WiFi network
  Serial.print("Connecting to " + String(ssid) + ".");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {

    Serial.print(".");
    delay(1000);

    timeout_counter++;
    if (timeout_counter >= CONNECTION_TIMEOUT)
    {
      switch (WiFi.status())
      {
      case WL_NO_SHIELD:
        Serial.println("No WiFi shield is present.");
        break;
      case WL_IDLE_STATUS:
        Serial.println("Attempting to connect...");
        break;
      case WL_NO_SSID_AVAIL:
        Serial.println("No SSID available.");
        break;
      case WL_SCAN_COMPLETED:
        Serial.println("Scan Networks is complete.");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("Connection FAILED.");
        break;
      case WL_CONNECTION_LOST:
        Serial.println("Connection LOST.");
        break;
      case WL_DISCONNECTED:
        Serial.println("Device has been DISCONNECTED from the Network.");
        break;
      default:
        Serial.println("UNKNOWN ERROR");
        break;
      }

      ESP.restart();
    }
  }
  Serial.println();
  Serial.println("Connected to WiFi with IP: " + WiFi.localIP().toString());
}

boolean shouldStartRecording()
{
}

void WifiController::disconnectFromWifi()
{
  WiFi.disconnect();
}
