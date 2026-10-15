# vision_ball_balancing

ESP-IDF starter for the **Seeed Studio XIAO ESP32S3 Sense**, built with
PlatformIO. The application uses C++ and ESP-IDF directly, with no Arduino
framework. The pinned pioarduino platform 55.03.38-1 provides ESP-IDF 5.5.4.
The board uses 8 MB flash, octal PSRAM, and a native USB Serial/JTAG console.

## Open in CLion

Use **File → Open**, select `platformio.ini`, then **Open as Project**.
This bypasses the broken board-list wizard. If needed, set the PlatformIO
Core executable in CLion to `/opt/homebrew/bin/pio` on this machine.

## Build, upload, and monitor

Run from the project directory with PlatformIO Core installed:

```sh
pio run
pio run --target upload
pio device monitor --baud 115200
```

Connect the board using a USB data cable. PlatformIO normally detects the
upload port automatically. Use `pio device list` if multiple boards are connected,
then add `--upload-port /dev/cu.YOUR_PORT` to the upload command.
If upload cannot connect, hold BOOT, tap RESET, release BOOT, and retry.
Exit the serial monitor with Ctrl+C before uploading again.
Open the monitor and press RESET to see startup logs. Building alone does not
upload firmware. The first build downloads ESP-IDF and its required build tools.

## Starter behavior

`src/main.cpp` starts in `extern "C" void app_main(void)`, initializes the camera,
then takes a picture and logs its byte count and dimensions. The task waits five
seconds after each capture attempt. It uses the XIAO ESP32S3 Sense pin map already
defined in `src/common/config.h`.

The capture settings in `src/camera/camera.cpp` match the supplied example: RGB565,
QVGA (320 x 240), a 20 MHz camera clock, and one frame buffer in PSRAM. A raw
RGB565 frame at this resolution normally contains 153,600 bytes. The image is
returned to the driver after logging; it is not saved or streamed. The existing
LED blink example has been replaced by camera capture.

The application stops camera startup with an error log if PSRAM is missing or
camera initialization fails. If an individual capture returns no frame, it logs
the failure and retries after the normal delay. Ball detection and servo control
are not implemented yet.

The camera driver is declared in `src/idf_component.yml` and pinned to 2.1.8.
ESP-IDF's component manager downloads it and loads its Kconfig automatically;
there is no need to copy the driver's Kconfig into this project. PSRAM uses the
ESP32-S3 settings in `sdkconfig.defaults`, including `CONFIG_SPIRAM=y` and octal
mode, rather than the older ESP32-specific setting from the supplied example.

## Reading the embedded example

- `app_main()` runs once in a FreeRTOS task after ESP-IDF initializes the system.
  The `extern "C"` declaration gives this C++ function the symbol name that
  ESP-IDF expects. There are no Arduino `setup()` or `loop()` functions.
- `init_camera()` configures the driver and returns an `esp_err_t` status.
  `ESP_OK` means initialization succeeded. The caller logs other status codes
  using `esp_err_to_name()` and returns from `app_main()`.
- `capture_picture()` borrows a `camera_fb_t` frame from the driver, logs its
  size, and returns its pointer (or `nullptr` if capture failed). In `main.cpp`,
  pass `picture->buf`, `picture->len`, `picture->width`, and `picture->height`
  to image processing before calling `release_picture(picture)` exactly once.
  The driver owns the memory: do not free it or use it after release. If another
  task processes the image, it must finish before release, or use its own copy.
- Pin numbers in `config.h` refer to ESP32 GPIO numbers, rather than the board's
  D0/D1 labels. Camera Y2 through Y9 map to the driver's D0 through D7 inputs.
- `while (true)` keeps the application task running. `vTaskDelay()` suspends this
  task so the scheduler can run other tasks; `pdMS_TO_TICKS()` converts
  milliseconds to FreeRTOS ticks. This is not a busy-wait delay.

For practice, change `CAM_CAPTURE_INTERVAL_MS`, add a log showing a capture
counter, then try moving capture into a task created with `xTaskCreate()`.
Later, use `xTaskDelayUntil()` for a periodic control task and keep camera work
separate. The five-second wait is only for this capture demonstration.

LEDC timer 0 and channel 0 generate the camera clock. When adding servo PWM,
use a different timer and channels so it does not reconfigure the camera clock.

