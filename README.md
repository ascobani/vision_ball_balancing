# vision_ball_balancing

PlatformIO / Arduino starter for the **Seeed Studio XIAO ESP32S3 Sense**.
Uses board ID `seeed_xiao_esp32s3`, 8 MB flash, OPI PSRAM, and USB CDC serial.
The pioarduino platform is pinned to 55.03.38-1 (Arduino ESP32 3.3.8).

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

## Starter behavior

`src/main.cpp` blinks the onboard LED and prints a serial heartbeat once per
second. It also reports detected flash and PSRAM sizes at startup. This is a
project scaffold; camera capture, ball detection, and servo control are not yet
implemented. No firmware is uploaded by project creation or a normal build.

## Layout and Git

- `src/`: application code
- `include/`: project headers
- `lib/`: project-local libraries
- `test/`: future PlatformIO tests

Git starts on branch `main`. `.gitignore` excludes PlatformIO output, local IDE
state, generated CMake files, macOS metadata, logs, and local secrets.

## References

- [PlatformIO board definition](https://docs.platformio.org/en/latest/boards/espressif32/seeed_xiao_esp32s3.html)
- [Seeed getting started](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
- [Seeed Sense camera guide](https://wiki.seeedstudio.com/xiao_esp32s3_camera_usage/)
