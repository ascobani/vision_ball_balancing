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

    if (count>0) {
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
