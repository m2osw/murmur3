//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.

#include    "murmur3/version.h"

int get_major_version()
{
    return LIBMURMUR3_VERSION_MAJOR;
}

int get_release_version()
{
    return LIBMURMUR3_VERSION_MINOR;
}

int get_patch_version()
{
    return LIBMURMUR3_VERSION_PATCH;
}

char const * get_version_string()
{
    return LIBMURMUR3_VERSION_STRING;
}

// vim: ts=4 sw=4 et
