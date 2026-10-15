/*
 * Created by Ali Sinan COBANİ on 15.10.2026.
 */

#ifndef VISION_BALL_BALANCING_CONFIG_H
#define VISION_BALL_BALANCING_CONFIG_H

#ifndef DEBUG_MODE
#define DEBUG_MODE 1
#endif

// GPIO assignments
#define SERVO_1_PIN             1   // D0
#define SERVO_2_PIN             2   // D1
#define SERVO_3_PIN             4   // D3

// Servo settings
#define SERVO_COUNT             3
#define SERVO_FREQUENCY_HZ      50

// Pulse widths (microseconds)
#define SERVO_MIN_PULSE_US      500
#define SERVO_MAX_PULSE_US      2500
#define SERVO_CENTER_PULSE_US   1500

// Servo angles
#define SERVO_MIN_ANGLE         0
#define SERVO_MAX_ANGLE         180
#define SERVO_CENTER_ANGLE      90

// Camera control
#define CAM_PWDN_PIN            -1
#define CAM_RESET_PIN           -1

// Camera clock and SCCB
#define CAM_XCLK_PIN            10
#define CAM_SIOD_PIN            40
#define CAM_SIOC_PIN            39

// Camera data pins
#define CAM_Y2_PIN              15
#define CAM_Y3_PIN              17
#define CAM_Y4_PIN              18
#define CAM_Y5_PIN              16
#define CAM_Y6_PIN              14
#define CAM_Y7_PIN              12
#define CAM_Y8_PIN              11
#define CAM_Y9_PIN              48

// Camera synchronization
#define CAM_VSYNC_PIN           38
#define CAM_HREF_PIN            47
#define CAM_PCLK_PIN            13

// Camera clock
#define CAM_XCLK_FREQ_HZ        20000000

#define CONTROL_FREQUENCY_HZ    100
#define CONTROL_INTERVAL_MS     (1000 / CONTROL_FREQUENCY_HZ)

// Initial values - must be tuned
#define PID_KP                  1.0f
#define PID_KI                  0.0f
#define PID_KD                  0.0f

#endif // VISION_BALL_BALANCING_CONFIG_H