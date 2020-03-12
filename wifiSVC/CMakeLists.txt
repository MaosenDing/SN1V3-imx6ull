


add_executable(wifictrl.exe wifictrl.cpp 
)


include_directories("./")



#add_subdirectory(mastersvc)
#target_link_libraries(wifictrl.exe PRIVATE mastersvc)
target_link_libraries(wifictrl.exe PRIVATE comlib)

INSTALL(TARGETS wifictrl.exe
       RUNTIME DESTINATION bin
)

target_link_libraries(wifictrl.exe PRIVATE -lpthread)

