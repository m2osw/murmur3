// Copyright (c) 2006-2023  Made to Order Software Corp.  All Rights Reserved
//
// https://snapwebsites.org/project/murmur3
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

// Tell catch we want it to add the runner code in this file.
#define CATCH_CONFIG_RUNNER

// self
//
#include    "catch_main.h"


// murmur3
//
#include    <murmur3/version.h>


// snapdev
//
//#include    <snapdev/not_used.h>


// C++
//
//#include    <sstream>


// last include
//
#include    <snapdev/poison.h>





namespace SNAP_CATCH2_NAMESPACE
{






}
// SNAP_CATCH2_NAMESPACE namespace




int main(int argc, char * argv[])
{
    return SNAP_CATCH2_NAMESPACE::snap_catch2_main(
              "murmur3"
            , MURMUR3_VERSION_STRING
            , argc
            , argv
            , nullptr
            , nullptr
            , nullptr
            , nullptr
        );
}

// vim: ts=4 sw=4 et