## Logging

Include `logger.h` and call a macro with a message and optional `printf` arguments:

```cpp
LOG_INFO("Camera ready");
LOG_DEBUG("Control frequency: %d Hz", CONTROL_FREQUENCY_HZ);
LOG_WARN("PSRAM not detected");
LOG_ERROR("Camera initialization failed: %d", errorCode);
```

No tag or trailing `\n` is needed. The macros wrap `ESP_LOGD/I/W/E`, using
`__FILE__` as the tag; ESP-IDF adds the level, timestamp, tag, and newline.
The application and camera module contain working examples. ESP-IDF configures
USB console output before entering the application, so no `Serial.begin()` is
required.

`DEBUG_MODE=1` in `platformio.ini` enables this project's debug and info calls.
Set it to `0` to compile those calls out. Warnings and errors are independent
of that switch. All enabled calls are still filtered by ESP-IDF's log level,
which defaults to Debug in this project.

## ESP-IDF configuration

`sdkconfig.defaults` contains the board's flash, PSRAM, console, log level, and
FreeRTOS tick settings. On the first build, PlatformIO generates
`sdkconfig.xiao_esp32s3_sense` from these defaults. The generated file is ignored
by Git. To edit the current configuration interactively, run:

```sh
pio run --target menuconfig
```

Existing generated configuration values take precedence over defaults. To
share a configuration change, update `sdkconfig.defaults` too. To regenerate
from defaults, delete only `sdkconfig.xiao_esp32s3_sense` and rebuild; this
discards any settings that were only saved in the generated file.

## Layout and Git

```text
src/
├── main.cpp                    # Entry point and application flow
├── CMakeLists.txt              # All application sources and dependencies
├── idf_component.yml           # esp32-camera dependency
├── camera/
│   ├── camera.cpp              # Initialization and picture capture
│   └── camera.h                # Public camera functions
├── image_processing/
│   └── README.md               # Image analysis responsibilities
├── servo/
│   └── README.md               # Servo control responsibilities
└── common/
    ├── config.h                # Shared pins, frequencies, and constants
    └── logger.h                # Shared logging macros
```

All application modules live inside `src/`, with source files and headers
together in each directory. `src/main.cpp` coordinates the modules. One
`src/CMakeLists.txt` lists the application sources, header directories, and
ESP-IDF dependencies. The directories are already listed in `INCLUDE_DIRS`,
so code can use `#include "camera.h"`, `#include "config.h"`, and
`#include "logger.h"` without relative paths.

`camera/` is implemented. `image_processing/` and `servo/` contain notes describing
what belongs there; they do not run processing or drive motors. When adding a
source file, list its path relative to `src/` in `SRCS`, for example
`"image_processing/image_processing.cpp"` or `"servo/servo.cpp"`. Add any new
ESP-IDF dependencies to `PRIV_REQUIRES` in the same CMake file. No per-directory
CMake files are needed.

The other root files configure the project: `CMakeLists.txt` starts the ESP-IDF
build, `platformio.ini` selects the board and tools, and `sdkconfig.defaults`
holds shared ESP-IDF settings. Keep `dependencies.lock` in Git for reproducible
component versions. `managed_components/` contains downloaded dependencies and
is ignored by Git.

Git starts on branch `main`. `.gitignore` excludes PlatformIO output, local IDE
state, generated CMake files, macOS metadata, logs, and local secrets.

## References

- [ESP-IDF startup and app_main](https://docs.espressif.com/projects/esp-idf/en/v5.5/esp32s3/api-guides/startup.html)
- [ESP-IDF GPIO API](https://docs.espressif.com/projects/esp-idf/en/v5.5/esp32s3/api-reference/peripherals/gpio.html)
- [ESP-IDF FreeRTOS](https://docs.espressif.com/projects/esp-idf/en/v5.5/esp32s3/api-reference/system/freertos_idf.html)
- [ESP-IDF logging](https://docs.espressif.com/projects/esp-idf/en/v5.5/esp32s3/api-reference/system/log.html)
- [PlatformIO ESP-IDF workflow](https://docs.platformio.org/en/latest/frameworks/espidf.html)
- [Seeed board pinout and active-low LED](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
- [ESP-IDF camera component](https://components.espressif.com/components/espressif/esp32-camera/versions/2.1.8/readme)
