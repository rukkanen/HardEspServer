#include "hardserver.h"

// Constructor: Initializes the web server on the specified port
HardServer::HardServer(uint16_t port) : server(port) {}

// Starts the server with secure configurations
void HardServer::begin()
{
  // Force HTTPS redirection - Placeholder for when HTTPS is implemented
  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
            {
        if (!request->authenticate(username.c_str(), hashedPassword.c_str())) {
            return request->requestAuthentication();
        }
        request->send(200, "text/html", "Welcome to the secure server!"); });

  // Start the server
  server.begin();

  // Print the IP address if available
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("Server IP address: ");
    Serial.println(WiFi.localIP());
  }
}

// Sets up a simple login page with secure password hashing
void HardServer::setupLoginPage(const char *user, const char *pass)
{
  username = String(user);
  hashedPassword = hashPassword(pass);

  // Setup the login route
  server.on("/login", HTTP_POST, [this](AsyncWebServerRequest *request)
            { handleLoginRequest(request); });
}

// Handles the login request securely
void HardServer::handleLoginRequest(AsyncWebServerRequest *request)
{
  if (validateCredentials(request))
  {
    request->send(200, "text/html", "Login successful!");
  }
  else
  {
    request->send(401, "text/html", "Unauthorized: Invalid credentials");
  }
}

// Utility function to hash passwords using SHA-1
String HardServer::hashPassword(const char *password)
{
  return sha1(password);
}

// Validates the username and password against stored credentials
bool HardServer::validateCredentials(AsyncWebServerRequest *request)
{
  if (!request->hasParam("username", true) || !request->hasParam("password", true))
  {
    return false;
  }

  String inputUser = request->getParam("username", true)->value();
  String inputPass = request->getParam("password", true)->value();
  String hashedInputPass = hashPassword(inputPass.c_str());

  return (inputUser == username && hashedInputPass == hashedPassword);
}
