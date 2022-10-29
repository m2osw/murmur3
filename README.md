Implementation of Murmur3
=========================

The original .c files come from
https://github.com/PeterScott/murmur3

The first implementation comes from
https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp

This project is compiled in
[a PPA](https://launchpad.net/~snapcpp/+archive/ubuntu/ppa/+packages)
so you can directly get the compiled version for your Ubuntu computer.
Maybe even on a Debian computer. Although note that there is are packages
named `libmurmur3` and `libmurmur3-dev` available in standard Ubuntu.
I do not know whether this is the same code or not.

Our library includes a C++ implementation which offers a streaming version
of the murmur3 implementation. The header also includes several helper
functions which one can use to directly transform a buffer or a file in
a murmur3 hash object. The hash object can be be transform in a `uint128_t`
or an `std::string` if you do not want to deal with a buffer of 16 chars.

Our package offers the `murmur3sum` tool, which can be used on the command
line very much like you use the `md5sum` tool.

---

The following comes from Peter Scott package:

C port of Murmur3 hash
======================

This is a port of the [Murmur3](http://code.google.com/p/smhasher/wiki/MurmurHash3) hash function. Murmur3 is a non-cryptographic hash, designed to be fast and excellent-quality for making things like hash tables or bloom filters. This is a port of the original C++ code, designed for Visual Studio, into standard C that gcc can compile efficiently.

How to use it
-------------

Just compile and link your program with `murmur3.c`, and be sure to include `murmur3.h` to get the function prototypes. There are three hash functions:

    void MurmurHash3_x86_32 (const void *key, int len, uint32_t seed, void *out);

    void MurmurHash3_x86_128(const void *key, int len, uint32_t seed, void *out);

    void MurmurHash3_x64_128(const void *key, int len, uint32_t seed, void *out);

All three of these functions have the same interface: you give them `key`, a pointer to the data you wish to hash; `len`, the length in bytes; `seed`, an arbitrary seed number which you can use to tweak the hash, and `out`, a pointer to a buffer big enough to hold the hash's output value.

The hash functions differ in both their internal mechanisms and in their outputs. They are specialized for different use cases:

**MurmurHash3_x86_32** has the lowest throughput, but also the lowest latency. If you're making a hash table that usually has small keys, this is probably the one you want to use on 32-bit machines. It has a 32-bit output.


**MurmurHash3_x86_128** is also designed for 32-bit systems, but produces a 128-bit output, and has about 30% higher throughput than the previous hash. Be warned, though, that its latency for a single 16-byte key is about 86% longer!

**MurmurHash3_x64_128** is the best of the lot, if you're using a 64-bit machine. Its throughput is 250% higher than `MurmurHash3_x86_32`, but it has roughly the same latency. It has a 128-bit output.

The hash functions are designed to work efficiently on x86 processors; in particular, they make some assumptions about the endianness of the processor, and about the speed of unaligned reads. If you have problems running this code on non-x86 architectures, it should be possible to modify it to work correctly and efficiently -- I just don't have access to those machines for testing. The code in `murmur3.c` is pretty straightforward, and shouldn't be too hard to alter.

There is an example program, `example.c`, which you can look at and play with. You can build it with the makefile.

License and contributing
------------------------

TODO: this is true of the original code, not all the code in this project.
All this code is in the public domain. Murmur3 was created by Austin Appleby, and the C port and general tidying up was done by Peter Scott. If you'd like to contribute something, I would love to add your name to this list.
