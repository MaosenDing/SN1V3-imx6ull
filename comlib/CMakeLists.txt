

#add_compile_options(-Ofast)

set(fil jd_share.cpp mem_share.cpp versions.cpp)

IF(ENABLE_SHARED_COM)
add_library(comlib SHARED ${fil})
INSTALL(TARGETS comlib LIBRARY DESTINATION lib)
else()
add_library(comlib STATIC ${fil})
endif()

