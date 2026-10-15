#pragma once

#include "esp_camera.h"

struct BallPosition {
    int x;
    int y;
    bool detected;
};

BallPosition process_picture(const camera_fb_t& picture);