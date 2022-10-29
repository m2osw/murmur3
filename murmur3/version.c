//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.

#include    "murmur3/version.h"

int get_murmur3_major_version()
{
    return MURMUR3_VERSION_MAJOR;
}

int get_murmur3_release_version()
{
    return MURMUR3_VERSION_MINOR;
}

int get_murmur3_patch_version()
{
    return MURMUR3_VERSION_PATCH;
}

char const * get_murmur3_version_string()
{
    return MURMUR3_VERSION_STRING;
}

// vim: ts=4 sw=4 et
