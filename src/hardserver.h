#ifndef HARDSERVER_H
#define HARDSERVER_H

#include <ESPAsyncWebServer.h> // Asynchronous Web Server library
#include <ArduinoJson.h>       // Library for JSON handling
#include <Hash.h>              // Library for SHA-1 hashing

/**
 * HardServer - A secure web server implementation for ESP8266
 * 
 * This class implements a dual-authentication web server that supports:
 * 1. HTTP Basic Authentication for simple browser access
 * 2. POST-based login with password hashing for enhanced security
 * 
 * Security Model:
 * - Stores passwords in two formats to support different auth methods
 * - Uses SHA-1 hashing for POST login to protect against memory dumps
 * - Uses plaintext for HTTP Basic Auth (required by the protocol)
 * - Designed for ESP8266 resource constraints
 */
class HardServer
{
public:
  /**
   * Constructor: Initializes the web server on the specified port
   * @param port The TCP port number for the web server (typically 80 for HTTP)
   */
  HardServer(uint16_t port);

  /**
   * Starts the server with secure configurations
   * Sets up routing, authentication, and begins listening for connections
   */
  void begin();

  /**
   * Sets up authentication credentials and login endpoints
   * @param username The username for authentication
   * @param password The password (stored in both plaintext and hashed forms)
   */
  void setupLoginPage(const char *username, const char *password);

private:
  AsyncWebServer server; // The asynchronous web server object
  
  // Dual password storage for different authentication methods:
  String hashedPassword;    // SHA-1 hashed password for secure POST login validation
  String plaintextPassword; // Plaintext password required for HTTP Basic Auth protocol
  String username;          // Username stored in plaintext (not sensitive for hashing)

  /**
   * Handles POST /login requests with credential validation
   * Uses hashed password comparison for enhanced security
   */
  void handleLoginRequest(AsyncWebServerRequest *request);

  /**
   * Utility function to hash passwords using SHA-1
   * @param password The plaintext password to hash
   * @return SHA-1 hash as hexadecimal string
   */
  String hashPassword(const char *password);

  /**
   * Validates username and password from POST request parameters
   * Compares against hashed password for security
   * @param request The HTTP request containing form data
   * @return true if credentials are valid, false otherwise
   */
  bool validateCredentials(AsyncWebServerRequest *request);
};

#endif // HARDSERVER_H
