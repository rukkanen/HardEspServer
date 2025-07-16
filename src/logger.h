#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

enum LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3
};

class Logger {
public:
    static void log(const String& message, LogLevel level = INFO) {
        String levelStr;
        switch(level) {
            case DEBUG: levelStr = "DEBUG"; break;
            case INFO: levelStr = "INFO"; break;
            case WARNING: levelStr = "WARNING"; break;
            case ERROR: levelStr = "ERROR"; break;
        }
        Serial.println("[" + levelStr + "] " + message);
    }
};

#endif // LOGGER_H