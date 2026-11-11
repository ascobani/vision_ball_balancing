idf_component_register(
        SRCS "main.cpp" "camera/camera.cpp" "vision/vision.cpp" "servo/servo.cpp"
        INCLUDE_DIRS "camera" "common" "vision" "servo"
        PRIV_REQUIRES esp_common esp_psram log freertos esp_system driver espressif__esp32-camera espressif__servo
)