/* Licensed to the Apache Software Foundation (ASF) under one or more
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

#include <memory/tt_memory_spring.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

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

void tt_memspg_init(IN tt_memspg_t *mspg,
                    IN tt_u32_t min_extend,
                    IN tt_u32_t max_extend,
                    IN tt_u32_t max_limit)
{
    TT_ASSERT(min_extend > 0);
    TT_ASSERT(min_extend < max_extend);
    TT_ASSERT((max_limit == 0) || (max_extend < max_limit));

    mspg->min_extend = min_extend;
    mspg->max_extend = max_extend;
    mspg->max_limit = max_limit;
}

tt_result_t tt_memspg_extend_ex(IN tt_memspg_t *mspg,
                                IN OUT tt_u8_t **p,
                                IN OUT tt_u32_t *size,
                                IN tt_u32_t to_size,
                                IN tt_u32_t flag)
{
    tt_u32_t new_size;
    tt_u8_t *new_p;

    TT_ASSERT_ALWAYS(to_size > *size);

    new_size = *size;
    do {
        new_size = tt_memspg_next_size(mspg, new_size);
        if (new_size == 0) {
            return TT_FAIL;
        }
        TT_ASSERT(new_size > *size);
    } while (new_size < to_size);

    new_p = tt_malloc(new_size);
    if (new_p == NULL) {
        TT_ERROR("no mem to extend");
        return TT_FAIL;
    }

    if (*p != NULL) {
        tt_memcpy(new_p, *p, *size);

        if (flag & TT_MSPGEXT_ZERO) {
            tt_memset(TT_PTR_INC(void, new_p, *size), 0, new_size - *size);
        }

        if (!(flag & TT_MSPGEXT_NOFREE)) {
            tt_free(*p);
        }
    }
    *p = new_p;
    *size = new_size;

    return TT_SUCCESS;
}

tt_result_t tt_memspg_compress(IN tt_memspg_t *mspg,
                               IN OUT tt_u8_t **p,
                               IN OUT tt_u32_t *size,
                               IN tt_u32_t to_size)
{
    tt_u8_t *new_p = NULL;

    TT_ASSERT(*p != NULL);
    TT_ASSERT_ALWAYS(to_size < *size);

    if (to_size != 0) {
        new_p = tt_malloc(to_size);
        if (new_p == NULL) {
            TT_ERROR("no mem to compress");
            return TT_FAIL;
        }
        tt_memcpy(new_p, *p, to_size);
    }

    tt_free(*p);
    *p = new_p;
    *size = to_size;

    return TT_SUCCESS;
}

tt_result_t tt_memspg_compress_range(IN tt_memspg_t *mspg,
                                     IN OUT tt_u8_t **p,
                                     IN OUT tt_u32_t *size,
                                     IN tt_u32_t from,
                                     IN tt_u32_t to)
{
    tt_u8_t *new_p = NULL;
    tt_u32_t new_size = to - from;

    TT_ASSERT(*p != NULL);
    TT_ASSERT_ALWAYS(from <= to);
    TT_ASSERT_ALWAYS(to <= *size);

    if (new_size != 0) {
        new_p = tt_malloc(new_size);
        if (new_p == NULL) {
            TT_ERROR("no mem to compress");
            return TT_FAIL;
        }
        tt_memcpy(new_p, *p + from, new_size);
    }

    tt_free(*p);
    *p = new_p;
    *size = new_size;

    return TT_SUCCESS;
}

tt_u32_t tt_memspg_next_size(IN tt_memspg_t *mspg, IN tt_u32_t size)
{
    tt_u32_t next_size;

    // set to min_extend if less than min_extend
    if (size < mspg->min_extend) {
        return mspg->min_extend;
    }

    // extend twice if less than max_extend
    if (size <= mspg->max_extend) {
        // as size <= mspg->max_extend <= (1 << 30), so it won't overflow:
        // (size << 1) <= (1 << 31)
        return (size << 1);
    }

    // plus max_extend if larger than max_extend
    next_size = (size + mspg->max_extend);
    if (next_size < size) {
        TT_ERROR("overflowed: %u", size);
        return 0;
    }
    if ((mspg->max_limit == 0) || (next_size <= mspg->max_limit)) {
        return next_size;
    }

    TT_ERROR("too large size: %u", size);
    return 0;
}
