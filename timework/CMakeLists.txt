

#add_compile_options(-Ofast)

add_library(SN1SG2 STATIC SN1SG_2.cpp)
target_compile_options(SN1SG2 PRIVATE -Ofast)

add_library(timework STATIC tableWork.cpp timeOperator.cpp timeTable.cpp timRegType_1.cpp timSet.c)
target_link_libraries(timework SN1SG2)

