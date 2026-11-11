#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

    for (;;) {
        camera_fb_t *picture = capture_picture();
        if (picture != nullptr) {
            BallPosition pos = process_picture(*picture);

            LOG_INFO(
                "Ball position: x=%d, y=%d, detected=%d",
                pos.x,
                pos.y,
                static_cast<int>(pos.detected)
            );


            release_picture(picture);
        }
        vTaskDelay(pdMS_TO_TICKS(CAM_CAPTURE_INTERVAL_MS));
    }
}