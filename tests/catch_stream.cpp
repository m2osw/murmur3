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
#include    <snapdev/math.h>
#include    <snapdev/ostream_int128.h>


// C++
//
#include    <iomanip>


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

    CATCH_START_SECTION("Stream with random seed")
    {
        std::size_t size(0);
        getrandom(&size, sizeof(size), 0);
        size %= 256;
        size += 25; // 25 to 280

        char buf[size];
        getrandom(buf, sizeof(buf), 0);

        murmur3::stream sum;
        sum.add_data(buf, sizeof(buf));

        murmur3::seed_t seed1(0);
        murmur3::seed_t seed2(0);
        sum.get_seeds(seed1, seed2);

        std::uint32_t hash[4];
        MurmurHash3_x64_128_128(buf, sizeof(buf), seed1, seed2, hash);
        std::string const c_hash(SNAP_CATCH2_NAMESPACE::hex128(hash));

        CATCH_CHECK(sum.flush().to_string() == c_hash);
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

            murmur3::hash expected_hash;
            expected_hash.set(reinterpret_cast<std::uint8_t const *>(hash));
//std::cerr << "-- full hash: " << c_hash << " -> " << expected_hash.to_string() << "\n";
            CATCH_CHECK(sum.flush() == expected_hash);
            CATCH_CHECK(sum.flush() <= expected_hash);
            CATCH_CHECK(sum.flush() >= expected_hash);

            murmur3::hash unexpected_hash;
            std::uint32_t unhash[4] = { ~hash[0], ~hash[1], ~hash[2], ~hash[3] };
            unexpected_hash.set(reinterpret_cast<std::uint8_t const *>(unhash));
            CATCH_CHECK(sum.flush() != unexpected_hash);

            murmur3::hash_t const value(expected_hash.to_uint128());
            murmur3::hash_t const unvalue(unexpected_hash.to_uint128());
            if(snapdev::bswap_128(value) < snapdev::bswap_128(unvalue))
            {
                CATCH_CHECK(sum.flush() < unexpected_hash);
                CATCH_CHECK_FALSE(sum.flush() > unexpected_hash);
            }
            else
            {
                CATCH_CHECK_FALSE(sum.flush() < unexpected_hash);
                CATCH_CHECK(sum.flush() > unexpected_hash);
            }

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

    CATCH_START_SECTION("1 to 15 bytes sent first")
    {
        // test all possibilities in the switch()
        //
        for(std::size_t size(1); size < 16; ++size)
        {
            char buf[size + 16];
            getrandom(buf, sizeof(buf), 0);

            // first compute the hash with the basic C function
            //
            std::uint32_t hash[4];
            MurmurHash3_x64_128(buf, sizeof(buf), 0, hash);
            std::string c_hash(SNAP_CATCH2_NAMESPACE::hex128(hash));

            // second compute the same hash with the stream function
            // starting with a few bytes
            //
            murmur3::stream sum(0);
            sum.add_data(buf, size);
            sum.add_data(buf + size, 16);
            CATCH_CHECK(sum.flush().to_string() == c_hash);
        }
    }
    CATCH_END_SECTION()

    CATCH_START_SECTION("Irregular number of incoming bytes")
    {
        // test all possibilities in the switch()
        //
        for(std::size_t count(0); count < 25; ++count)
        {
            std::size_t size(0);
            getrandom(&size, sizeof(size), 0);
            size %= 256;
            size += 25; // 25 to 280

            char buf[size];
            getrandom(buf, sizeof(buf), 0);

            // first compute the hash with the basic C function
            //
            std::uint32_t hash[4];
            MurmurHash3_x64_128(buf, sizeof(buf), 0, hash);
            std::string c_hash(SNAP_CATCH2_NAMESPACE::hex128(hash));

            murmur3::stream sum(0);
            std::size_t pos(0);
            while(pos < size)
            {
                std::size_t incr(0);
                getrandom(&incr, sizeof(incr), 0);
                incr %= 15;
                ++incr; // 1 to 15
                if(pos + incr > size)
                {
                    incr = size - pos;
                }

                sum.add_data(buf + pos, incr);

                pos += incr;
            }
            CATCH_CHECK(sum.flush().to_string() == c_hash);
        }
    }
    CATCH_END_SECTION()
}


