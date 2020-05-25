


add_executable(wifictrl.exe wifictrl.cpp wifi_frame.cpp wifi_rec.cpp wifi_snd.cpp wifi_svc_init.cpp wifi_module_ctrl.cpp
)


include_directories("./")



add_subdirectory(svc)
#target_link_libraries(wifictrl.exe PRIVATE mastersvc)

target_link_libraries(wifictrl.exe PRIVATE comlib)
target_link_libraries(wifictrl.exe PRIVATE wifisvc)

INSTALL(TARGETS wifictrl.exe
       RUNTIME DESTINATION bin
)

target_link_libraries(wifictrl.exe PRIVATE -lpthread)

