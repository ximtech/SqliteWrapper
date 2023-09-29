cmake_minimum_required(VERSION 3.16)
project(Tests C)

set(CMAKE_C_STANDARD 99)

set(ROOT_DIR "..")

include_directories(${ROOT_DIR}/ resources)

get_filename_component(BUILD_DIRECTORY_NAME "${CMAKE_CURRENT_BINARY_DIR}" NAME)
add_subdirectory(${ROOT_DIR} ${BUILD_DIRECTORY_NAME})

add_executable(Tests
        main.c
        munit/munit.h
        munit/munit.c
        munit/example.c

        resources/sqlite3.h
        resources/sqlite3.c)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/resources)

target_link_libraries(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} SqliteWrapper)