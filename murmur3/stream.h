// This extension was added by Alexis Wilke to support a streaming version.

#pragma once

// C++
//
#include    <string>



namespace murmur3
{

// total number of bytes in a `std::uint8_t hash[]` buffer
//
constexpr std::size_t   HASH_SIZE = sizeof(std::uint64_t) * 2;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
typedef unsigned __int128       hash_t;
#pragma GCC diagnostic pop


class murmur3_stream
{
public:
                        murmur3_stream();
                        murmur3_stream(std::uint64_t seed);
                        murmur3_stream(std::uint64_t seed1, std::uint64_t seed2);

    void                reset();
    void                reset(std::uint64_t seed);
    void                reset(std::uint64_t seed1, std::uint64_t seed2);

    void                get_seeds(std::uint64_t & a, std::uint64_t & b) const;
    void                add_data(void const * data, std::size_t size);

    void                flush(std::uint8_t * hash);
    hash_t              flush();
    void                flush(std::string & hash);

private:
    void                read_block(
                              const uint8_t * & data
                            , std::size_t & size
                            , std::uint64_t & k1
                            , std::uint64_t & k2);
    std::uint64_t       rotl64(std::uint64_t x, int r);
    std::uint64_t       fmix64(std::uint64_t k);

    bool                f_flushed = false;  // if true, the stream is "closed"
    std::size_t         f_data_size = 0;    // number of bytes in data[]
    std::uint8_t        f_data[16] = {};    // if the input buffer was not a multiple of 16
    std::uint64_t       f_seed1 = 0;
    std::uint64_t       f_seed2 = 0;
    std::uint64_t       f_h1 = 0;
    std::uint64_t       f_h2 = 0;
    std::uint64_t       f_total_size = 0;
};

} // namespace murmur3
// vim: ts=4 sw=4 et
