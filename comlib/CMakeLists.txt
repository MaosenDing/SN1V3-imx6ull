

#add_compile_options(-Ofast)

IF(useShared STREQUAL "1")
add_library(comlib SHARED jd_share.cpp mem_share.cpp)
INSTALL(TARGETS comlib LIBRARY DESTINATION lib)
else()
add_library(comlib STATIC jd_share.cpp mem_share.cpp)
endif()

