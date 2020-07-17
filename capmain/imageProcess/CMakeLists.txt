



set_source_files_properties(rgbxx_yuvxx.c PROPERTIES COMPILE_FLAGS -Ofast)
set_source_files_properties(binaryzation.cpp PROPERTIES COMPILE_FLAGS -O3)
set_source_files_properties(process PROPERTIES COMPILE_FLAGS -O3)


add_library(imageProcess STATIC  
jpegProcess 
process.cpp 
IMAGEDATA.cpp 
imageOperator.cpp 
rgbxx_yuvxx.c 
binaryzation.cpp
)


target_include_directories(imageProcess PUBLIC "jpeg")
target_link_directories(imageProcess PUBLIC "jpeg")
target_link_libraries(imageProcess PUBLIC -ljpeg)

