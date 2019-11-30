


add_library(rgbxx_yuvxx2 STATIC rgbxx_yuvxx.c)
target_compile_options(rgbxx_yuvxx2 PRIVATE -Ofast)

add_library(imageProcess STATIC  jpegProcess process)
target_link_libraries(imageProcess rgbxx_yuvxx2)



target_include_directories(imageProcess PUBLIC "jpeg")
target_link_directories(imageProcess PUBLIC "jpeg")
target_link_libraries(imageProcess -ljpeg)

