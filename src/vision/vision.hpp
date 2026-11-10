#pragma once

#include "esp_camera.h"

struct BallPosition {
    float x;
    float y;
    bool detected;
};

BallPosition process_picture(const camera_fb_t &picture);


struct BallTracker {
    float prevX = 0;
    float prevY = 0;

    float vx = 0;
    float vy = 0;

    uint64_t prevTimeUs = 0;
    bool initialized = false;
};


struct BallVelocity {
    float x;
    float y;
};

BallVelocity update_velocity(
    BallTracker &state,
    float x,
    float y,
    uint64_t nowUs
    );
