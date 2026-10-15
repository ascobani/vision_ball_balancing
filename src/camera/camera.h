#ifndef VISION_BALL_BALANCING_CAMERA_H
#define VISION_BALL_BALANCING_CAMERA_H

#include "esp_err.h"
#include "esp_camera.h"

// Initialize the camera once, using the board pins in config.h.
esp_err_t init_camera(void);

// Borrow one driver-owned frame after initialization, or nullptr on failure.
// Release every successful capture exactly once after processing finishes.
[[nodiscard]] camera_fb_t *capture_picture(void);

// Return a borrowed frame. Do not access it or its buffer afterward.
// Passing nullptr is allowed and does nothing.
void release_picture(camera_fb_t *picture);

#endif // VISION_BALL_BALANCING_CAMERA_H
