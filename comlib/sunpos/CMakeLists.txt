



#set(Eigen3_DIR "/share/a53/buildroot-2020.02.3/output/build/eigen-3.3.7/cmake")
#find_package(Eigen3)
#target_link_libraries(sunpos PUBLIC Eigen3)


set_source_files_properties(getIdealSunPoint.cpp PROPERTIES COMPILE_FLAGS "-O3 ")
set_source_files_properties(SunPos.cpp PROPERTIES COMPILE_FLAGS "-O3 ")

add_library(sunpos OBJECT getIdealSunPoint.cpp SunPos.cpp)

target_include_directories(sunpos PUBLIC "/share/a53/buildroot-2020.02.3/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include/eigen3")

