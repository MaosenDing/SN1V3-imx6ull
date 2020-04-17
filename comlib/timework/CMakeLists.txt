


set_source_files_properties(SN1SG_2.cpp PROPERTIES COMPILE_FLAGS -Ofast)

add_library(timework OBJECT tableWork.cpp timeOperator.cpp timeTable.cpp timeTableV2.cpp timRegType_1.cpp timSet.c SN1SG_2.cpp)


