#include "servo.h"
#include <math.h>
#include "driver/ledc.h"
#include "esp_log.h"

static const char *TAG = "servo";

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// Eğer config.h içine henüz eklenmediyse varsayılan mekanik ölçüler:
#ifndef SERVO_ARM_LENGTH_MM
#define SERVO_ARM_LENGTH_MM     20.0f
#endif

#ifndef PLATFORM_RADIUS_MM
#define PLATFORM_RADIUS_MM      65.0f
#endif

#ifndef SERVO_MAX_TILT_DEG
#define SERVO_MAX_TILT_DEG      15.0f
#endif

#ifndef SERVO_MIN_SAFE_ANGLE
#define SERVO_MIN_SAFE_ANGLE    30.0f
#endif

#ifndef SERVO_MAX_SAFE_ANGLE
#define SERVO_MAX_SAFE_ANGLE    150.0f
#endif

// config.h dosyasındaki pin tanımları (D0=1, D1=2, D3=4)
static const int SERVO_PINS[SERVO_COUNT] = {
    SERVO_1_PIN,
    SERVO_2_PIN,
    SERVO_3_PIN
};

// README kuralı: Timer 0 ve Channel 0 kamera saat sinyali (XCLK) için ayrılmıştır!
// Bu yüzden servolar için Timer 1 ve Channel 1, 2, 3 kullanıyoruz.
static const ledc_timer_t SERVO_TIMER = LEDC_TIMER_1;
static const ledc_mode_t SERVO_SPEED_MODE = LEDC_LOW_SPEED_MODE;
static const ledc_timer_bit_t SERVO_DUTY_RES = LEDC_TIMER_14_BIT; // 16384 adım çözünürlük

static const ledc_channel_t SERVO_CHANNELS[SERVO_COUNT] = {
    LEDC_CHANNEL_1,
    LEDC_CHANNEL_2,
    LEDC_CHANNEL_3
};

// 3 servonun platform etrafındaki açıları: 0°, 120°, 240° (Radyan cinsinden)
static const float SERVO_MOUNT_ANGLES_RAD[SERVO_COUNT] = {
    0.0f,
    (120.0f * (float)M_PI) / 180.0f,
    (240.0f * (float)M_PI) / 180.0f
};

// Mikrosaniye değerini LEDC duty (doluluk) değerine çeviren yardımcı fonksiyon
static uint32_t pulse_us_to_duty(uint32_t pulse_us) {
    uint32_t period_us = 1000000UL / SERVO_FREQUENCY_HZ; // 50 Hz -> 20000 us
    uint32_t max_duty = (1UL << SERVO_DUTY_RES);         // 2^14 = 16384
    return (pulse_us * max_duty) / period_us;
}

esp_err_t servo_init(void) {
    ledc_timer_config_t timer_conf = {};
    timer_conf.speed_mode      = SERVO_SPEED_MODE;
    timer_conf.duty_resolution = SERVO_DUTY_RES;
    timer_conf.timer_num       = SERVO_TIMER;
    timer_conf.freq_hz         = SERVO_FREQUENCY_HZ;
    timer_conf.clk_cfg         = LEDC_AUTO_CLK;

    esp_err_t err = ledc_timer_config(&timer_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "LEDC timer kurulumu basarisiz: %s", esp_err_to_name(err));
        return err;
    }

    uint32_t center_duty = pulse_us_to_duty(SERVO_CENTER_PULSE_US);

    for (int i = 0; i < SERVO_COUNT; i++) {
        ledc_channel_config_t ch_conf = {};
        ch_conf.gpio_num   = SERVO_PINS[i];
        ch_conf.speed_mode = SERVO_SPEED_MODE;
        ch_conf.channel    = SERVO_CHANNELS[i];
        ch_conf.intr_type  = LEDC_INTR_DISABLE;
        ch_conf.timer_sel  = SERVO_TIMER;
        ch_conf.duty       = center_duty; // Başlangıçta 90 derecede (1500 us) başlasın
        ch_conf.hpoint     = 0;

        err = ledc_channel_config(&ch_conf);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Servo %d (GPIO %d) kanal kurulumu basarisiz: %s",
                     i, SERVO_PINS[i], esp_err_to_name(err));
            return err;
        }
    }

    ESP_LOGI(TAG, "3 servo basariyla baslatildi (GPIO %d, %d, %d)",
             SERVO_1_PIN, SERVO_2_PIN, SERVO_3_PIN);
    return ESP_OK;
}

