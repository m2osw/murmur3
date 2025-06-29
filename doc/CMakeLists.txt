# Copyright (c) 2022-2025  Made to Order Software Corp.  All Rights Reserved.
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


##
## Documentation
##
find_package(SnapDoxygen)
AddDoxygenTarget(murmur3
    ${MURMUR3_VERSION_MAJOR}
    ${MURMUR3_VERSION_MINOR}
    ${MURMUR3_VERSION_PATCH}
)

install(
    FILES
        murmur3sum.1

    DESTINATION
        share/man/man1
)

# vim: ts=4 sw=4 et
