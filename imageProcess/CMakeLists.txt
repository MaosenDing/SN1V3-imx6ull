

#add_compile_options(-Ofast)
add_library(rgbxx_yuvxx STATIC rgbxx_yuvxx.c)
target_compile_options(rgbxx_yuvxx PRIVATE -Ofast)

add_library(imageProcess STATIC  jpegProcess rgbxx_yuvxx process)


