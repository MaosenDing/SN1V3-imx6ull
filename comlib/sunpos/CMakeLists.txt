



set_source_files_properties(getIdealSunPoint.cpp PROPERTIES COMPILE_FLAGS "-O3 ")
set_source_files_properties(SunPos.cpp PROPERTIES COMPILE_FLAGS "-O3 ")
set_source_files_properties(suntable.cpp PROPERTIES COMPILE_FLAGS "-O3 ")

add_library(sunpos OBJECT getIdealSunPoint.cpp SunPos.cpp suntable.cpp)
