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

// murmur3
//
#include    <murmur3/stream.h>

#include    <murmur3/murmur3.h>


// self
//
#include    "catch_main.h"


// snapdev
//
#include    <snapdev/file_contents.h>
#include    <snapdev/glob_to_list.h>


// C
//
#include    <sys/random.h>


// last include
//
#include    <snapdev/poison.h>



CATCH_TEST_CASE("stream_basic", "[stream][valid]")
{
    CATCH_START_SECTION("Verify small stream (Hello World!)")
    {
        murmur3::stream sum(123, 123);

        murmur3::seed_t seed1(0);
        murmur3::seed_t seed2(0);
        sum.get_seeds(seed1, seed2);
        CATCH_CHECK(seed1 == 123);
        CATCH_CHECK(seed2 == 123);

        std::string const hello_world("Hello, world!");
        sum.add_data(hello_world.c_str(), hello_world.length());
        CATCH_CHECK(sum.flush().to_string() == "8743acad421c8c73d373c3f5f19732fd");

        CATCH_CHECK(murmur3::sum(hello_world.c_str(), hello_world.length(), 123).to_string() == "8743acad421c8c73d373c3f5f19732fd");

        // verify the reset()
        //
        sum.reset();

        sum.add_data(hello_world.c_str(), hello_world.length());
        CATCH_CHECK(sum.flush().to_string() == "8743acad421c8c73d373c3f5f19732fd");

        // verify different seed
        //
        sum.reset(321);

        sum.add_data(hello_world.c_str(), hello_world.length());
        CATCH_CHECK(sum.flush().to_string() == "f86d4004ca47f42bb9546c7979200aee");

        CATCH_CHECK(murmur3::sum(hello_world.c_str(), hello_world.length(), 321).to_string() == "f86d4004ca47f42bb9546c7979200aee");
    }
    CATCH_END_SECTION()

    CATCH_START_SECTION("X28 stream")
    {
        murmur3::stream sum(123, 123);

        murmur3::seed_t seed1(0);
        murmur3::seed_t seed2(0);
        sum.get_seeds(seed1, seed2);
        CATCH_CHECK(seed1 == 123);
        CATCH_CHECK(seed2 == 123);

        std::string const x28("xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
        sum.add_data(x28.c_str(), x28.length());
        CATCH_CHECK(sum.flush().to_string() == "becf7e04dbcf74637751664ef66e73e0");

        CATCH_CHECK(murmur3::sum(x28.c_str(), x28.length(), 123).to_string() == "becf7e04dbcf74637751664ef66e73e0");
    }
    CATCH_END_SECTION()

    CATCH_START_SECTION("Empty file stream")
    {
        murmur3::stream sum(123, 123);

        murmur3::seed_t seed1(0);
        murmur3::seed_t seed2(0);
        sum.get_seeds(seed1, seed2);
        CATCH_CHECK(seed1 == 123);
        CATCH_CHECK(seed2 == 123);

        std::string const empty("");
        sum.add_data(empty.c_str(), empty.length());
        CATCH_CHECK(sum.flush().to_string() == "4cd9597081679d1abd92f8784bace33d");

        CATCH_CHECK(murmur3::sum(empty.c_str(), empty.length(), 123).to_string() == "4cd9597081679d1abd92f8784bace33d");
    }
    CATCH_END_SECTION()
}


CATCH_TEST_CASE("stream_file", "[stream][valid]")
{
    CATCH_START_SECTION("Stream files")
    {
        snapdev::glob_to_list<std::list<snapdev::file>> glob;
        CATCH_REQUIRE(glob.read_path<
                 snapdev::glob_to_list_flag_t::GLOB_FLAG_IGNORE_ERRORS,
                 snapdev::glob_to_list_flag_t::GLOB_FLAG_PERIOD>("..."));

        for(auto const & f : glob)
        {
            if(!f.is_regular_file())
            {
                // skip directories and special files
                //
                continue;
            }
            if(SNAP_CATCH2_NAMESPACE::g_verbose())
            {
                std::cout << "-- testing with file \"" << f.filename() << "\".\n";
            }

            snapdev::file_contents file(f.filename());
            CATCH_REQUIRE(file.read_all());
            std::string const & contents(file.contents());

            // first compute the hash with the basic C function
            //
            std::uint32_t hash[4];
            MurmurHash3_x64_128(contents.c_str(), contents.length(), 0, hash);
            std::string c_hash(SNAP_CATCH2_NAMESPACE::hex128(hash));

            // second compute the same hash with the stream function 4Kb at
            // a time so we should hit all possible cases
            //
            constexpr std::size_t four_kb(1024 * 4);
            murmur3::stream sum(0);
            std::size_t const nblocks(contents.length() / four_kb);
            std::size_t const left_over(contents.length() % four_kb);
            for(std::size_t b(0); b < nblocks; ++b)
            {
                sum.add_data(contents.c_str() + b * four_kb, four_kb);
            }
            if(left_over != 0)
            {
                sum.add_data(contents.c_str() + nblocks * four_kb, left_over);
            }

            CATCH_CHECK(sum.flush().to_string() == c_hash);

            // test the helper function that computes the murmur3 of a file
            // using the streaming mechanism (to avoid loading the file in
            // memory all at once)
            //
            CATCH_CHECK(murmur3::sum(f.filename()).to_string() == c_hash);
        }
    }
    CATCH_END_SECTION()

    CATCH_START_SECTION("Stream 4Kb exactly (special case)")
    {
        constexpr std::size_t four_kb(1024 * 4);
        char buf[four_kb];
        getrandom(buf, sizeof(buf), 0);

        // first compute the hash with the basic C function
        //
        std::uint32_t hash[4];
        MurmurHash3_x64_128(buf, sizeof(buf), 0, hash);
        std::string c_hash(SNAP_CATCH2_NAMESPACE::hex128(hash));

        // second compute the same hash with the stream function 4Kb at
        // a time so we should hit all possible cases
        //
        murmur3::stream sum(0);
        sum.add_data(buf, sizeof(buf));
        CATCH_CHECK(sum.flush().to_string() == c_hash);

        // test the helper function that computes the murmur3 of a file
        // using the streaming mechanism (to avoid loading the file in
        // memory all at once)
        //
        std::string const tmp_dir(SNAP_CATCH2_NAMESPACE::g_tmp_dir());
        std::string const test_filename(tmp_dir + "/4kb.bin");
        std::ofstream out(test_filename);
        out.write(buf, sizeof(buf));
        CATCH_CHECK(murmur3::sum(test_filename).to_string() == c_hash);
    }
    CATCH_END_SECTION()

    CATCH_START_SECTION("Stream 0Kb to 4Kb (many special cases)")
    {
        constexpr std::size_t four_kb(1024 * 4);
        for(std::size_t size(0); size < four_kb; ++size)
        {
            char buf[size];
            getrandom(buf, sizeof(buf), 0);

            // first compute the hash with the basic C function
            //
            std::uint32_t hash[4];
            MurmurHash3_x64_128(buf, sizeof(buf), 0, hash);
            std::string c_hash(SNAP_CATCH2_NAMESPACE::hex128(hash));

            // second compute the same hash with the stream function 4Kb at
            // a time so we should hit all possible cases
            //
            murmur3::stream sum(0);
            sum.add_data(buf, sizeof(buf));
            CATCH_CHECK(sum.flush().to_string() == c_hash);

            // test the helper function that computes the murmur3 of a file
            // using the streaming mechanism (to avoid loading the file in
            // memory all at once)
            //
            std::string const tmp_dir(SNAP_CATCH2_NAMESPACE::g_tmp_dir());
            std::string const test_filename(tmp_dir + "/" + std::to_string(size) + "_bytes.bin");
            {
                std::ofstream out(test_filename);
                out.write(buf, sizeof(buf));
            }
//std::cerr << "-- test with file [" << test_filename << "] -> " << murmur3::sum(test_filename).to_string() << "\n";
            CATCH_CHECK(murmur3::sum(test_filename).to_string() == c_hash);
        }
    }
    CATCH_END_SECTION()
}


// vim: ts=4 sw=4 et
