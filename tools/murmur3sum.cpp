// Copyright (c) 2012-2022  Made to Order Software Corp.  All Rights Reserved
//
// https://snapwebsites.org/project/libmurmur3
// contact@m2osw.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.


// murmur3
//
#include    <murmur3/stream.h>


// C++
//
#include    <iostream>
#include    <vector>


// C
//
#include    <stdio.h>
#include    <string.h>


namespace
{

std::string                     g_progname = std::string();
bool                            g_stream = true;
std::vector<std::string>        g_filenames = {};


void usage()
{
    std::cout << "Usage: " << g_progname << " [--<opts>] [--] [-] [filename]\n";
    std::cout << "where --opts is one or more of:\n";
    std::cout << "   -h | --help     print out this help screen\n";
}


void generate_hashes()
{
    for(auto const & f : g_filenames)
    {
        FILE * in(nullptr);
        if(f == "-")
        {
            in = stdin;
        }
        else
        {
            in = fopen(f.c_str(), "rb");
            if(in == nullptr)
            {
                int const e(errno);
                std::cerr << "error:" << g_progname
                    << ": could not open \"" << f << "\" for reading: "
                    << strerror(e) << "\n";
                continue;
            }
        }
        bool valid(true);
        murmur3::murmur3_stream sum(0);  // no seeding (add option to command line?)
        for(;;)
        {
            char buffer[4096];
            std::size_t const size(fread(buffer, sizeof(buffer[0]), sizeof(buffer) / sizeof(buffer[0]), in));
            if(size == 0)
            {
                if(ferror(in))
                {
                    int const e(errno);
                    std::cerr
                        << "error:"
                        << g_progname
                        << ": an I/O error occurred while reading \""
                        << f
                        << "\": "
                        << strerror(e)
                        << "\n";
                    valid = false;
                }
                break;
            }
            sum.add_data(buffer, size);
        }
        if(valid)
        {
            std::string hash;
            sum.flush(hash);
            std::cout << hash << "  " << f << "\n";
        }
        if(f != "-")
        {
            fclose(in);
        }
    }
}


} // no name namespace


int main(int argc, char * argv[])
{
    g_progname = argv[0];
    std::string::size_type const slash(g_progname.rfind('/'));
    if(slash != std::string::npos)
    {
        g_progname = g_progname.substr(slash + 1);
    }

    bool filename_only(false);
    for(int i(1); i < argc; ++i)
    {
        if(!filename_only && argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
            case '-':   // long option
                if(argv[i][2])
                {
                    // found "--", only filenames after that
                    filename_only = true;
                    break;
                }
                if(strcmp(argv[i] + 2, "help") == 0)
                {
                    usage();
                    return 1;
                }
                break;

            case 'h':
                usage();
                return 1;

            }
        }
        else
        {
            g_filenames.push_back(argv[i]);
        }
    }

    // if no filename was specified, use stdin
    //
    if(g_filenames.empty())
    {
        g_filenames.push_back("-");
    }

    generate_hashes();
}

// vim: ts=4 sw=4 et
