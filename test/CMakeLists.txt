IF(WIN32)
MESSAGE(STATUS "on windows")
ENDIF()

IF(UNIX)
MESSAGE(STATUS "on linux")
ENDIF()

CMAKE_MINIMUM_REQUIRED(VERSION 3.6)
SET(TEST_ENCODE_SRC test_all_encoder.cpp)
INCLUDE_DIRECTORIES(/usr/local/include)
ADD_EXECUTABLE(test_all_encoder ${TEST_ENCODE_SRC})

FIND_LIBRARY(LIB_ALL_ENC all_encoder /usr/local/lib)
IF(NOT LIB_ALL_ENC)
MESSAGE(FATAL_ERROR "liball_encoder not found")
ENDIF()

MESSAGE(STATUS "liball_encoder:" ${LIB_ALL_ENC}) 
TARGET_LINK_LIBRARIES(test_all_encoder ${LIB_ALL_ENC})
