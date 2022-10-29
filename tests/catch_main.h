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
#pragma once


// advgetopt
//
#include    <advgetopt/advgetopt.h>


// cppthread
//
#include    <cppthread/log.h>


// catch2
//
#include    <catch2/snapcatch2.hpp>


// C++
//
#include    <string>
#include    <cstring>
#include    <cstdlib>
#include    <iostream>


namespace SNAP_CATCH2_NAMESPACE
{


extern std::string                  g_config_filename;
extern std::string                  g_config_project_filename;


void                                push_expected_log(std::string const & message);
void                                log_for_test(cppthread::log_level_t level, std::string const & message);
void                                expected_logs_stack_is_empty();

void                                init_tmp_dir(std::string const & project_name, std::string const & prefname, bool dir = false);


} // SNAP_CATCH2_NAMESPACE namespace
// vim: ts=4 sw=4 et
