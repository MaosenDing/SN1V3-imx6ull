
#PROJECT(MDC)
include_directories("${PROJECT_SOURCE_DIR}/errHandle")
include_directories("./")
link_directories("${PROJECT_SOURCE_DIR}/errHandle")
link_directories("${PROJECT_SOURCE_DIR}/comlib")



add_executable(mdc mdc.cpp JDrec.cpp uart.cpp)

INSTALL(TARGETS mdc
       RUNTIME DESTINATION bin
)

target_link_libraries(mdc errHandle)
target_link_libraries(mdc comlib)