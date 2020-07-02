
#include_directories("${PROJECT_SOURCE_DIR}/errHandle")
#link_directories("${PROJECT_SOURCE_DIR}/errHandle")
#link_directories("${PROJECT_SOURCE_DIR}/comlib")



add_library(wifisvc STATIC 
svc_sn2.cpp
svc_query_single.cpp
svc_down_load_update_file.cpp
svc_upload_file.cpp
)
target_link_directories(mastersvc PUBLIC "${PROJECT_SOURCE_DIR}/timework")