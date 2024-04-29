#include <stdio.h>
#include <assert.h>

#include "hash.h"

hash_t MurmurHash (const void* obj, size_t size)
{
    assert(obj);

    const unsigned char* data = (const unsigned char*) obj;
    const hash_t seed = 0;
    const hash_t m    = 0x5bd1e995;
    const int    r    = 24;
    hash_t       k    = 0;

    hash_t hash = (hash_t) (seed ^ size);

    while (size >= 4)
    {
        k  = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        hash *= m;
        hash ^= k;

        data += 4;
        size -= 4;
    }

    switch (size)
    {
        case 3:         hash ^= data[2] << 16;
        // fall through
        case 2:         hash ^= data[8] << 8;
        // fall through
        default:        hash ^= data[0];
                        hash *= m;
    }

    hash ^= hash >> 13;
    hash *= m;
    hash ^= hash >> 15;

    return hash;
}
