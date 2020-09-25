

set(main aim.exe)
add_executable(${main} aim.cpp processtest.cpp threadtest.cpp tmp_ctrl_method.cpp)

target_link_libraries(${main} PRIVATE boost_thread)
#sensor
add_subdirectory(sensor)
target_link_libraries(${main} PRIVATE sensor)

add_subdirectory(imageProcess)
target_link_libraries(${main} PRIVATE imageProcess)

target_link_directories(${main} PUBLIC ${PROJECT_SOURCE_DIR}/comlib)
target_link_libraries(${main} PRIVATE comlib)
#install main

INSTALL(TARGETS ${main}
       RUNTIME DESTINATION bin
)
