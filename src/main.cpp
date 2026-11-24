#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "camera.h"
#include "config.h"
#include "logger.h"
#include "servo.h"
#include "vision/vision.hpp"

extern "C" void app_main(void) {
    LOG_INFO("Starting vision ball balancing (ESP-IDF %s)", esp_get_idf_version());

    const esp_err_t cameraError = init_camera();
    if (cameraError != ESP_OK) {
        LOG_ERROR("Camera initialization failed: %s (0x%x)",
                  esp_err_to_name(cameraError), static_cast<unsigned int>(cameraError));
        return;
    }
    LOG_INFO("Camera ready; capture interval: %d ms", CAM_CAPTURE_INTERVAL_MS);
    const esp_err_t servoError = servo_init();
    if (servoError != ESP_OK) {
        LOG_ERROR("Servo initialization failed: %s (0x%x)",
                  esp_err_to_name(servoError), static_cast<unsigned int>(servoError));
        return;
    }

    LOG_INFO("Servos initialized and centered at %d deg", SERVO_CENTER_ANGLE);

    BallVelocity velocity{};
    BallTracker state;

    for (;;) {
        camera_fb_t *picture = capture_picture();
        if (picture != nullptr) {
            // görüntünün ortası, top buraya gelsin istiyoruz
            const float center_x = static_cast<float>(picture->width) / 2.0f;
            const float center_y = static_cast<float>(picture->height) / 2.0f;

            BallPosition pos = process_picture(*picture);

            LOG_INFO(
                "Ball position: x=%d, y=%d, detected=%d",
                pos.x,
                pos.y,
                static_cast<int>(pos.detected)
            );

            release_picture(picture);

            velocity = update_velocity(state, pos.x, pos.y, esp_timer_get_time());
            LOG_INFO("Velocity: vX:%.2f vY:%.2f", velocity.x, velocity.y);

            if (pos.detected) {
                // topun ortadan ne kadar uzak (-1 ile 1 arası)
                const float error_x = (center_x - static_cast<float>(pos.x)) / center_x;
                const float error_y = (center_y - static_cast<float>(pos.y)) / center_y;

                // P konuma, D hıza bakıyor. 0.05 hız katsayısı, deneyerek ayar çektik
                const float tilt_x_deg = (PID_KP * error_x * SERVO_MAX_TILT_DEG) - (PID_KD * velocity.x * 0.05f);
                const float tilt_y_deg = (PID_KP * error_y * SERVO_MAX_TILT_DEG) - (PID_KD * velocity.y * 0.05f);

                // eğimi servolara göndermek için
                servo_set_platform_tilt(tilt_x_deg, tilt_y_deg, 0.0f);
            } else {
                // top görünmüyorsa platform düz kalacak
                servo_center_all();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(CAM_CAPTURE_INTERVAL_MS));
    }
}