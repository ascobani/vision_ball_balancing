#include <Arduino.h>

namespace {
constexpr unsigned long kHeartbeatIntervalMs = 1000;
unsigned long lastHeartbeatMs = 0;
bool ledOn = false;
}  // namespace

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);  // The onboard LED is active-low.

    Serial.begin(115200);
    const unsigned long serialWaitStarted = millis();
    while (!Serial && millis() - serialWaitStarted < 2000) {
        delay(10);
    }

    Serial.println("vision_ball_balancing: Seeed Studio XIAO ESP32S3 Sense");
    Serial.printf("Flash: %lu bytes | PSRAM: %lu bytes\n",
                  static_cast<unsigned long>(ESP.getFlashChipSize()),
                  static_cast<unsigned long>(ESP.getPsramSize()));
}

void loop() {
    const unsigned long now = millis();
    if (now - lastHeartbeatMs >= kHeartbeatIntervalMs) {
        lastHeartbeatMs = now;
        ledOn = !ledOn;
        digitalWrite(LED_BUILTIN, ledOn ? LOW : HIGH);
        Serial.printf("Ready | uptime: %lu ms\n", now);
    }
    delay(1);
}
