#include "hardserver.h"
#include "logger.h"

HardServer::HardServer(uint16_t port) : server(port) {}

// Starts the server with secure configurations
void HardServer::begin()
{
  // Route Setup: This sets up the main route ("/") of the web server to handle HTTP GET requests.
  // Why? The lambda function is used here to capture the 'this' pointer, allowing access to class members (like username and hashedPassword).
  // Decision: Basic HTTP authentication is used here because it is simple to implement and sufficient for low-stakes IoT applications.
  // Alternatives like OAuth or JWT were not chosen due to their complexity and the limited processing power of the ESP8266.
  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
            {
        if (!request->authenticate(username.c_str(), hashedPassword.c_str())) {
            return request->requestAuthentication();
        }
        request->send(200, "text/html", "Welcome to the secure server!"); });

  server.begin();

  if (WiFi.status() == WL_CONNECTED)
  {
    Logger::log("Server started at: http://" + WiFi.localIP().toString(), INFO);
  }
}

// Sets up a simple login page with secure password hashing
void HardServer::setupLoginPage(const char *user, const char *pass)
{
  // Why? Storing the username as a String object makes it easy to compare with incoming HTTP requests.
  username = String(user);

  // Why? Passwords are hashed before being stored to enhance security. If someone gains access to the memory, they won't see plain-text passwords.
  // Decision: SHA-1 was chosen because it's a straightforward hashing algorithm with sufficient speed and security for this context.
  hashedPassword = hashPassword(pass);

  // Setup the login route
  // Why? This route listens for POST requests at the "/login" endpoint.
  // POST is used instead of GET because it allows for more secure transmission of credentials (e.g., not showing them in the URL).
  server.on("/login", HTTP_POST, [this](AsyncWebServerRequest *request)
            { handleLoginRequest(request); });
}

// Handles the login request securely
void HardServer::handleLoginRequest(AsyncWebServerRequest *request)
{
  // Why? If credentials are valid, the server responds with a success message. If invalid, it sends a 401 Unauthorized status.
  // Decision: This straightforward handling is typical for simple web servers. More complex implementations could involve session tokens or multi-factor authentication,
  // but these were not implemented to keep the design lightweight and suitable for ESP8266 capabilities.
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
  // Why? Hashing the password ensures that even if the data is intercepted or the server is compromised, the attacker won't easily retrieve the original password.
  // Decision: SHA-1 is used because it's a well-known, easy-to-implement hashing function with a good balance of speed and security for this purpose.
  // More secure options like SHA-256 could be used, but they require more computational power, which might not be ideal for ESP8266's limited resources.
  return sha1(password);
}

// Validates the username and password against stored credentials
bool HardServer::validateCredentials(AsyncWebServerRequest *request)
{
  // Why? This checks whether the required parameters (username and password) are present in the request.
  // Decision: If either is missing, the request is considered invalid, returning false.
  if (!request->hasParam("username", true) || !request->hasParam("password", true))
  {
    return false;
  }

  // Why? This retrieves the values of the username and password parameters from the request.
  // String comparison is used because it's a straightforward and effective way to validate credentials in this context.
  String inputUser = request->getParam("username", true)->value();
  String inputPass = request->getParam("password", true)->value();

  // Why? The input password is hashed before comparison to ensure that only hashed values are compared, enhancing security.
  String hashedInputPass = hashPassword(inputPass.c_str());

  // Why? This final comparison determines if the provided credentials match the stored ones. If both match, access is granted.
  return (inputUser == username && hashedInputPass == hashedPassword);
}
