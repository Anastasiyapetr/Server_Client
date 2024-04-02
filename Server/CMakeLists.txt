cmake_minimum_required(VERSION 3.22)
project(server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wextra")

set(COMMANDS_DIR COMMANDS)
set(COMMANDS_NAME COMMANDS)

set(SERVER_DIR SERVER)
set(SERVER_NAME SERVER)

set(INCLUDE_DIR_NAME include)
set(HRC_REGEX include/*.hpp)
set(SRC_REGEX src/*.cpp)


#libraries
add_subdirectory(${COMMANDS_DIR})
include_directories(${COMMANDS_DIR}/${INCLUDE_DIR_NAME})

add_subdirectory(${SERVER_DIR})
include_directories(${SERVER_DIR}/${INCLUDE_DIR_NAME})

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${COMMANDS_NAME}
    ${SERVER_NAME}
)

