


add_executable(mdc.exe mdc.cpp 
JDrec.cpp uart.cpp timer_reflush.cpp master_frame.cpp
mdc_txt.cpp

)


include_directories("./")



add_subdirectory(mastersvc)
target_link_libraries(mdc.exe PRIVATE mastersvc)
target_link_libraries(mdc.exe PRIVATE comlib)

INSTALL(TARGETS mdc.exe
       RUNTIME DESTINATION bin
)

target_link_libraries(mdc.exe PRIVATE -lpthread)

