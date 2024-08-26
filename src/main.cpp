#include <Arduino.h>
#include "hardserver.h"
#include "secret.h"

// Create an instance of the HardServer class on port 80
HardServer secureServer(80);

void setup()
{
  // Start serial communication for debugging
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Setup and start the secure server
  secureServer.setupLoginPage("admini", "passu"); // Set username and password
  secureServer.begin();
}

void loop()
{
  // Server is asynchronous, so no code is needed in the loop
}
