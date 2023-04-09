project(murmur3)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

add_library(${PROJECT_NAME} SHARED
    murmur3.c
    stream.cpp
    version.c
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${LIBEXCEPT_INCLUDE_DIRS}
        ${SNAPDEV_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    ${LIBEXCEPT_LIBRARIES}
    ${SNAPDEV_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION
        ${MURMUR3_VERSION_MAJOR}.${MURMUR3_VERSION_MINOR}

    SOVERSION
        ${MURMUR3_VERSION_MAJOR}
)

install(
    TARGETS
        ${PROJECT_NAME}

    RUNTIME DESTINATION
        bin

    LIBRARY DESTINATION
        lib

    ARCHIVE DESTINATION
        lib
)

install(
    FILES
        murmur3.h
        stream.h
        ${CMAKE_CURRENT_BINARY_DIR}/version.h

    DESTINATION
        include/murmur3
)

# vim: ts=4 sw=4 et nocindent
