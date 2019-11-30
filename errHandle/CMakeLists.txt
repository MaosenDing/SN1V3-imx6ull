




IF(ENABLE_SHARED_ERRHANDLE)
add_library(errHandle SHARED errHandle.cpp)
INSTALL(TARGETS errHandle LIBRARY DESTINATION lib)
else()
add_library(errHandle STATIC errHandle.cpp)
endif()


target_include_directories(errHandle PUBLIC "glog")
target_link_directories(errHandle PUBLIC "glog")
target_link_libraries(errHandle -lglog)
