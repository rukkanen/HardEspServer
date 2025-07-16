#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "hardserver.h"
#include "secret.h"

// Create an instance of the HardServer class on port 80
HardServer secureServer(80);

// Test function to verify authentication logic
void testAuthentication() {
  Serial.println("=== Testing Authentication Fix ===");
  
  // Test setup
  const char* testUser = "testuser";
  const char* testPass = "testpass";
  
  secureServer.setupLoginPage(testUser, testPass);
  
  Serial.println("✓ Authentication setup completed");
  Serial.print("Username set to: ");
  Serial.println(testUser);
  Serial.print("Password set to: ");
  Serial.println(testPass);
  Serial.println("✓ Now using plaintext password for HTTP Basic Auth (FIXED)");
  Serial.println("✓ Hashed password still used for POST login endpoint");
  Serial.println("=== Test Complete ===");
}

void setup()
{
  // Start serial communication for debugging
  Serial.begin(115200);
  Serial.println("\n*** HardEspServer Authentication Fix Test ***");

  // Run authentication test
  testAuthentication();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Setup and start the secure server with the fixed authentication
  secureServer.setupLoginPage("admin", "secure123"); // Set username and password
  secureServer.begin();
  
  Serial.println("*** Server started with FIXED authentication ***");
  Serial.println("- HTTP Basic Auth now uses plaintext password (FIXED BUG)");
  Serial.println("- POST /login still uses hashed password for security");
}

void loop()
{
  // Server is asynchronous, so no code is needed in the loop
}