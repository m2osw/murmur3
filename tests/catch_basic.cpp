/* Tests */

// murmur3
//
#include    <murmur3/murmur3.h>


// self
//
#include    "catch_main.h"


// C
//
//#include    <stdio.h>
//#include    <stdlib.h>
//#include    <stdint.h>
//#include    <string.h>


// last include
//
#include    <snapdev/poison.h>




CATCH_TEST_CASE("basic", "[version][valid]")
{
    CATCH_START_SECTION("Verify C version")
    {
//#define TESTHASH(arch, nbytes, seed, str, expected) {                   \
//        char *input = str;                                              \
//        uint32_t hash[4];                                               \
//        char buf[33];                                                   \
//        MurmurHash3_##arch##_##nbytes(input, strlen(input), (seed), hash); \
//        hex##nbytes(hash, buf);                                         \
//        if (strcmp(buf, expected) != 0) {                               \
//            printf("FAIL(line %i): %s != %s\n", __LINE__, expected, buf); \
//            failed++;                                                   \
//        } else passed++;                                                \
//    }

        std::uint32_t hash[4];

        //TESTHASH(x86, 32, 1234, "Hello, world!", "faf6cdb3");
        std::string const hello_world("Hello, world!");
        MurmurHash3_x86_32(hello_world.c_str(), hello_world.length(), 1234, hash);
        CATCH_CHECK(SNAP_CATCH2_NAMESPACE::hex32(hash) == "faf6cdb3");

        //TESTHASH(x86, 32, 4321, "Hello, world!", "bf505788");
        MurmurHash3_x86_32(hello_world.c_str(), hello_world.length(), 4321, hash);
        CATCH_CHECK(SNAP_CATCH2_NAMESPACE::hex32(hash) == "bf505788");

        //TESTHASH(x86, 32, 1234, "xxxxxxxxxxxxxxxxxxxxxxxxxxxx", "8905ac28");
        std::string const x28("xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
        MurmurHash3_x86_32(x28.c_str(), x28.length(), 1234, hash);
        CATCH_CHECK(SNAP_CATCH2_NAMESPACE::hex32(hash) == "8905ac28");

        //TESTHASH(x86, 32, 1234, "", "0f2cc00b");
        std::string const empty("");
        MurmurHash3_x86_32(empty.c_str(), empty.length(), 1234, hash);
        CATCH_CHECK(SNAP_CATCH2_NAMESPACE::hex32(hash) == "0f2cc00b");

        //TESTHASH(x86, 128, 123, "Hello, world!", "61c9129e5a1aacd7a41621629e37c886");
        MurmurHash3_x86_128(hello_world.c_str(), hello_world.length(), 123, hash);
        CATCH_CHECK(SNAP_CATCH2_NAMESPACE::hex128(hash) == "61c9129e5a1aacd7a41621629e37c886");

        //TESTHASH(x86, 128, 321, "Hello, world!", "d5fbdcb3c26c4193045880c5a7170f0f");
        MurmurHash3_x86_128(hello_world.c_str(), hello_world.length(), 321, hash);
        CATCH_CHECK(SNAP_CATCH2_NAMESPACE::hex128(hash) == "d5fbdcb3c26c4193045880c5a7170f0f");

        //TESTHASH(x86, 128, 123, "xxxxxxxxxxxxxxxxxxxxxxxxxxxx", "5e40bab278825a164cf929d31fec6047");
        MurmurHash3_x86_128(x28.c_str(), x28.length(), 123, hash);
        CATCH_CHECK(SNAP_CATCH2_NAMESPACE::hex128(hash) == "5e40bab278825a164cf929d31fec6047");

        //TESTHASH(x86, 128, 123, "", "fedc524526f3e79926f3e79926f3e799");
        MurmurHash3_x86_128(empty.c_str(), empty.length(), 123, hash);
        CATCH_CHECK(SNAP_CATCH2_NAMESPACE::hex128(hash) == "fedc524526f3e79926f3e79926f3e799");

        //TESTHASH(x64, 128, 123, "Hello, world!", "8743acad421c8c73d373c3f5f19732fd");
        MurmurHash3_x64_128(hello_world.c_str(), hello_world.length(), 123, hash);
        CATCH_CHECK(SNAP_CATCH2_NAMESPACE::hex128(hash) == "8743acad421c8c73d373c3f5f19732fd");

        //TESTHASH(x64, 128, 321, "Hello, world!", "f86d4004ca47f42bb9546c7979200aee");
        MurmurHash3_x64_128(hello_world.c_str(), hello_world.length(), 321, hash);
        CATCH_CHECK(SNAP_CATCH2_NAMESPACE::hex128(hash) == "f86d4004ca47f42bb9546c7979200aee");

        //TESTHASH(x64, 128, 123, "xxxxxxxxxxxxxxxxxxxxxxxxxxxx", "becf7e04dbcf74637751664ef66e73e0");
        MurmurHash3_x64_128(x28.c_str(), x28.length(), 123, hash);
        CATCH_CHECK(SNAP_CATCH2_NAMESPACE::hex128(hash) == "becf7e04dbcf74637751664ef66e73e0");

        //TESTHASH(x64, 128, 123, "", "4cd9597081679d1abd92f8784bace33d");
        MurmurHash3_x64_128(empty.c_str(), empty.length(), 123, hash);
        CATCH_CHECK(SNAP_CATCH2_NAMESPACE::hex128(hash) == "4cd9597081679d1abd92f8784bace33d");
    }
    CATCH_END_SECTION()
}


// vim: ts=4 sw=4 et
