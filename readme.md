# Secure ESP8266 Web Server Guide

This guide details how to implement a hardened web server on the ESP8266 using the `HardServer` class. It includes instructions on setting up the server, improving security, and understanding the limitations of securing an ESP8266-based server.

## Table of Contents

- [Introduction](#introduction)
- [Prerequisites](#prerequisites)
- [Getting Started](#getting-started)
- [Security Features](#security-features)
- [Using the HardServer Class](#using-the-hardserver-class)
- [Best Practices for ESP8266 Security](#best-practices-for-esp8266-security)
- [Limitations and Security Considerations](#limitations-and-security-considerations)
- [Conclusion](#conclusion)

## Introduction

The ESP8266 is a popular choice for IoT projects due to its low cost and built-in Wi-Fi capabilities. However, being a low-power microcontroller, it has inherent security limitations. This guide demonstrates how to implement a simple, secure web server with a login page on the ESP8266 and provides best practices for hardening the server against potential attacks.

## Prerequisites

- Basic understanding of C++ programming.
- Experience with PlatformIO and Arduino IDE.
- ESP8266-based board (e.g., NodeMCU v2).
- PlatformIO installed with the necessary environment set up as described in the `platformio.ini`.

## Getting Started

### Project Setup

1. **Clone or Download the Project:**

    ```sh
    git clone https://github.com/your-repo/secure-esp8266-server.git
    cd secure-esp8266-server
    ```

2. **Open the Project in PlatformIO:**

    ```sh
    pio init --board nodemcuv2
    ```

3. **Ensure `platformio.ini` is configured correctly**:

    ```ini
    [env:nodemcuv2]
    platform = espressif8266
    board = nodemcuv2
    framework = arduino
    monitor_speed = 115200

    build_flags =
        -DDEBUG_ESP_PORT=Serial
        -DDEBUG_ESP_HTTP_CLIENT
        -DDEBUG_ESP_SSL
        -DHTTP_REUSE
        -DHTTP_SECURE

    lib_deps =
        ESP8266WiFi
        ESPAsyncWebServer
        AsyncTCP
        ArduinoJson
        Hash
    ```

## Security Features

The `HardServer` class includes the following security features:

1. **Password Hashing:**
    - User passwords are hashed using SHA-1 before storage and comparison.
    - This reduces the risk of password leakage if memory is compromised.

2. **Basic Authentication:**
    - Implements HTTP Basic Authentication for accessing the server.
    - Credentials are compared against securely stored and hashed versions.

3. **Server Configuration:**
    - Uses asynchronous handling to manage multiple connections without blocking.
    - Placeholder support for HTTPS (requires additional setup for SSL/TLS certificates).

## Using the HardServer Class

### Setting Up the Server

To use the `HardServer` class:

1. **Include the Class in Your Project:**

    ```cpp
    #include "hardserver.h"
    ```

2. **Instantiate the Server:**

    ```cpp
    HardServer secureServer(80);
    ```

3. **Set Wi-Fi Credentials and Start the Server:**

    ```cpp
    const char* ssid = "Your_SSID";
    const char* password = "Your_WIFI_Password";

    void setup() {
        Serial.begin(115200);
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.println("Connecting to WiFi...");
        }
        Serial.println("Connected to WiFi");

        secureServer.setupLoginPage("admin", "admin_password");
        secureServer.begin();
    }
    ```

4. **Upload and Monitor:**

    - Upload the code using PlatformIO.
    - Monitor the serial output for debugging and to confirm the server is running.

### Accessing the Server

1. **Open a Web Browser:**
    - Navigate to `http://<your-esp8266-ip>/login`.

2. **Login with the Credentials:**
    - Use the username and password set in `setupLoginPage`.

## Best Practices for ESP8266 Security

While the `HardServer` class provides basic security measures, consider the following best practices to further harden your server:

1. **Use HTTPS:**
    - Implement HTTPS by generating and installing SSL/TLS certificates.
    - Note that ESP8266 has limited processing power, which can affect performance.

2. **Strong Passwords:**
    - Always use strong, complex passwords for the server login.
    - Consider implementing rate limiting or account lockout mechanisms for repeated failed login attempts.

3. **Keep Firmware Updated:**
    - Regularly update the ESP8266 firmware to the latest version to patch vulnerabilities.

4. **Network Security:**
    - Secure your Wi-Fi network with WPA2 encryption.
    - Use a dedicated network for IoT devices to isolate them from critical systems.

5. **Monitor and Log Access:**
    - Implement logging mechanisms to track access attempts and monitor for suspicious activities.

## Limitations and Security Considerations

Despite the security measures implemented, the ESP8266 has inherent limitations:

- **Resource Constraints:**
    - Limited processing power, memory, and storage can hinder the implementation of robust security features like full SSL/TLS encryption.

- **No Hardware Encryption:**
    - The ESP8266 lacks hardware support for advanced encryption algorithms, relying instead on software implementations, which are less efficient.

- **Vulnerability to Physical Attacks:**
    - Being a microcontroller, the ESP8266 is vulnerable to physical attacks if the device is accessible.

- **Susceptibility to IoT-Specific Attacks:**
    - IoT devices like the ESP8266 are often targets for attacks such as DDoS, botnets, and unauthorized access.

## Conclusion

While the `HardServer` class and the practices outlined in this guide help secure your ESP8266 web server, it's essential to understand the limitations of the hardware. For critical applications requiring high security, consider using more powerful and secure platforms like the ESP32 or other dedicated microcontrollers with hardware encryption support.

Always keep security in mind when deploying IoT devices, as they are often the weakest link in a network. With careful planning and implementation, you can significantly reduce the risk of unauthorized access and ensure your IoT projects remain secure.
