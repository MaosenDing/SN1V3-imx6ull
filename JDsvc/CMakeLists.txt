


add_executable(mdc.exe mdc.cpp 
timer_reflush.cpp master_frame.cpp
mdc_txt.cpp
autorecoder.cpp
led_ctrl.cpp
JD_file_cre.cpp
JD_file_service.cpp
JD_file_version.cpp
JD_TransMit_img.cpp
)


include_directories("./")



add_subdirectory(mastersvc)
target_link_libraries(mdc.exe PRIVATE mastersvc)
target_link_libraries(mdc.exe PRIVATE comlib)

INSTALL(TARGETS mdc.exe
       RUNTIME DESTINATION bin
)

target_link_libraries(mdc.exe PRIVATE -lpthread)

