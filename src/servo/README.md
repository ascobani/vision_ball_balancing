# Servo control

This directory is reserved for servo PWM initialization and output updates.
It has no implementation yet and does not drive any pins.

Put source files and headers together here. Add each new source file to `SRCS`
in `src/CMakeLists.txt`, for example `"servo/servo.cpp"`. This directory is
already listed in `INCLUDE_DIRS`, so code can include its headers without
relative paths. Add any required ESP-IDF driver to `PRIV_REQUIRES` in the same
CMake file.

Use the existing pin assignments, frequency, and pulse limits from
`src/common/config.h`. LEDC timer 0 and channel 0 are reserved
for the camera clock, so servo PWM must use a different timer and channels.

Keep image analysis in `image_processing/`. The application coordinates
detection, control decisions, and calls to the servo module.
