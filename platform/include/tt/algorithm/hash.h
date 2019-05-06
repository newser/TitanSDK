/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
@file hash.h
@brief hash

*/

#ifndef __TT_HASH_CPP__
#define __TT_HASH_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/misc/util.h>

namespace tt {

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

enum hash_alg
{
    e_murmur3,
    e_fnv1a,
};

template<hash_alg t_alg = e_murmur3>
class hash
{
public:
    hash() = default;
    hash(uint32_t seed): seed_(seed) {}

    void seed(uint32_t seed) { seed_ = seed; }

    uint32_t calc(const void *addr, size_t size);

private:
    uint32_t seed_{0};

    TT_NON_COPYABLE(hash);
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

template<>
uint32_t hash<e_murmur3>::calc(const void *addr, size_t size)
{
    static constexpr uint32_t c1 = 0xcc9e2d51;
    static constexpr uint32_t c2 = 0x1b873593;
    static constexpr uint32_t r1 = 15;
    static constexpr uint32_t r2 = 13;
    static constexpr uint32_t m = 5;
    static constexpr uint32_t n = 0xe6546b64;
    // accessing these static const vars should have been optimized by
    // compiler as constants

    const int nblocks = size >> 2;
    const uint32_t *blocks = (const uint32_t *)addr;
    int i;

    const uint8_t *tail = (const uint8_t *)addr + (nblocks << 2);
    uint32_t k1 = 0;

    uint32_t hash = seed_;

    for (i = 0; i < nblocks; i++) {
        uint32_t k = blocks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
    }

    switch (size & 3) {
    case 3: k1 ^= tail[2] << 16;
    case 2: k1 ^= tail[1] << 8;
    case 1:
        k1 ^= tail[0];

        k1 *= c1;
        k1 = (k1 << r1) | (k1 >> (32 - r1));
        k1 *= c2;
        hash ^= k1;
    }

    hash ^= size;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}

template<>
uint32_t hash<e_fnv1a>::calc(const void *addr, size_t size)
{
    uint8_t *bp = (uint8_t *)addr;
    uint8_t *be = bp + size;
    uint32_t hval = seed_;

    while (bp < be) {
        hval ^= (uint32_t)(*bp++);
        hval += (hval << 1) + (hval << 4) + (hval << 7) + (hval << 8) +
                (hval << 24);
    }

    // fnv1a requires passing hval to next call, but woule this generate
    // different hash value from two same inputs?
    return hval;
}

}

#endif /* __TT_HASH_CPP__ */
