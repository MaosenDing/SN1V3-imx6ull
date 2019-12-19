
#include_directories("${PROJECT_SOURCE_DIR}/errHandle")
#link_directories("${PROJECT_SOURCE_DIR}/errHandle")
#link_directories("${PROJECT_SOURCE_DIR}/comlib")



add_library(mastersvc STATIC jdsvc_time.cpp jdsvc_table.cpp jdsvc_manual.cpp jdsvc_correct.cpp jdsvc_par_set.cpp )
target_link_directories(mastersvc PUBLIC "${PROJECT_SOURCE_DIR}/timework")