//
// Created by Ali Sinan COBANİ on 15.10.2026.
//

#include "config.h"
#include "vision.hpp"

BallPosition process_picture(const camera_fb_t &picture) {
    BallPosition pos{};


    uint64_t sum_x = 0;
    uint64_t sum_y = 0;
    uint32_t count = 0;

    for (int y = 0; y < picture.height; y++) {
        for (int x = 0; x < picture.width; x++) {
            uint8_t pixel = picture.buf[y * picture.width + x];
            // 255 beyaz burda beyaz olmayan bir yer aradigimiz icin suan belirledigim 100 degeri aslinda bir esik degeri o nokta icin
            // kalibrasyon icin degistiilevliiir
            //TODO(threshold):Esik degerini degistir
            if (pixel < 25) {
                sum_x += x;
                sum_y += y;
                count++;
            }
        }
    }

    if (count > 0) {
        pos.x = sum_x / count;
        pos.y = sum_y / count;
        pos.detected = true;
        return pos;
    }

    pos.x = 0;
    pos.y = 0;
    pos.detected = false;

    return pos;
}


BallVelocity update_velocity(BallTracker &state, float x, float y, uint64_t now) {
    if (!state.initialized) {
        state.prevTime = 0;
        state.prevX = state.prevY = 0;
        state.vx = state.vy = 0;
        state.initialized = true;
    }
    float dt = (now - state.prevTime) * 1e-6f;

    if (dt <= 0) {
        state.prevX = x;
        state.prevY = y;
        state.vx = state.vy = 0;
        state.prevTime = now;
        return BallVelocity{0, 0};
    }

    float rawVx = (x - state.prevX) / dt;
    float rawVy = (y - state.prevY) / dt;

    constexpr float filterTau =
            CAM_CAPTURE_INTERVAL_MS * 1e-3f;

    float alpha = dt / (filterTau + dt);

    state.vx += alpha * (rawVx - state.vx);
    state.vy += alpha * (rawVy - state.vy);

    state.prevX = x;
    state.prevY = y;
    state.prevTime = now;

    return BallVelocity{state.vx, state.vy};
}
