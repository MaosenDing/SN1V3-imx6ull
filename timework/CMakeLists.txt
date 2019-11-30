

#add_compile_options(-Ofast)

add_library(SN1SG2 STATIC SN1SG_2.cpp)
target_compile_options(SN1SG2 PRIVATE -Ofast)


IF(ENABLE_SHARED_TIM)
add_library(timework SHARED tableWork.cpp timeOperator.cpp timeTable.cpp timRegType_1.cpp timSet.c)
INSTALL(TARGETS timework LIBRARY DESTINATION lib)
else()
add_library(timework STATIC tableWork.cpp timeOperator.cpp timeTable.cpp timRegType_1.cpp timSet.c)
endif()



target_link_libraries(timework SN1SG2)

