#include "camera.h"

#include "esp_psram.h"

#include "config.h"
#include "logger.h"

esp_err_t init_camera(void) {
    if (!esp_psram_is_initialized()) {
        LOG_ERROR("PSRAM is required for the camera frame buffer; check the memory settings");
        return ESP_ERR_NO_MEM;
    }

    LOG_INFO("PSRAM: %zu MB", esp_psram_get_size() / (1024 * 1024));
    LOG_INFO("Initializing camera: RGB565, QVGA (320 x 240)");

    // Zero-initialize first, then use the XIAO ESP32S3 Sense pins from config.h.
    camera_config_t cameraConfig = {};
    cameraConfig.pin_pwdn = CAM_PWDN_PIN;
    cameraConfig.pin_reset = CAM_RESET_PIN;
    cameraConfig.pin_xclk = CAM_XCLK_PIN;
    cameraConfig.pin_sccb_sda = CAM_SIOD_PIN;
    cameraConfig.pin_sccb_scl = CAM_SIOC_PIN;

    // Camera Y2..Y9 correspond to the driver's data inputs D0..D7.
    cameraConfig.pin_d0 = CAM_Y2_PIN;
    cameraConfig.pin_d1 = CAM_Y3_PIN;
    cameraConfig.pin_d2 = CAM_Y4_PIN;
    cameraConfig.pin_d3 = CAM_Y5_PIN;
    cameraConfig.pin_d4 = CAM_Y6_PIN;
    cameraConfig.pin_d5 = CAM_Y7_PIN;
    cameraConfig.pin_d6 = CAM_Y8_PIN;
    cameraConfig.pin_d7 = CAM_Y9_PIN;
    cameraConfig.pin_vsync = CAM_VSYNC_PIN;
    cameraConfig.pin_href = CAM_HREF_PIN;
    cameraConfig.pin_pclk = CAM_PCLK_PIN;

    // Reserve LEDC timer/channel 0 for the camera clock, not future servo PWM.
    cameraConfig.xclk_freq_hz = CAM_XCLK_FREQ_HZ;
    cameraConfig.ledc_timer = LEDC_TIMER_0;
    cameraConfig.ledc_channel = LEDC_CHANNEL_0;
    cameraConfig.pixel_format = PIXFORMAT_RGB565;
    cameraConfig.frame_size = FRAMESIZE_QVGA;
    cameraConfig.jpeg_quality = 12; // Only used if pixel_format is JPEG.
    cameraConfig.fb_count = 1;
    cameraConfig.fb_location = CAMERA_FB_IN_PSRAM;
    cameraConfig.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

    return esp_camera_init(&cameraConfig);
}

camera_fb_t *capture_picture(void) {
    LOG_INFO("Taking picture...");
    camera_fb_t *picture = esp_camera_fb_get();
    if (picture == nullptr) {
        LOG_ERROR("Camera capture failed");
        return nullptr;
    }

    LOG_INFO("Picture taken: %zu bytes, %zu x %zu", picture->len,
             picture->width, picture->height);
    LOG_DEBUG("Pixel format: %d", static_cast<int>(picture->format));

    return picture;
}

void release_picture(camera_fb_t *picture) {
    if (picture != nullptr) {
        esp_camera_fb_return(picture);
    }
}
