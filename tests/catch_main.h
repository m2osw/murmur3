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
#pragma once


// catch2
//
#include    <catch2/snapcatch2.hpp>


// snapdev
//
#include    <snapdev/hexadecimal_string.h>


// C++
//
#include    <string>
#include    <cstring>
#include    <cstdlib>
#include    <iostream>


namespace SNAP_CATCH2_NAMESPACE
{


inline std::string hex32(std::uint32_t *hash)
{
    //sprintf(buf, "%08x", *hash);
    return snapdev::int_to_hex(hash[0], false, 8);
}

inline std::string hex128(std::uint32_t *hash)
{
    //sprintf(buf, "%08x%08x%08x%08x", hash[0], hash[1], hash[2], hash[3]);
    return snapdev::int_to_hex(hash[0], false, 8)
         + snapdev::int_to_hex(hash[1], false, 8)
         + snapdev::int_to_hex(hash[2], false, 8)
         + snapdev::int_to_hex(hash[3], false, 8);
}



} // SNAP_CATCH2_NAMESPACE namespace
// vim: ts=4 sw=4 et
