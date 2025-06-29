# - Find Murmur3
#
# MURMUR3_FOUND        - System has Murmur3
# MURMUR3_INCLUDE_DIRS - The Murmur3 include directories
# MURMUR3_LIBRARIES    - The libraries needed to use Murmur3
# MURMUR3_DEFINITIONS  - Compiler switches required for using Murmur3
#
# License:
#
# Copyright (c) 2011-2025  Made to Order Software Corp.  All Rights Reserved
#
# https://snapwebsites.org/project/murmur3
# contact@m2osw.com
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

find_path(
    MURMUR3_INCLUDE_DIR
        murmur3/murmur3.h

    PATHS
        ENV MURMUR3_INCLUDE_DIR
)
find_library(
    MURMUR3_LIBRARY
        murmur3

    PATHS
        ${MURMUR3_LIBRARY_DIR}
        ENV MURMUR3_LIBRARY
)
mark_as_advanced(
    MURMUR3_INCLUDE_DIR
    MURMUR3_LIBRARY
)

set(MURMUR3_INCLUDE_DIRS ${MURMUR3_INCLUDE_DIR})
set(MURMUR3_LIBRARIES    ${MURMUR3_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Murmur3
    REQUIRED_VARS
        MURMUR3_INCLUDE_DIR
        MURMUR3_LIBRARY
)

# vim: ts=4 sw=4 et nocindent
