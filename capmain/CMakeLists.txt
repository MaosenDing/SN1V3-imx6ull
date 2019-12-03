


add_executable(main aim.cpp)

#sensor
add_subdirectory(sensor)
target_link_libraries(main sensor)


target_link_directories(main PUBLIC ${PROJECT_SOURCE_DIR}/comlib)
target_link_libraries(main comlib)
#install main
INSTALL(TARGETS main
       RUNTIME DESTINATION bin
)
