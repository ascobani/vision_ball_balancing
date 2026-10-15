idf_component_register(
    SRCS "main.cpp" "camera/camera.cpp" "vision/vision.cpp"
    INCLUDE_DIRS "camera" "common" "vision" "servo"
    PRIV_REQUIRES esp_common esp_psram log freertos esp_system espressif__esp32-camera
)
