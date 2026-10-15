# Image processing

This directory is reserved for image analysis and ball detection. It has no
implementation yet.

Put source files and headers together here. Add each new source file to `SRCS`
in `src/CMakeLists.txt`, for example `"image_processing/image_processing.cpp"`.
This directory is already listed in `INCLUDE_DIRS`, so code can include its
headers without relative paths.

Keep camera initialization, frame acquisition, and frame return in the camera
module. Image processing should read a borrowed image buffer while it is
valid and return detection results. Coordinate the calls from `src/main.cpp`;
return the camera frame after processing finishes. `capture_picture()` now
returns a borrowed `camera_fb_t *`; read `picture->buf` and use `picture->len`,
`picture->width`, and `picture->height` for the image bounds. The configured
format is RGB565 (two bytes per pixel). The application calls
`release_picture(picture)` exactly once when processing is done. Never store the
buffer pointer for later use after that release.
