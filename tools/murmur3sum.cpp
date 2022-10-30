// Copyright (c) 2012-2022  Made to Order Software Corp.  All Rights Reserved
//
// https://snapwebsites.org/project/murmur3
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
#include    <murmur3/version.h>


// snapdev
//
#include    <snapdev/hexadecimal_string.h>


// C++
//
#include    <iostream>
#include    <vector>


// C
//
#include    <limits.h>
#include    <stdio.h>
#include    <string.h>


namespace
{

std::string                     g_progname = std::string();
bool                            g_stream = true;
bool                            g_basename = false;
bool                            g_no_filename = false;
bool                            g_check = false;
bool                            g_quiet = false;
std::vector<std::string>        g_filenames = {};
std::size_t                     g_error = 0;


void usage()
{
    std::cout << "Usage: " << g_progname << " [--<opts>] [--] [-] [filename]\n";
    std::cout << "where --opts is one or more of:\n";
    std::cout << "   -b | --binary   read file in binary\n";
    std::cout << "   -c | --check    the input files are lists of checksum, read the corresponding file and verify the hash\n";
    std::cout << "   -h | --help     print out this help screen\n";
    std::cout << "   --basename      only print basename of filename\n";
    std::cout << "   --license       print out version and license information\n";
    std::cout << "   --no-filename   output the hash without the filename\n";
    std::cout << "   -t | --text     read file in text (this is ignored, we always read in binary)\n";
    std::cout << "   --quiet         check quietly (do not print OK for each file)\n";
    std::cout << "   --version       print out version and license information\n";
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
                std::cerr
                    << "error:"
                    << g_progname
                    << ": could not open \""
                    << f
                    << "\" for reading: "
                    << strerror(e)
                    << "\n";
                ++g_error;
                continue;
            }
        }
        bool valid(true);
        murmur3::stream sum(0);  // no seeding (add option to command line?)
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
                    ++g_error;
                    valid = false;
                }
                break;
            }
            sum.add_data(buffer, size);
        }
        if(valid)
        {
            std::cout << sum.flush().to_string();
            if(!g_no_filename)
            {
                std::string::size_type const last_slash((g_basename ? f.rfind('/') : std::string::npos) + 1);
                std::cout << "  " << f.substr(last_slash);
            }
            std::cout << "\n";
        }
        if(f != "-")
        {
            fclose(in);
        }
    }
}


void check_hashes()
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
                std::cerr
                    << "error:"
                    << g_progname
                    << ": could not open \""
                    << f
                    << "\" for reading: "
                    << strerror(e)
                    << "\n";
                ++g_error;
                continue;
            }
        }
        char line[murmur3::HASH_SIZE * 2 + 2 + PATH_MAX + 2]; // try to allow for a long filename
        line[sizeof(line) - 1] = '\0';
        int l(0);
        while(fgets(line, sizeof(line), in) != nullptr)
        {
            ++l;

            // make sure the input line is valid (not empty, not too long)
            //
            std::size_t const len(strlen(line));
            if(len == 0)
            {
                std::cerr
                    << "error: "
                    << g_progname
                    << ':'
                    << l
                    << ": could not read line of data (empty).\n";
                ++g_error;
                break;
            }
            if(line[len - 1] != '\n')
            {
                std::cerr
                    << "error:"
                    << g_progname
                    << ':'
                    << l
                    << ": line too long.\n";
                ++g_error;
                break;
            }
            if(len == 1)
            {
                // ignore empty lines
                //
                continue;
            }
            if(line[0] == '#')
            {
                // ignore lines commented out
                //
                continue;
            }

            // extract the expected hash from the murmur3 file
            //
            std::string const hex(line, 32);
            murmur3::hash expected_hash;
            try
            {
                expected_hash.from_string(hex);
            }
            catch(snapdev::hexadecimal_string_invalid_parameter const & e)
            {
                std::cerr
                    << "error:"
                    << g_progname
                    << ':'
                    << l
                    << ": hash ("
                    << hex
                    << ") could not be decoded: "
                    << e.what()
                    << "\n";
                ++g_error;
                continue;
            }

            // retrieve the filename
            //
            // TBD: do we want to verify the two spaces between hash & filename?
            //
            std::string const filename(line + 34, len - 34 - 1);    // -1 for the '\n'
            murmur3::hash const computed_hash(murmur3::sum(filename));

            if(expected_hash != computed_hash)
            {
                std::cerr
                    << "error:"
                    << g_progname
                    << ':'
                    << l
                    << ": hash found in checksum file ("
                    << hex
                    << ") does not match hash ("
                    << computed_hash.to_string()
                    << ") computed from file \""
                    << filename
                    << "\".\n";
                ++g_error;
                continue;
            }

            if(!g_quiet)
            {
                std::cout << filename << ": OK\n";
            }
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
                if(argv[i][2] == '\0')
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
                if(strcmp(argv[i] + 2, "basename") == 0)
                {
                    g_basename = true;
                }
                else if(strcmp(argv[i] + 2, "binary") == 0)
                {
                    ; // ignore
                }
                else if(strcmp(argv[i] + 2, "check") == 0)
                {
                    g_check = true;
                }
                else if(strcmp(argv[i] + 2, "no-filename") == 0)
                {
                    g_no_filename = true;
                }
                else if(strcmp(argv[i] + 2, "quiet") == 0)
                {
                    g_quiet = true;
                }
                else if(strcmp(argv[i] + 2, "text") == 0)
                {
                    ; // ignore
                }
                else if(strcmp(argv[i] + 2, "license") == 0
                     || strcmp(argv[i] + 2, "version") == 0)
                {
                    std::cout
                        << g_progname
                        << " (Snap! C++ Project) "
                        << MURMUR3_VERSION_STRING
                        << '\n'
                        << "Copyright (C) 2022 Made to Order Software Corporation\n"
                           "License GPVv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
                           "This is free software: you are free to change and redistribute it.\n"
                           "There is NO WARRANTY, to the extent permitted by law.\n"
                           "\n"
                           "Written by Alexis Wilke, Peter Scott, and A. Appleby.\n";
                    return 0;
                }
                break;

            case 'h':
                usage();
                return 1;

            case 'c':
                g_check = true;
                break;

            case 'b': // binary
            case 't': // text
                // just ignore
                break;

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

    if(g_check)
    {
        check_hashes();
    }
    else
    {
        generate_hashes();
    }

    if(g_error != 0)
    {
        std::cerr << "error:"
            << g_progname
            << ": found "
            << g_error
            << " errors.\n";
        return 1;
    }

    return 0;
}

// vim: ts=4 sw=4 et
