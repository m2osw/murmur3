# - Try to find Murmur3 library/headers
#
# Once done this will define
#
# MURMUR3_FOUND        - System has Murmur3
# MURMUR3_INCLUDE_DIRS - The Murmur3 include directories
# MURMUR3_LIBRARIES    - The libraries needed to use Murmur3
# MURMUR3_DEFINITIONS  - Compiler switches required for using Murmur3

find_path(MURMUR3_INCLUDE_DIR murmur3/murmur3.h
    PATHS ENV MURMUR3_INCLUDE_DIR
)
find_library(MURMUR3_LIBRARY murmur3
    PATHS ${MURMUR3_LIBRARY_DIR} ENV MURMUR3_LIBRARY
)
mark_as_advanced(MURMUR3_INCLUDE_DIR MURMUR3_LIBRARY)

set(MURMUR3_INCLUDE_DIRS ${MURMUR3_INCLUDE_DIR})
set(MURMUR3_LIBRARIES    ${MURMUR3_LIBRARY}    )

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set MURMUR3_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Murmur3 DEFAULT_MSG MURMUR3_INCLUDE_DIR MURMUR3_LIBRARY)

# vim: ts=4 sw=4 et nocindent
