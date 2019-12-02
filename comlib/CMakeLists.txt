

#add_compile_options(-Ofast)

set(fil jd_share.cpp mem_share.cpp versions.cpp)

IF(ENABLE_SHARED_COM)
add_library(comlib SHARED ${fil})
INSTALL(TARGETS comlib LIBRARY DESTINATION lib)
else()
add_library(comlib STATIC ${fil})
endif()


#imageProcess
add_subdirectory(imageProcess)
target_link_libraries(comlib imageProcess)
#errHandle
add_subdirectory(errHandle)
target_link_libraries(comlib errHandle)
#datapro
add_subdirectory(datapro)
target_link_libraries(comlib datapro)
#timework
add_subdirectory(timework)
target_link_libraries(comlib timework)
#CRE
add_subdirectory(CRE)
target_link_libraries(comlib CRE)
#text
add_subdirectory(text)
target_link_libraries(comlib text)