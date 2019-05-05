/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_hash.h>

#include <misc/tt_util.h>
#include <os/tt_thread.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_hashctx_init(IN tt_hashctx_t *hctx)
{
    TT_ASSERT(hctx != NULL);

    if (tt_current_thread() != NULL) {
        hctx->seed = tt_rand_u32();
    } else {
        hctx->seed = (tt_u32_t)tt_c_rand();
    }
}

tt_hashcode_t tt_hash_murmur3(IN const tt_u8_t *key, IN tt_u32_t key_len,
                              IN tt_hashctx_t *hctx)
{
    static const tt_u32_t c1 = 0xcc9e2d51;
    static const tt_u32_t c2 = 0x1b873593;
    static const tt_u32_t r1 = 15;
    static const tt_u32_t r2 = 13;
    static const tt_u32_t m = 5;
    static const tt_u32_t n = 0xe6546b64;
    // accessing these static const vars should have been optimized by
    // compiler as constants

    const int nblocks = key_len >> 2;
    const tt_u32_t *blocks = (const tt_u32_t *)key;
    int i;

    const tt_u8_t *tail = (const tt_u8_t *)(key + (nblocks << 2));
    tt_u32_t k1 = 0;

    tt_u32_t hash = hctx->seed;
    // tt_u32_t hash = (tt_u32_t)((tt_uintptr_t)key);

    for (i = 0; i < nblocks; i++) {
        tt_u32_t k = blocks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
    }

    switch (key_len & 3) {
    case 3: k1 ^= tail[2] << 16;
    case 2: k1 ^= tail[1] << 8;
    case 1:
        k1 ^= tail[0];

        k1 *= c1;
        k1 = (k1 << r1) | (k1 >> (32 - r1));
        k1 *= c2;
        hash ^= k1;
    }

    hash ^= key_len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}

tt_hashcode_t tt_hash_fnv1a(IN const tt_u8_t *key, IN tt_u32_t key_len,
                            IN tt_hashctx_t *hctx)
{
    tt_u8_t *bp = (tt_u8_t *)key;
    tt_u8_t *be = TT_PTR_INC(tt_u8_t, key, key_len);
    tt_hashcode_t hval = hctx->seed;

    while (bp < be) {
        hval ^= (tt_u32_t)(*bp++);
        hval += (hval << 1) + (hval << 4) + (hval << 7) + (hval << 8) +
                (hval << 24);
    }

    // fnv1a requires passing hval to next call, but woule this generate
    // different hash value from two same inputs?
    return hval;
}
