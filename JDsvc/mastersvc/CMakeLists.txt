
#include_directories("${PROJECT_SOURCE_DIR}/errHandle")
#link_directories("${PROJECT_SOURCE_DIR}/errHandle")
#link_directories("${PROJECT_SOURCE_DIR}/comlib")



add_library(mastersvc STATIC jdsvc_time.cpp)


