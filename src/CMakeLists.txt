idf_component_register(
    SRCS "main.cpp" "camera/camera.cpp"
    INCLUDE_DIRS "camera" "common" "image_processing" "servo"
    PRIV_REQUIRES esp_common esp_psram log freertos esp_system espressif__esp32-camera
)
