cmake_minimum_required(VERSION 3.0)

# set directory as project name
get_filename_component(PROJECT ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" PROJECT ${PROJECT})
project(${PROJECT} C CXX)

# set compiler flags
set(CMAKE_OPTIMIZATION "-O3")
if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    set(CMAKE_OPTIMIZATION "-O0")
endif()
set(CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_OPTIMIZATION}")
set(CMAKE_C_FLAGS "${CMAKE_OPTIMIZATION}")
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_DEBUG "-ggdb3 -Wall -Wextra -DDEBUG   \
    -Wno-format -Wno-write-strings -Wno-unused-function \
    -Wno-unused-parameter -Wno-system-headers           \
    -Wno-format-security -Wno-ignored-qualifiers")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

# set source files
set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

file(GLOB HEADERS "${INCLUDE_DIR}/*.h" "${INCLUDE_DIR}/*.hxx")
file(GLOB SOURCES ${SOURCE_DIR}/*.cc)

set(LIB_SOURCES ${SOURCES})
list(REMOVE_ITEM LIB_SOURCES ${SOURCE_DIR}/main.cc)

## set include directories
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    get_filename_component(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
endif()
include_directories( ${INCLUDE_DIR} ${CMAKE_INSTALL_PREFIX}/include)

# create executable
add_executable(${PROJECT}-bin ${SOURCES})
set_target_properties(${PROJECT}-bin PROPERTIES OUTPUT_NAME ${PROJECT})
target_link_libraries(${PROJECT}-bin pthread)

# create library
add_library(${PROJECT} STATIC ${LIB_SOURCES})
target_link_libraries(${PROJECT} pthread)

# set install locations
install(TARGETS ${PROJECT}-bin
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
)

set_target_properties(${PROJECT} PROPERTIES PUBLIC_HEADER "${HEADERS}")
install(TARGETS ${PROJECT}
    ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${PROJECT}"
)

