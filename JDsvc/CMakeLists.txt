


add_executable(mdc mdc.cpp 
JDrec.cpp uart.cpp timer_reflush.cpp master_frame.cpp)


include_directories("./")



add_subdirectory(mastersvc)
target_link_libraries(mdc PRIVATE mastersvc)
target_link_libraries(mdc PRIVATE comlib)

INSTALL(TARGETS mdc
       RUNTIME DESTINATION bin
)

target_link_libraries(mdc PRIVATE -lpthread)