CATCH_TEST_CASE("hash", "[hash][valid]")
{
    CATCH_START_SECTION("Hash validation")
    {
        murmur3::hash_t value;
        getrandom(&value, sizeof(value), 0);
        murmur3::hash h;
        h.set(reinterpret_cast<std::uint8_t const *>(&value));
        CATCH_CHECK(value == h.to_uint128());

        murmur3::hash_t verify;
        memcpy(&verify, h.get(), murmur3::HASH_SIZE);
        CATCH_CHECK(value == verify);

        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(8) << static_cast<std::uint32_t>(value);
        ss << std::hex << std::setfill('0') << std::setw(8) << static_cast<std::uint32_t>(value >> 32);
        ss << std::hex << std::setfill('0') << std::setw(8) << static_cast<std::uint32_t>(value >> 64);
        ss << std::hex << std::setfill('0') << std::setw(8) << static_cast<std::uint32_t>(value >> 96);
        CATCH_CHECK(ss.str() == h.to_string());

        murmur3::hash h2;
        h2.from_string(ss.str());
        CATCH_CHECK(value == h2.to_uint128());
    }
    CATCH_END_SECTION()

    CATCH_START_SECTION("Hash starts with zeroes")
    {
        murmur3::hash_t value;
        getrandom(&value, sizeof(value), 0);
        value &= ~static_cast<murmur3::hash_t>(0xFFFFFFFF);
        murmur3::hash h;
        h.set(reinterpret_cast<std::uint8_t const *>(&value));
        CATCH_CHECK(value == h.to_uint128());

        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(8) << static_cast<std::uint32_t>(value);
        ss << std::hex << std::setfill('0') << std::setw(8) << static_cast<std::uint32_t>(value >> 32);
        ss << std::hex << std::setfill('0') << std::setw(8) << static_cast<std::uint32_t>(value >> 64);
        ss << std::hex << std::setfill('0') << std::setw(8) << static_cast<std::uint32_t>(value >> 96);
        CATCH_CHECK(ss.str() == h.to_string());

        std::string const without_the_zeroes(ss.str().substr(8));
        murmur3::hash zh;
        zh.from_string(without_the_zeroes);
        CATCH_CHECK(value == zh.to_uint128());
    }
    CATCH_END_SECTION()
}


CATCH_TEST_CASE("invalid_hash", "[hash][invalid]")
{
    CATCH_START_SECTION("Invalid Hash (odd size)")
    {
        murmur3::hash h;

        // invalid characters
        //
        CATCH_REQUIRE_THROWS_MATCHES(
                      h.from_string("abc")
                    , snapdev::hexadecimal_string_invalid_parameter
                    , Catch::Matchers::ExceptionMessage(
                              "hexadecimal_string_exception: the hex parameter"
                              " must have an even size."));
    }
    CATCH_END_SECTION()

    CATCH_START_SECTION("Invalid Hash (not hex. characters)")
    {
        murmur3::hash h;

        // invalid characters
        //
        CATCH_REQUIRE_THROWS_MATCHES(
                      h.from_string("abcdefghijkl")
                    , snapdev::hexadecimal_string_invalid_parameter
                    , Catch::Matchers::ExceptionMessage(
                              "hexadecimal_string_exception: input character"
                              " 'g' is not an hexadecimal digit."));
    }
    CATCH_END_SECTION()

    CATCH_START_SECTION("Invalid Hash (too long)")
    {
        murmur3::hash h;

        // invalid characters
        //
        CATCH_REQUIRE_THROWS_MATCHES(
                      h.from_string("1111111111111111111111111111111111")
                    , snapdev::hexadecimal_string_invalid_parameter
                    , Catch::Matchers::ExceptionMessage(
                              "hexadecimal_string_exception: \"1111111111111111111111111111111111\" is not a valid 128 bit murmur3 hash value; it is too long."));
    }
    CATCH_END_SECTION()
}


// vim: ts=4 sw=4 et
