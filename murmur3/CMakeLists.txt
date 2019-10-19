project(murmur3)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

add_library(${PROJECT_NAME} SHARED
    murmur3.c
    version.c
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION
        ${LIBMURMUR3_VERSION_MAJOR}.${LIBMURMUR3_VERSION_MINOR}

    SOVERSION
        ${LIBMURMUR3_VERSION_MAJOR}
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
        ${CMAKE_CURRENT_BINARY_DIR}/version.h

    DESTINATION
        include/murmur3
)

# vim: ts=4 sw=4 et nocindent
