#ifndef VISION_BALL_BALANCING_SERVO_H
#define VISION_BALL_BALANCING_SERVO_H

#include <stdint.h>
#include "esp_err.h"
#include "config.h"

// 3 servoyu LEDC Timer 1 ve Channel 1, 2, 3 üzerinde başlatır ve merkeze (90 dereceye) alır
esp_err_t servo_init(void);

// Tek bir servonun (0, 1 veya 2) açısını derece cinsinden (0.0 - 180.0) ayarlar
esp_err_t servo_set_angle(uint8_t servo_index, float angle_deg);

// 3 servonun açısını aynı anda ayarlar (Ters kinematik / PID çıktısı için çok pratiktir)
esp_err_t servo_set_all_angles(float angle1_deg, float angle2_deg, float angle3_deg);

// Tek bir servonun darbe genişliğini doğrudan mikrosaniye (500 - 2500 us) olarak ayarlar
esp_err_t servo_set_pulse_us(uint8_t servo_index, uint32_t pulse_us);

// Tüm servoları merkez pozisyona (90 derece / 1500 us) getirir
esp_err_t servo_center_all(void);

#endif // VISION_BALL_BALANCING_SERVO_H