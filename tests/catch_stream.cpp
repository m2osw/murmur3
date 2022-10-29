// Copyright (c) 2006-2022  Made to Order Software Corp.  All Rights Reserved
//
// https://snapwebsites.org/project/advgetopt
// contact@m2osw.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

// advgetopt lib
//
#include    <advgetopt/version.h>


// self
//
#include    "catch_main.h"


// last include
//
#include    <snapdev/poison.h>



CATCH_TEST_CASE("version", "[version][valid]")
{
    CATCH_START_SECTION("Verify the test version")
        CATCH_REQUIRE(advgetopt::get_major_version() == LIBADVGETOPT_VERSION_MAJOR);
        CATCH_REQUIRE(advgetopt::get_release_version() == LIBADVGETOPT_VERSION_MINOR);
        CATCH_REQUIRE(advgetopt::get_patch_version() == LIBADVGETOPT_VERSION_PATCH);
        CATCH_REQUIRE(strcmp(advgetopt::get_version_string(), LIBADVGETOPT_VERSION_STRING) == 0);
    CATCH_END_SECTION()
}


// vim: ts=4 sw=4 et
