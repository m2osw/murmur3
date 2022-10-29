

// self
//
#include    "stream.h"



// snapdev
//
#include    <snapdev/hexadecimal_string.h>


// C++
//
#include    <exception>


// C
//
#include    <string.h>
#include    <sys/random.h>



namespace murmur3
{

constexpr std::uint64_t const g_c1(0x87c37b91114253d5LLU);
constexpr std::uint64_t const g_c2(0x4cf5ad432745937fLLU);


murmur3_stream::murmur3_stream()
{
    char random_data[sizeof(f_seed1) + sizeof(f_seed2)];
    getrandom(random_data, sizeof(random_data), 0);

    memcpy(&f_seed1, random_data, sizeof(f_seed1));
    memcpy(&f_seed2, random_data + sizeof(f_seed1), sizeof(f_seed2));

    f_h1 = f_seed1;
    f_h2 = f_seed2;
}

murmur3_stream::murmur3_stream(std::uint64_t seed)
{
    reset(seed);
}


murmur3_stream::murmur3_stream(std::uint64_t seed1, std::uint64_t seed2)
{
    reset(seed1, seed2);
}


void murmur3_stream::reset()
{
    f_h1 = f_seed1;
    f_h2 = f_seed2;
}


void murmur3_stream::reset(std::uint64_t seed)
{
    f_seed1 = seed;
    f_seed2 = seed;

    f_h1 = f_seed1;
    f_h2 = f_seed2;
}


void murmur3_stream::reset(std::uint64_t seed1, std::uint64_t seed2)
{
    f_seed1 = seed1;
    f_seed2 = seed2;

    f_h1 = f_seed1;
    f_h2 = f_seed2;
}


void murmur3_stream::get_seeds(std::uint64_t & a, std::uint64_t & b) const
{
    a = f_seed1;
    b = f_seed2;
}


void murmur3_stream::add_data(void const * data, std::size_t size)
{
    if(f_flushed)
    {
        throw std::runtime_error("called add_data() after flush()");
    }

    // enough data?
    if(f_data_size + size < 16)
    {
        memcpy(f_data + f_data_size, data, size);
        f_data_size += size;
        return;
    }

    f_total_size += size;

    std::uint8_t const * d(static_cast<std::uint8_t const *>(data));

    while(f_data_size + size >= 16)
    {
        std::uint64_t k1, k2;
        read_block(d, size, k1, k2);

        k1 *= g_c1;
        k1  = rotl64(k1, 31);
        k1 *= g_c2;
        f_h1 ^= k1;

        f_h1  = rotl64(f_h1, 27);
        f_h1 += f_h2;
        f_h1  = f_h1 * 5 + 0x52dce729;

        k2 *= g_c2;
        k2  = rotl64(k2, 33);
        k2 *= g_c1;
        f_h2 ^= k2;

        f_h2  = rotl64(f_h2, 31);
        f_h2 += f_h1;
        f_h2  = f_h2 * 5 + 0x38495ab5;
    }

    if(size > 0)
    {
        f_data_size = size;
        memcpy(f_data, d, size);
    }
}


void murmur3_stream::flush(std::uint8_t * hash)
{
    if(!f_flushed)
    {
        f_flushed = true;

        std::uint64_t k1(0);
        std::uint64_t k2(0);

        switch(f_data_size)
        {
        case 15:
            k2 ^= static_cast<std::uint64_t>(f_data[14]) << 48;
            [[fallthrough]];
        case 14:
            k2 ^= static_cast<std::uint64_t>(f_data[13]) << 40;
            [[fallthrough]];
        case 13:
            k2 ^= static_cast<std::uint64_t>(f_data[12]) << 32;
            [[fallthrough]];
        case 12:
            k2 ^= static_cast<std::uint64_t>(f_data[11]) << 24;
            [[fallthrough]];
        case 11:
            k2 ^= static_cast<std::uint64_t>(f_data[10]) << 16;
            [[fallthrough]];
        case 10:
            k2 ^= static_cast<std::uint64_t>(f_data[ 9]) << 8;
            [[fallthrough]];
        case  9:
            k2 ^= static_cast<std::uint64_t>(f_data[ 8]) << 0;

            k2 *= g_c2;
            k2  = rotl64(k2, 33);
            k2 *= g_c1;
            f_h2 ^= k2;

            [[fallthrough]];
        case  8:
            k1 ^= static_cast<std::uint64_t>(f_data[ 7]) << 56;
            [[fallthrough]];
        case  7:
            k1 ^= static_cast<std::uint64_t>(f_data[ 6]) << 48;
            [[fallthrough]];
        case  6:
            k1 ^= static_cast<std::uint64_t>(f_data[ 5]) << 40;
            [[fallthrough]];
        case  5:
            k1 ^= static_cast<std::uint64_t>(f_data[ 4]) << 32;
            [[fallthrough]];
        case  4:
            k1 ^= static_cast<std::uint64_t>(f_data[ 3]) << 24;
            [[fallthrough]];
        case  3:
            k1 ^= static_cast<std::uint64_t>(f_data[ 2]) << 16;
            [[fallthrough]];
        case  2:
            k1 ^= static_cast<std::uint64_t>(f_data[ 1]) << 8;
            [[fallthrough]];
        case  1:
            k1 ^= static_cast<std::uint64_t>(f_data[ 0]) << 0;

            k1 *= g_c1;
            k1  = rotl64(k1, 31);
            k1 *= g_c2;
            f_h1 ^= k1;
            break;

        case 0:
            break;

        default:
            throw std::runtime_error("f_data_size is "
                + std::to_string(f_data_size)
                + ", which is not a number from 0 to 15 inclusive.");

        }

        f_data_size = 0;

        f_h1 ^= f_total_size;
        f_h2 ^= f_total_size;

        f_h1 += f_h2;
        f_h2 += f_h1;

        f_h1 = fmix64(f_h1);
        f_h2 = fmix64(f_h2);

        f_h1 += f_h2;
        f_h2 += f_h1;
    }

    reinterpret_cast<std::uint64_t *>(hash)[0] = f_h1;
    reinterpret_cast<std::uint64_t *>(hash)[1] = f_h2;
}


hash_t murmur3_stream::flush()
{
    std::uint8_t hash[HASH_SIZE];
    flush(hash);
    return *reinterpret_cast<hash_t *>(hash);
}


void murmur3_stream::flush(std::string & hash)
{
    hash = snapdev::int_to_hex(flush(), false, 32);
}


void murmur3_stream::read_block(
      const uint8_t * & data
    , std::size_t & size
    , std::uint64_t & k1
    , std::uint64_t & k2)
{
    switch(f_data_size)
    {
    case 0:
        k1 = (static_cast<std::uint64_t>(  data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(  data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(  data[ 2]) << 16)
           | (static_cast<std::uint64_t>(  data[ 3]) << 24)
           | (static_cast<std::uint64_t>(  data[ 4]) << 32)
           | (static_cast<std::uint64_t>(  data[ 5]) << 40)
           | (static_cast<std::uint64_t>(  data[ 6]) << 48)
           | (static_cast<std::uint64_t>(  data[ 7]) << 56);
        k2 = (static_cast<std::uint64_t>(  data[ 8]) <<  0)
           | (static_cast<std::uint64_t>(  data[ 9]) <<  8)
           | (static_cast<std::uint64_t>(  data[10]) << 16)
           | (static_cast<std::uint64_t>(  data[11]) << 24)
           | (static_cast<std::uint64_t>(  data[12]) << 32)
           | (static_cast<std::uint64_t>(  data[13]) << 40)
           | (static_cast<std::uint64_t>(  data[14]) << 48)
           | (static_cast<std::uint64_t>(  data[15]) << 56);
        data += 16;
        size -= 16;
        break;

    case 1:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(  data[ 0]) <<  8)
           | (static_cast<std::uint64_t>(  data[ 1]) << 16)
           | (static_cast<std::uint64_t>(  data[ 2]) << 24)
           | (static_cast<std::uint64_t>(  data[ 3]) << 32)
           | (static_cast<std::uint64_t>(  data[ 4]) << 40)
           | (static_cast<std::uint64_t>(  data[ 5]) << 48)
           | (static_cast<std::uint64_t>(  data[ 6]) << 56);
        k2 = (static_cast<std::uint64_t>(  data[ 7]) <<  0)
           | (static_cast<std::uint64_t>(  data[ 8]) <<  8)
           | (static_cast<std::uint64_t>(  data[ 9]) << 16)
           | (static_cast<std::uint64_t>(  data[10]) << 24)
           | (static_cast<std::uint64_t>(  data[11]) << 32)
           | (static_cast<std::uint64_t>(  data[12]) << 40)
           | (static_cast<std::uint64_t>(  data[13]) << 48)
           | (static_cast<std::uint64_t>(  data[14]) << 56);
        data += 15;
        size -= 15;
        f_data_size = 0;
        break;

    case 2:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(  data[ 0]) << 16)
           | (static_cast<std::uint64_t>(  data[ 1]) << 24)
           | (static_cast<std::uint64_t>(  data[ 2]) << 32)
           | (static_cast<std::uint64_t>(  data[ 3]) << 40)
           | (static_cast<std::uint64_t>(  data[ 4]) << 48)
           | (static_cast<std::uint64_t>(  data[ 5]) << 56);
        k2 = (static_cast<std::uint64_t>(  data[ 6]) <<  0)
           | (static_cast<std::uint64_t>(  data[ 7]) <<  8)
           | (static_cast<std::uint64_t>(  data[ 8]) << 16)
           | (static_cast<std::uint64_t>(  data[ 9]) << 24)
           | (static_cast<std::uint64_t>(  data[10]) << 32)
           | (static_cast<std::uint64_t>(  data[11]) << 40)
           | (static_cast<std::uint64_t>(  data[12]) << 48)
           | (static_cast<std::uint64_t>(  data[13]) << 56);
        data += 14;
        size -= 14;
        f_data_size = 0;
        break;

    case 3:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(f_data[ 2]) << 16)
           | (static_cast<std::uint64_t>(  data[ 0]) << 24)
           | (static_cast<std::uint64_t>(  data[ 1]) << 32)
           | (static_cast<std::uint64_t>(  data[ 2]) << 40)
           | (static_cast<std::uint64_t>(  data[ 3]) << 48)
           | (static_cast<std::uint64_t>(  data[ 4]) << 56);
        k2 = (static_cast<std::uint64_t>(  data[ 5]) <<  0)
           | (static_cast<std::uint64_t>(  data[ 6]) <<  8)
           | (static_cast<std::uint64_t>(  data[ 7]) << 16)
           | (static_cast<std::uint64_t>(  data[ 8]) << 24)
           | (static_cast<std::uint64_t>(  data[ 9]) << 32)
           | (static_cast<std::uint64_t>(  data[10]) << 40)
           | (static_cast<std::uint64_t>(  data[11]) << 48)
           | (static_cast<std::uint64_t>(  data[12]) << 56);
        data += 13;
        size -= 13;
        f_data_size = 0;
        break;

    case 4:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(f_data[ 2]) << 16)
           | (static_cast<std::uint64_t>(f_data[ 3]) << 24)
           | (static_cast<std::uint64_t>(  data[ 0]) << 32)
           | (static_cast<std::uint64_t>(  data[ 1]) << 40)
           | (static_cast<std::uint64_t>(  data[ 2]) << 48)
           | (static_cast<std::uint64_t>(  data[ 3]) << 56);
        k2 = (static_cast<std::uint64_t>(  data[ 4]) <<  0)
           | (static_cast<std::uint64_t>(  data[ 5]) <<  8)
           | (static_cast<std::uint64_t>(  data[ 6]) << 16)
           | (static_cast<std::uint64_t>(  data[ 7]) << 24)
           | (static_cast<std::uint64_t>(  data[ 8]) << 32)
           | (static_cast<std::uint64_t>(  data[ 9]) << 40)
           | (static_cast<std::uint64_t>(  data[10]) << 48)
           | (static_cast<std::uint64_t>(  data[11]) << 56);
        data += 12;
        size -= 12;
        f_data_size = 0;
        break;

