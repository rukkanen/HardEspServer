#ifndef HARDSERVER_H
#define HARDSERVER_H

#include <ESPAsyncWebServer.h> // Asynchronous Web Server library
#include <ArduinoJson.h>       // Library for JSON handling
#include <Hash.h>              // Library for SHA-1 hashing

class HardServer
{
public:
  // Constructor: Initializes the web server on the specified port
  HardServer(uint16_t port);

  // Starts the server with secure configurations
  void begin();

  // Sets up a simple login page
  void setupLoginPage(const char *username, const char *password);

private:
  AsyncWebServer server; // The asynchronous web server object
  String hashedPassword; // Stores the hashed version of the password for comparison
  String username;       // Stores the plaintext username

  // Handles the login request securely
  void handleLoginRequest(AsyncWebServerRequest *request);

  // Utility function to hash passwords using SHA-1
  String hashPassword(const char *password);

  // Validates the username and password
  bool validateCredentials(AsyncWebServerRequest *request);
};

#endif // HARDSERVER_H
