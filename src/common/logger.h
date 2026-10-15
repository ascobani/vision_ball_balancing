/*
* Created by Ali Sinan COBANİ on 15.10.2026.
 */

#ifndef VISION_BALL_BALANCING_LOGGER_H
#define VISION_BALL_BALANCING_LOGGER_H

#include "esp_log.h"
#include "config.h"

// printf-style calls; ESP-IDF adds the level, timestamp, file tag, and newline.
#if DEBUG_MODE

#define LOG_DEBUG(...) ESP_LOGD(__FILE__, __VA_ARGS__)
#define LOG_INFO(...)  ESP_LOGI(__FILE__, __VA_ARGS__)

#else

#define LOG_DEBUG(...) ((void)0)
#define LOG_INFO(...)  ((void)0)

#endif

#define LOG_WARN(...)  ESP_LOGW(__FILE__, __VA_ARGS__)
#define LOG_ERROR(...) ESP_LOGE(__FILE__, __VA_ARGS__)

#endif // VISION_BALL_BALANCING_LOGGER_H
