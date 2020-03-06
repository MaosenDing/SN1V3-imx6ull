

#add_compile_options(-Ofast)

set(fil jd_share.cpp mem_share.cpp versions.cpp)

IF(ENABLE_SHARED_COM)
add_library(comlib SHARED ${fil})
INSTALL(TARGETS comlib LIBRARY DESTINATION lib)
else()
add_library(comlib STATIC ${fil})
endif()

#errHandle
add_subdirectory(errHandle)
#datapro
add_subdirectory(datapro)
#timework
add_subdirectory(timework)
#text
add_subdirectory(text)
#CRE
add_subdirectory(CRE)
#CRE
add_subdirectory(JD)


target_link_libraries(comlib
PRIVATE
"-Wl,--whole-archive" 
text
CRE
timework
datapro
errHandle
JD
"-Wl,--no-whole-archive"
)

target_include_directories(comlib PUBLIC "glog")
target_link_directories(comlib PUBLIC "glog")
target_link_libraries(comlib PUBLIC -lglog)
