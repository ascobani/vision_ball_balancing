/*
* Created by Ali Sinan COBANİ on 15.10.2026.
 */

#ifndef VISION_BALL_BALANCING_LOGGER_H
#define VISION_BALL_BALANCING_LOGGER_H

#include <Arduino.h>
#include "config.h"

#if DEBUG_MODE

#define LOG_DEBUG(tag, fmt, ...) \
    log_d("[%s] " fmt, tag, ##__VA_ARGS__)

#define LOG_INFO(tag, fmt, ...) \
    log_i("[%s] " fmt, tag, ##__VA_ARGS__)

#else

#define LOG_DEBUG(tag, fmt, ...) ((void)0)
#define LOG_INFO(tag, fmt, ...)  ((void)0)

#endif

#define LOG_WARN(tag, fmt, ...) \
    log_w("[%s] " fmt, tag, ##__VA_ARGS__)

#define LOG_ERROR(tag, fmt, ...) \
    log_e("[%s] " fmt, tag, ##__VA_ARGS__)

#endif // VISION_BALL_BALANCING_LOGGER_H