esp_err_t servo_set_pulse_us(uint8_t servo_index, uint32_t pulse_us) {
    if (servo_index >= SERVO_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    if (pulse_us < SERVO_MIN_PULSE_US) pulse_us = SERVO_MIN_PULSE_US;
    if (pulse_us > SERVO_MAX_PULSE_US) pulse_us = SERVO_MAX_PULSE_US;

    uint32_t duty = pulse_us_to_duty(pulse_us);

    esp_err_t err = ledc_set_duty(SERVO_SPEED_MODE, SERVO_CHANNELS[servo_index], duty);
    if (err != ESP_OK) return err;

    return ledc_update_duty(SERVO_SPEED_MODE, SERVO_CHANNELS[servo_index]);
}

esp_err_t servo_set_angle(uint8_t servo_index, float angle_deg) {
    if (servo_index >= SERVO_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    if (angle_deg < (float)SERVO_MIN_ANGLE) angle_deg = (float)SERVO_MIN_ANGLE;
    if (angle_deg > (float)SERVO_MAX_ANGLE) angle_deg = (float)SERVO_MAX_ANGLE;

    float ratio = (angle_deg - (float)SERVO_MIN_ANGLE) /
                  ((float)(SERVO_MAX_ANGLE - SERVO_MIN_ANGLE));

    uint32_t pulse_us = SERVO_MIN_PULSE_US +
                        (uint32_t)(ratio * (float)(SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US) + 0.5f);

    return servo_set_pulse_us(servo_index, pulse_us);
}

esp_err_t servo_set_all_angles(float angle1_deg, float angle2_deg, float angle3_deg) {
    esp_err_t err;
    err = servo_set_angle(0, angle1_deg);
    if (err != ESP_OK) return err;

    err = servo_set_angle(1, angle2_deg);
    if (err != ESP_OK) return err;

    return servo_set_angle(2, angle3_deg);
}

esp_err_t servo_center_all(void) {
    return servo_set_all_angles(SERVO_CENTER_ANGLE, SERVO_CENTER_ANGLE, SERVO_CENTER_ANGLE);
}

float servo_height_to_angle(float height_offset_mm) {
    // arcsin tanım kümesi [-1.0, 1.0] dışına taşmaması için oranı sınırla
    float sin_val = height_offset_mm / SERVO_ARM_LENGTH_MM;
    if (sin_val > 0.95f) sin_val = 0.95f;
    if (sin_val < -0.95f) sin_val = -0.95f;

    // 0 mm -> 90 derece (yatay merkez). +mm yukarı, -mm aşağı döndürür.
    float delta_angle_deg = asinf(sin_val) * (180.0f / (float)M_PI);
    float target_angle = (float)SERVO_CENTER_ANGLE + delta_angle_deg;

    // Mekanik kolların sıkışmaması için güvenli açı sınırlarında tut
    if (target_angle < SERVO_MIN_SAFE_ANGLE) target_angle = SERVO_MIN_SAFE_ANGLE;
    if (target_angle > SERVO_MAX_SAFE_ANGLE) target_angle = SERVO_MAX_SAFE_ANGLE;

    return target_angle;
}

esp_err_t servo_set_height_offset_mm(uint8_t servo_index, float height_offset_mm) {
    if (servo_index >= SERVO_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    float angle_deg = servo_height_to_angle(height_offset_mm);
    return servo_set_angle(servo_index, angle_deg);
}

esp_err_t servo_set_all_heights_mm(float h1_mm, float h2_mm, float h3_mm) {
    esp_err_t err;
    err = servo_set_height_offset_mm(0, h1_mm);
    if (err != ESP_OK) return err;

    err = servo_set_height_offset_mm(1, h2_mm);
    if (err != ESP_OK) return err;

    return servo_set_height_offset_mm(2, h3_mm);
}

esp_err_t servo_set_platform_tilt(float tilt_x_deg, float tilt_y_deg, float heave_z_mm) {
    // Platform eğim açılarını güvenli aralıkta sınırla
    if (tilt_x_deg > SERVO_MAX_TILT_DEG) tilt_x_deg = SERVO_MAX_TILT_DEG;
    if (tilt_x_deg < -SERVO_MAX_TILT_DEG) tilt_x_deg = -SERVO_MAX_TILT_DEG;
    if (tilt_y_deg > SERVO_MAX_TILT_DEG) tilt_y_deg = SERVO_MAX_TILT_DEG;
    if (tilt_y_deg < -SERVO_MAX_TILT_DEG) tilt_y_deg = -SERVO_MAX_TILT_DEG;

    float tx_rad = (tilt_x_deg * (float)M_PI) / 180.0f;
    float ty_rad = (tilt_y_deg * (float)M_PI) / 180.0f;

    float heights[SERVO_COUNT];
    for (int i = 0; i < SERVO_COUNT; i++) {
        // i. servonun platform üzerindeki (x_i, y_i) koordinatı
        float x_i = PLATFORM_RADIUS_MM * cosf(SERVO_MOUNT_ANGLES_RAD[i]);
        float y_i = PLATFORM_RADIUS_MM * sinf(SERVO_MOUNT_ANGLES_RAD[i]);

        // İstenen X ve Y eğimi için o köşenin gereken yükseklik değişimi (mm)
        heights[i] = heave_z_mm + (x_i * sinf(tx_rad)) + (y_i * sinf(ty_rad));
    }

    return servo_set_all_heights_mm(heights[0], heights[1], heights[2]);
}