    case 5:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(f_data[ 2]) << 16)
           | (static_cast<std::uint64_t>(f_data[ 3]) << 24)
           | (static_cast<std::uint64_t>(f_data[ 4]) << 32)
           | (static_cast<std::uint64_t>(  data[ 0]) << 40)
           | (static_cast<std::uint64_t>(  data[ 1]) << 48)
           | (static_cast<std::uint64_t>(  data[ 2]) << 56);
        k2 = (static_cast<std::uint64_t>(  data[ 3]) <<  0)
           | (static_cast<std::uint64_t>(  data[ 4]) <<  8)
           | (static_cast<std::uint64_t>(  data[ 5]) << 16)
           | (static_cast<std::uint64_t>(  data[ 6]) << 24)
           | (static_cast<std::uint64_t>(  data[ 7]) << 32)
           | (static_cast<std::uint64_t>(  data[ 8]) << 40)
           | (static_cast<std::uint64_t>(  data[ 9]) << 48)
           | (static_cast<std::uint64_t>(  data[10]) << 56);
        data += 11;
        size -= 11;
        f_data_size = 0;
        break;

    case 6:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(f_data[ 2]) << 16)
           | (static_cast<std::uint64_t>(f_data[ 3]) << 24)
           | (static_cast<std::uint64_t>(f_data[ 4]) << 32)
           | (static_cast<std::uint64_t>(f_data[ 5]) << 40)
           | (static_cast<std::uint64_t>(  data[ 0]) << 48)
           | (static_cast<std::uint64_t>(  data[ 1]) << 56);
        k2 = (static_cast<std::uint64_t>(  data[ 2]) <<  0)
           | (static_cast<std::uint64_t>(  data[ 3]) <<  8)
           | (static_cast<std::uint64_t>(  data[ 4]) << 16)
           | (static_cast<std::uint64_t>(  data[ 5]) << 24)
           | (static_cast<std::uint64_t>(  data[ 6]) << 32)
           | (static_cast<std::uint64_t>(  data[ 7]) << 40)
           | (static_cast<std::uint64_t>(  data[ 8]) << 48)
           | (static_cast<std::uint64_t>(  data[ 9]) << 56);
        data += 10;
        size -= 10;
        f_data_size = 0;
        break;

    case 7:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(f_data[ 2]) << 16)
           | (static_cast<std::uint64_t>(f_data[ 3]) << 24)
           | (static_cast<std::uint64_t>(f_data[ 4]) << 32)
           | (static_cast<std::uint64_t>(f_data[ 5]) << 40)
           | (static_cast<std::uint64_t>(f_data[ 6]) << 48)
           | (static_cast<std::uint64_t>(  data[ 0]) << 56);
        k2 = (static_cast<std::uint64_t>(  data[ 1]) <<  0)
           | (static_cast<std::uint64_t>(  data[ 2]) <<  8)
           | (static_cast<std::uint64_t>(  data[ 3]) << 16)
           | (static_cast<std::uint64_t>(  data[ 4]) << 24)
           | (static_cast<std::uint64_t>(  data[ 5]) << 32)
           | (static_cast<std::uint64_t>(  data[ 6]) << 40)
           | (static_cast<std::uint64_t>(  data[ 7]) << 48)
           | (static_cast<std::uint64_t>(  data[ 8]) << 56);
        data += 9;
        size -= 9;
        f_data_size = 0;
        break;

    case 8:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(f_data[ 2]) << 16)
           | (static_cast<std::uint64_t>(f_data[ 3]) << 24)
           | (static_cast<std::uint64_t>(f_data[ 4]) << 32)
           | (static_cast<std::uint64_t>(f_data[ 5]) << 40)
           | (static_cast<std::uint64_t>(f_data[ 6]) << 48)
           | (static_cast<std::uint64_t>(f_data[ 7]) << 56);
        k2 = (static_cast<std::uint64_t>(  data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(  data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(  data[ 2]) << 16)
           | (static_cast<std::uint64_t>(  data[ 3]) << 24)
           | (static_cast<std::uint64_t>(  data[ 4]) << 32)
           | (static_cast<std::uint64_t>(  data[ 5]) << 40)
           | (static_cast<std::uint64_t>(  data[ 6]) << 48)
           | (static_cast<std::uint64_t>(  data[ 7]) << 56);
        data += 8;
        size -= 8;
        f_data_size = 0;
        break;

    case 9:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(f_data[ 2]) << 16)
           | (static_cast<std::uint64_t>(f_data[ 3]) << 24)
           | (static_cast<std::uint64_t>(f_data[ 4]) << 32)
           | (static_cast<std::uint64_t>(f_data[ 5]) << 40)
           | (static_cast<std::uint64_t>(f_data[ 6]) << 48)
           | (static_cast<std::uint64_t>(f_data[ 7]) << 56);
        k2 = (static_cast<std::uint64_t>(f_data[ 8]) <<  0)
           | (static_cast<std::uint64_t>(  data[ 0]) <<  8)
           | (static_cast<std::uint64_t>(  data[ 1]) << 16)
           | (static_cast<std::uint64_t>(  data[ 2]) << 24)
           | (static_cast<std::uint64_t>(  data[ 3]) << 32)
           | (static_cast<std::uint64_t>(  data[ 4]) << 40)
           | (static_cast<std::uint64_t>(  data[ 5]) << 48)
           | (static_cast<std::uint64_t>(  data[ 6]) << 56);
        data += 7;
        size -= 7;
        f_data_size = 0;
        break;

    case 10:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(f_data[ 2]) << 16)
           | (static_cast<std::uint64_t>(f_data[ 3]) << 24)
           | (static_cast<std::uint64_t>(f_data[ 4]) << 32)
           | (static_cast<std::uint64_t>(f_data[ 5]) << 40)
           | (static_cast<std::uint64_t>(f_data[ 6]) << 48)
           | (static_cast<std::uint64_t>(f_data[ 7]) << 56);
        k2 = (static_cast<std::uint64_t>(f_data[ 8]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 9]) <<  8)
           | (static_cast<std::uint64_t>(  data[ 0]) << 16)
           | (static_cast<std::uint64_t>(  data[ 1]) << 24)
           | (static_cast<std::uint64_t>(  data[ 2]) << 32)
           | (static_cast<std::uint64_t>(  data[ 3]) << 40)
           | (static_cast<std::uint64_t>(  data[ 4]) << 48)
           | (static_cast<std::uint64_t>(  data[ 5]) << 56);
        data += 6;
        size -= 6;
        f_data_size = 0;
        break;

    case 11:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(f_data[ 2]) << 16)
           | (static_cast<std::uint64_t>(f_data[ 3]) << 24)
           | (static_cast<std::uint64_t>(f_data[ 4]) << 32)
           | (static_cast<std::uint64_t>(f_data[ 5]) << 40)
           | (static_cast<std::uint64_t>(f_data[ 6]) << 48)
           | (static_cast<std::uint64_t>(f_data[ 7]) << 56);
        k2 = (static_cast<std::uint64_t>(f_data[ 8]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 9]) <<  8)
           | (static_cast<std::uint64_t>(f_data[10]) << 16)
           | (static_cast<std::uint64_t>(  data[ 0]) << 24)
           | (static_cast<std::uint64_t>(  data[ 1]) << 32)
           | (static_cast<std::uint64_t>(  data[ 2]) << 40)
           | (static_cast<std::uint64_t>(  data[ 3]) << 48)
           | (static_cast<std::uint64_t>(  data[ 4]) << 56);
        data += 5;
        size -= 5;
        f_data_size = 0;
        break;

    case 12:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(f_data[ 2]) << 16)
           | (static_cast<std::uint64_t>(f_data[ 3]) << 24)
           | (static_cast<std::uint64_t>(f_data[ 4]) << 32)
           | (static_cast<std::uint64_t>(f_data[ 5]) << 40)
           | (static_cast<std::uint64_t>(f_data[ 6]) << 48)
           | (static_cast<std::uint64_t>(f_data[ 7]) << 56);
        k2 = (static_cast<std::uint64_t>(f_data[ 8]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 9]) <<  8)
           | (static_cast<std::uint64_t>(f_data[10]) << 16)
           | (static_cast<std::uint64_t>(f_data[11]) << 24)
           | (static_cast<std::uint64_t>(  data[ 0]) << 32)
           | (static_cast<std::uint64_t>(  data[ 1]) << 40)
           | (static_cast<std::uint64_t>(  data[ 2]) << 48)
           | (static_cast<std::uint64_t>(  data[ 3]) << 56);
        data += 4;
        size -= 4;
        f_data_size = 0;
        break;

    case 13:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(f_data[ 2]) << 16)
           | (static_cast<std::uint64_t>(f_data[ 3]) << 24)
           | (static_cast<std::uint64_t>(f_data[ 4]) << 32)
           | (static_cast<std::uint64_t>(f_data[ 5]) << 40)
           | (static_cast<std::uint64_t>(f_data[ 6]) << 48)
           | (static_cast<std::uint64_t>(f_data[ 7]) << 56);
        k2 = (static_cast<std::uint64_t>(f_data[ 8]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 9]) <<  8)
           | (static_cast<std::uint64_t>(f_data[10]) << 16)
           | (static_cast<std::uint64_t>(f_data[11]) << 24)
           | (static_cast<std::uint64_t>(f_data[12]) << 32)
           | (static_cast<std::uint64_t>(  data[ 0]) << 40)
           | (static_cast<std::uint64_t>(  data[ 1]) << 48)
           | (static_cast<std::uint64_t>(  data[ 2]) << 56);
        data += 3;
        size -= 3;
        f_data_size = 0;
        break;

    case 14:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(f_data[ 2]) << 16)
           | (static_cast<std::uint64_t>(f_data[ 3]) << 24)
           | (static_cast<std::uint64_t>(f_data[ 4]) << 32)
           | (static_cast<std::uint64_t>(f_data[ 5]) << 40)
           | (static_cast<std::uint64_t>(f_data[ 6]) << 48)
           | (static_cast<std::uint64_t>(f_data[ 7]) << 56);
        k2 = (static_cast<std::uint64_t>(f_data[ 8]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 9]) <<  8)
           | (static_cast<std::uint64_t>(f_data[10]) << 16)
           | (static_cast<std::uint64_t>(f_data[11]) << 24)
           | (static_cast<std::uint64_t>(f_data[12]) << 32)
           | (static_cast<std::uint64_t>(f_data[13]) << 40)
           | (static_cast<std::uint64_t>(  data[ 0]) << 48)
           | (static_cast<std::uint64_t>(  data[ 1]) << 56);
        data += 2;
        size -= 2;
        f_data_size = 0;
        break;

    case 15:
        k1 = (static_cast<std::uint64_t>(f_data[ 0]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 1]) <<  8)
           | (static_cast<std::uint64_t>(f_data[ 2]) << 16)
           | (static_cast<std::uint64_t>(f_data[ 3]) << 24)
           | (static_cast<std::uint64_t>(f_data[ 4]) << 32)
           | (static_cast<std::uint64_t>(f_data[ 5]) << 40)
           | (static_cast<std::uint64_t>(f_data[ 6]) << 48)
           | (static_cast<std::uint64_t>(f_data[ 7]) << 56);
        k2 = (static_cast<std::uint64_t>(f_data[ 8]) <<  0)
           | (static_cast<std::uint64_t>(f_data[ 9]) <<  8)
           | (static_cast<std::uint64_t>(f_data[10]) << 16)
           | (static_cast<std::uint64_t>(f_data[11]) << 24)
           | (static_cast<std::uint64_t>(f_data[12]) << 32)
           | (static_cast<std::uint64_t>(f_data[13]) << 40)
           | (static_cast<std::uint64_t>(f_data[14]) << 48)
           | (static_cast<std::uint64_t>(  data[ 0]) << 56);
        data += 1;
        size -= 1;
        f_data_size = 0;
        break;

    default:
        throw std::runtime_error("somehow the f_data_size field is "
            + std::to_string(f_data_size)
            + " which not a number between 0 and 15 inclusive.");

    }
}


std::uint64_t murmur3_stream::rotl64(std::uint64_t x, int r)
{
    return (x << r) | (x >> (64 - r));
}


std::uint64_t murmur3_stream::fmix64(std::uint64_t k)
{
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccdLLU;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53LLU;
    k ^= k >> 33;

    return k;
}



} // namespace murmur3
// vim: ts=4 sw=4 et
