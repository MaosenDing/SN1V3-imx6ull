


add_executable(main aim.cpp)

#sensor
add_subdirectory(sensor)
target_link_libraries(main sensor)

#install main
INSTALL(TARGETS main
       RUNTIME DESTINATION bin
)
