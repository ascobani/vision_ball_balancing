#ifndef VISION_BALL_BALANCING_SERVO_H
#define VISION_BALL_BALANCING_SERVO_H

#include <stdint.h>
#include "esp_err.h"
#include "config.h"

// 3 servoyu LEDC Timer 1 ve Channel 1, 2, 3 üzerinde başlatır ve merkeze (90 dereceye) alır
esp_err_t servo_init(void);

// Tek bir servonun (0, 1 veya 2) açısını derece cinsinden (0.0 - 180.0) ayarlar
esp_err_t servo_set_angle(uint8_t servo_index, float angle_deg);

// 3 servonun açısını aynı anda ayarlar
esp_err_t servo_set_all_angles(float angle1_deg, float angle2_deg, float angle3_deg);

// Tek bir servonun darbe genişliğini doğrudan mikrosaniye (500 - 2500 us) olarak ayarlar
esp_err_t servo_set_pulse_us(uint8_t servo_index, uint32_t pulse_us);

// Tüm servoları merkez pozisyona (90 derece / 1500 us / 0 mm yükseklik) getirir
esp_err_t servo_center_all(void);

// --- YÜKSEKLİK VE TERS KİNEMATİK (INVERSE KINEMATICS) FONKSİYONLARI ---

// İstenen yükseklik değişimini (mm) servo açısına (derece) çevirir (0 mm -> 90 derece)
float servo_height_to_angle(float height_offset_mm);

// Tek bir servonun kol yüksekliğini merkeze göre mm cinsinden (-L ile +L arası) ayarlar
esp_err_t servo_set_height_offset_mm(uint8_t servo_index, float height_offset_mm);

// 3 servonun kol yüksekliğini merkeze göre mm cinsinden aynı anda ayarlar
esp_err_t servo_set_all_heights_mm(float h1_mm, float h2_mm, float h3_mm);

// PID/İvme kontrolcüsünden gelen X ve Y eğim açılarını (derece) ve Z yüksekliğini (mm)
// 3 servonun yüksekliğine ve açısına çevirip uygular (0°, 120°, 240° yerleşim)
esp_err_t servo_set_platform_tilt(float tilt_x_deg, float tilt_y_deg, float heave_z_mm = 0.0f);

#endif // VISION_BALL_BALANCING_SERVO_H