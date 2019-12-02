





add_library(errHandle STATIC errHandle.cpp)


target_include_directories(errHandle PUBLIC "glog")
target_link_directories(errHandle PUBLIC "glog")
target_link_libraries(errHandle -lglog)
