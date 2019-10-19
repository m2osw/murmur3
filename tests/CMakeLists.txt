project(unittest)

add_executable(${PROJECT_NAME}
    test.c
)

target_link_libraries(${PROJECT_NAME}
    murmur3
)

# vim: ts=4 sw=4 et
