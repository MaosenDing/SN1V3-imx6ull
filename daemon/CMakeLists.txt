


add_executable(daemon.exe daemon.cpp )


include_directories("./")


target_link_libraries(daemon.exe PRIVATE comlib)

INSTALL(TARGETS daemon.exe
       RUNTIME DESTINATION bin
)

#target_link_libraries(daemon PRIVATE -lpthread)

