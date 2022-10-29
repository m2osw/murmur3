# Murmur3 Hash

cmake_minimum_required(VERSION 3.10.2)

project(murmur3_library)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(LibExcept        REQUIRED)
find_package(SnapCMakeModules REQUIRED)
find_package(SnapDev          REQUIRED)

SnapGetVersion(MURMUR3 ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_subdirectory(cmake  )
add_subdirectory(murmur3)
add_subdirectory(tools  )
add_subdirectory(tests  )

# vim: ts=4 sw=4 et nocindent
