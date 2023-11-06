// This extension was added by Alexis Wilke to support a streaming version.

#pragma once

// C++
//
#include    <cstdint>
#include    <string>



namespace murmur3
{

typedef std::uint64_t           seed_t;

// total number of bytes in a `std::uint8_t hash[]` buffer
//
constexpr std::size_t   HASH_SIZE = sizeof(seed_t) * 2;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
typedef unsigned __int128       hash_t;
#pragma GCC diagnostic pop


constexpr seed_t const          FFMPEG_DEFAULT_SEED = 0x725acc55daddca55LLU;


class hash
{
public:
                            hash(seed_t h1 = 0, seed_t h2 = 0);

    void                    set(std::uint8_t const * h);
    std::uint8_t const *    get() const;

    hash_t                  to_uint128() const;
    std::string             to_string() const;
    void                    from_string(std::string const & in);

    bool                    operator == (hash const & rhs) const;
    bool                    operator != (hash const & rhs) const;
    bool                    operator <  (hash const & rhs) const;
    bool                    operator <= (hash const & rhs) const;
    bool                    operator >  (hash const & rhs) const;
    bool                    operator >= (hash const & rhs) const;

private:
    std::uint8_t            f_hash[HASH_SIZE] = {};
};



class stream
{
public:
                        stream();
                        stream(seed_t seed);
                        stream(seed_t seed1, seed_t seed2);

    void                reset();
    void                reset(seed_t seed);
    void                reset(seed_t seed1, seed_t seed2);

    void                get_seeds(seed_t & a, seed_t & b) const;
    void                add_data(void const * data, std::size_t size);

    hash                flush() const;
    //void                flush(std::uint8_t * hash) const;
    //hash_t              flush() const;
    //void                flush(std::string & hash) const;

private:
    void                read_block(
                              std::uint8_t const * & data
                            , std::size_t & size
                            , std::uint64_t & k1
                            , std::uint64_t & k2);

    std::size_t         f_data_size = 0;    // number of bytes in f_data[]
    std::uint8_t        f_data[15] = {};    // if the input buffer was not a multiple of 16 save up to 15 bytes here
    seed_t              f_seed1 = 0;
    seed_t              f_seed2 = 0;
    seed_t              f_h1 = 0;
    seed_t              f_h2 = 0;
    seed_t              f_total_size = 0;
};


hash sum(void const * data, std::size_t size, seed_t seed = 0);
hash sum(void const * data, std::size_t size, seed_t seed1, seed_t seed2);
hash sum(std::string const & filename, seed_t seed = 0);
hash sum(std::string const & filename, seed_t seed1, seed_t seed2);


} // namespace murmur3
// vim: ts=4 sw=4 et
