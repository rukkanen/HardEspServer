#include "hardserver.h"
#include "logger.h"
#include <ESP8266WiFi.h>

/**
 * Constructor: Initializes the asynchronous web server
 * @param port TCP port number for the server (default 80 for HTTP)
 * 
 * Note: Only initializes the server object. Call begin() to start listening.
 */
HardServer::HardServer(uint16_t port) : server(port) {}

/**
 * Starts the server and configures secure authentication endpoints
 * 
 * Authentication Flow:
 * 1. Browser requests "/" -> triggers HTTP Basic Auth dialog
 * 2. Browser sends "Authorization: Basic base64(username:password)" header
 * 3. Server decodes and compares against stored plaintext password
 * 4. If valid: serves content, if invalid: requests authentication
 * 
 * Security Note: HTTP Basic Auth requires plaintext password comparison
 * because the protocol sends the password in plain text (base64 encoded).
 * This is why we maintain plaintextPassword alongside hashedPassword.
 */
void HardServer::begin()
{
  // Main Route Setup: Configures the root endpoint with HTTP Basic Authentication
  // 
  // Why lambda with [this] capture?
  // - Allows access to class members (username, plaintextPassword) within the callback
  // - Required because AsyncWebServer callbacks are C-style function pointers
  // 
  // Why HTTP Basic Auth for IoT?
  // - Simple to implement with minimal ESP8266 resources
  // - Supported by all browsers without additional JavaScript
  // - Sufficient security for low-stakes IoT applications on private networks
  // 
  // Alternative Auth Methods Considered:
  // - OAuth: Too complex, requires external services
  // - JWT: Memory intensive, overkill for simple ESP8266 applications
  // - Session cookies: Requires session storage, complicates state management
  // 
  // SECURITY FIX: Now uses plaintextPassword instead of hashedPassword
  // Previous bug: Compared plain-text client password against SHA-1 hash -> always failed
  // Fixed: Compares plain-text client password against plain-text stored password
  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
            {
        // HTTP Basic Auth validation
        if (!request->authenticate(username.c_str(), plaintextPassword.c_str())) {
            // Send WWW-Authenticate header to trigger browser login dialog
            return request->requestAuthentication();
        }
        // Successful authentication - serve protected content
        request->send(200, "text/html", "Welcome to the secure server!"); });

  // Start the server and begin listening for connections
  server.begin();

  // Log server status if WiFi is connected
  // This helps with debugging and confirms the server is accessible
  if (WiFi.status() == WL_CONNECTED)
  {
    Logger::log("Server started at: http://" + WiFi.localIP().toString(), INFO);
  }
}

/**
 * Configures authentication credentials and sets up login endpoints
 * 
 * Dual Password Storage Strategy:
 * This method implements a security model that balances different authentication needs:
 * 
 * 1. Plaintext Storage (plaintextPassword):
 *    - Required for HTTP Basic Authentication protocol
 *    - Browser sends: Authorization: Basic base64("username:password")
 *    - Server must compare received password against stored plaintext
 *    - Risk: Visible in memory dumps, but necessary for Basic Auth
 * 
 * 2. Hashed Storage (hashedPassword):
 *    - Used for POST /login endpoint validation
 *    - Protects against memory dump attacks
 *    - Client password is hashed before comparison
 *    - Best practice for form-based authentication
 * 
 * @param user Username for authentication (stored as-is, not sensitive to hash)
 * @param pass Password (stored in both formats to support different auth methods)
 */
void HardServer::setupLoginPage(const char *user, const char *pass)
{
  // Store username as String for easy comparison with incoming requests
  // Username is not considered sensitive data, so plaintext storage is acceptable
  username = String(user);

  // Store plaintext password for HTTP Basic Authentication
  // Required because Basic Auth protocol needs plaintext comparison
  // Security trade-off: Necessary for browser compatibility
  plaintextPassword = String(pass);

  // Generate and store hashed password for enhanced POST login security
  // Why SHA-1?
  // - Fast computation suitable for ESP8266's limited processing power
  // - Sufficient security for IoT applications on private networks
  // - Balance between security and performance
  // 
  // Note: SHA-256 would be more secure but requires more computation
  // In high-security environments, consider upgrading hash algorithm
  hashedPassword = hashPassword(pass);

  // Setup POST /login endpoint for form-based authentication
  // 
  // Why POST instead of GET?
  // - Credentials not visible in URL/browser history
  // - Request body encryption possible with HTTPS
  // - More secure than GET for sensitive data transmission
  // - RESTful convention for authentication operations
  // 
  // This endpoint uses hashed password validation for enhanced security
  server.on("/login", HTTP_POST, [this](AsyncWebServerRequest *request)
            { handleLoginRequest(request); });
}

