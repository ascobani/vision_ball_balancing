#include "servo.h"
#include "driver/ledc.h"
#include "esp_log.h"

static const char *TAG = "servo";

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