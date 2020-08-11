

#add_compile_options(-Ofast)
add_compile_options(-fPIC)
set(fil jd_share.cpp mem_share.cpp versions.cpp)

IF(ENABLE_SHARED_COM)
add_library(comlib SHARED ${fil})
INSTALL(TARGETS comlib LIBRARY DESTINATION lib)
add_library(comlib_static STATIC ${fil})
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
#sunpos
add_subdirectory(sunpos)


target_link_libraries(comlib
PRIVATE
"-Wl,--whole-archive" 
text
CRE
timework
datapro
errHandle
JD
sunpos
"-Wl,--no-whole-archive"
)

IF (CPU STREQUAL "A7")
set(GLOG_DIR "/share/imx/gloginstall/lib/cmake/glog")
ENDIF()

IF (CPU STREQUAL "A53")
set(GLOG_DIR "/share/a53/gloginstall/lib/cmake/glog")
ENDIF()

IF (CPU STREQUAL "arm9")
set(GLOG_DIR "/share/nuc/gloginstall/lib/cmake/glog")
ENDIF()

find_package(GLOG)
target_link_libraries(comlib PUBLIC glog::glog)



IF(ENABLE_SHARED_COM)
target_link_libraries(comlib_static PUBLIC glog::glog)
endif()