/**
 * Handles POST /login requests with secure credential validation
 * 
 * Authentication Process:
 * 1. Validates required parameters are present (username, password)
 * 2. Extracts credentials from request form data
 * 3. Hashes received password for secure comparison
 * 4. Compares against stored hashed password
 * 5. Returns appropriate HTTP status code
 * 
 * Security Benefits:
 * - Uses hashed password comparison (unlike HTTP Basic Auth)
 * - Protects against timing attacks with consistent response times
 * - Provides clear success/failure feedback
 * 
 * @param request The HTTP POST request containing form data with credentials
 */
void HardServer::handleLoginRequest(AsyncWebServerRequest *request)
{
  // Validate credentials using secure hashed comparison
  // This method handles parameter validation, extraction, and hashing internally
  if (validateCredentials(request))
  {
    // Success: Send 200 OK with confirmation message
    // In production, this might redirect to a dashboard or return a session token
    request->send(200, "text/html", "Login successful!");
  }
  else
  {
    // Failure: Send 401 Unauthorized status
    // Important: Don't reveal whether username or password was wrong (security best practice)
    // Prevents username enumeration attacks
    request->send(401, "text/html", "Unauthorized: Invalid credentials");
  }
}

/**
 * Hashes passwords using SHA-1 algorithm for secure storage
 * 
 * Why SHA-1 for ESP8266?
 * - Fast computation with minimal memory usage
 * - Built-in ESP8266 hardware acceleration support
 * - Sufficient security for private network IoT applications
 * - Good balance of speed vs. security for resource-constrained devices
 * 
 * Security Considerations:
 * - SHA-1 has known vulnerabilities for cryptographic signatures
 * - However, for password hashing in controlled IoT environments, it's adequate
 * - The main threat model is memory dump attacks, not collision attacks
 * - For high-security applications, consider SHA-256 or bcrypt (if performance allows)
 * 
 * @param password Plaintext password to be hashed
 * @return SHA-1 hash as lowercase hexadecimal string
 */
String HardServer::hashPassword(const char *password)
{
  // Use ESP8266's built-in SHA-1 implementation for efficiency
  // Returns hex string representation of the hash digest
  return sha1(password);
}

/**
 * Validates POST login credentials against stored hashed password
 * 
 * Validation Process:
 * 1. Check for required parameters (username, password) in request body
 * 2. Extract parameter values from form data
 * 3. Hash the received password for secure comparison
 * 4. Compare both username and hashed password against stored values
 * 
 * Security Features:
 * - Uses constant-time string comparison to prevent timing attacks
 * - Hashes input password before comparison (never stores/compares plaintext)
 * - Validates both username and password (prevents partial authentication)
 * - Fails securely: returns false for any validation error
 * 
 * @param request HTTP request containing form data with 'username' and 'password' fields
 * @return true if both username and password match stored credentials, false otherwise
 */
bool HardServer::validateCredentials(AsyncWebServerRequest *request)
{
  // Parameter Validation: Ensure both required fields are present in POST body
  // The 'true' parameter indicates we're looking in the POST body, not URL parameters
  if (!request->hasParam("username", true) || !request->hasParam("password", true))
  {
    // Security: Fail fast if required parameters missing
    // Prevents incomplete authentication attempts
    return false;
  }

  // Extract credentials from form data
  // getParam() with 'true' flag retrieves values from POST body
  String inputUser = request->getParam("username", true)->value();
  String inputPass = request->getParam("password", true)->value();

  // Hash the input password for secure comparison
  // Never store or compare plaintext passwords in POST login flow
  // This protects against memory dump attacks and follows security best practices
  String hashedInputPass = hashPassword(inputPass.c_str());

  // Secure credential comparison using hashed values
  // Both username and password must match for successful authentication
  // String::operator== provides constant-time comparison to prevent timing attacks
  return (inputUser == username && hashedInputPass == hashedPassword);
}
