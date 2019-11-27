




IF(ENABLE_ERRHANDLE_COM)
add_library(errHandle SHARED errHandle.cpp)
INSTALL(TARGETS errHandle LIBRARY DESTINATION lib)
else()
add_library(errHandle STATIC errHandle.cpp)
endif()


include_directories("glog")
link_directories("glog")
target_link_libraries(errHandle -lglog)
