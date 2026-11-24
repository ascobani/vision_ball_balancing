#ifndef VISION_BALL_BALANCING_SERVO_H
#define VISION_BALL_BALANCING_SERVO_H

#include <stdint.h>
#include "esp_err.h"
#include "config.h"

esp_err_t servo_init(void);

// açı 0 - 180 derece
esp_err_t servo_set_angle(uint8_t servo_index, float angle_deg);
esp_err_t servo_set_all_angles(float angle1_deg, float angle2_deg, float angle3_deg);

// pulse 500 - 2500 us
esp_err_t servo_set_pulse_us(uint8_t servo_index, uint32_t pulse_us);
esp_err_t servo_center_all(void);
float servo_height_to_angle(float height_offset_mm);
esp_err_t servo_set_height_offset_mm(uint8_t servo_index, float height_offset_mm);
esp_err_t servo_set_all_heights_mm(float h1_mm, float h2_mm, float h3_mm);
esp_err_t servo_set_platform_tilt(float tilt_x_deg, float tilt_y_deg, float heave_z_mm = 0.0f);
#endif