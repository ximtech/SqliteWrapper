cmake_minimum_required(VERSION 3.16)
project(SqliteWrapper VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

include(cmake/CPM.cmake)

CPMAddPackage(
        NAME Collections
        GITHUB_REPOSITORY ximtech/Collections
        GIT_TAG origin/main)

set(SOURCE_FILES
        include/SqliteParameter.h
        include/SqliteResultSet.h
        include/SqliteQuery.h
        include/SqliteWrapper.h

        SqliteQuery.c
        SqliteResultSet.c
        SqliteWrapper.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_include_directories(${PROJECT_NAME} PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(${PROJECT_NAME} Collections)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}.h
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Export
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}Export
        FILE ${PROJECT_NAME}-config.cmake
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME})
