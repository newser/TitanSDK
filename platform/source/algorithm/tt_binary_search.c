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

#include <algorithm/tt_binary_search.h>

#include <misc/tt_assert.h>

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

void *__short_bsearch(IN void *key,
                      IN void *base,
                      IN tt_u32_t num,
                      IN tt_u32_t width,
                      IN tt_cmp_t cmp);

void *__short_bsearch_upper(IN void *key,
                            IN void *base,
                            IN tt_u32_t num,
                            IN tt_u32_t width,
                            IN tt_cmp_t cmp);

void *__short_bsearch_lower(IN void *key,
                            IN void *base,
                            IN tt_u32_t num,
                            IN tt_u32_t width,
                            IN tt_cmp_t cmp);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void *tt_bsearch(IN void *key,
                 IN void *base,
                 IN tt_u32_t num,
                 IN tt_u32_t width,
                 IN tt_cmp_t cmp)
{
    tt_u8_t *cur_start, *cur_end, *cur_middle;
    tt_u32_t cur_num;

    TT_ASSERT(key != NULL);
    TT_ASSERT(base != NULL);
    TT_ASSERT(num != 0);
    TT_ASSERT(width != 0);
    TT_ASSERT(cmp != NULL);

    cur_start = (tt_u8_t *)base;
    cur_end = cur_start + (num - 1) * width;
    cur_middle = NULL;

sub_bsearch:

    cur_num = (tt_u32_t)((cur_end - cur_start) / width + 1);

#define SHORT_BSEARCH_THRESHOLD 8

    if (cur_num <= SHORT_BSEARCH_THRESHOLD) {
        return __short_bsearch(key, base, num, width, cmp);
    } else {
        tt_s32_t comp_ret = 0;

        cur_middle = cur_start + (cur_num >> 1) * width;

        comp_ret = cmp(key, cur_middle);
        if (comp_ret < 0) {
            if (cur_start < cur_middle) {
                cur_end = cur_middle - width;
                goto sub_bsearch;
            } else {
                return NULL;
            }
        } else if (comp_ret > 0) {
            if (cur_middle < cur_end) {
                cur_start = cur_middle + width;
                goto sub_bsearch;
            } else {
                return NULL;
            }
        } else {
            return cur_middle;
        }
    }

#undef SHORT_BSEARCH_THRESHOLD
}

void *tt_bsearch_upper(IN void *key,
                       IN void *base,
                       IN tt_u32_t num,
                       IN tt_u32_t width,
                       IN tt_cmp_t cmp)
{
    tt_u8_t *cur_start, *cur_end, *cur_middle;
    tt_u32_t cur_num;

    TT_ASSERT(key != NULL);
    TT_ASSERT(base != NULL);
    TT_ASSERT(num != 0);
    TT_ASSERT(width != 0);
    TT_ASSERT(cmp != NULL);

    cur_start = (tt_u8_t *)base;
    cur_end = cur_start + (num - 1) * width;
    cur_middle = NULL;

sub_bsearch_min_larger:

    cur_num = (tt_u32_t)((cur_end - cur_start) / width + 1);

#define SHORT_BSEARCH_MIN_LARGER_THRESHOLD 8

    if (cur_num <= SHORT_BSEARCH_MIN_LARGER_THRESHOLD) {
        return __short_bsearch_upper(key, base, num, width, cmp);
    } else {
        tt_s32_t comp_ret = 0;

        cur_middle = cur_start + (cur_num >> 1) * width;

        comp_ret = cmp(key, cur_middle);
        if (comp_ret < 0) {
            if (cur_start < cur_middle) {
                cur_end = cur_middle - width;
                if (cmp(key, cur_end) > 0) {
                    return cur_middle;
                }

                goto sub_bsearch_min_larger;
            } else {
                return cur_middle;
            }
        } else if (comp_ret > 0) {
            if (cur_middle < cur_end) {
                cur_start = cur_middle + width;
                goto sub_bsearch_min_larger;
            } else {
                return NULL;
            }
        } else {
            return cur_middle;
        }
    }

#undef SHORT_BSEARCH_MIN_LARGER_THRESHOLD
}

void *tt_bsearch_lower(IN void *key,
                       IN void *base,
                       IN tt_u32_t num,
                       IN tt_u32_t width,
                       IN tt_cmp_t cmp)
{
    tt_u8_t *cur_start, *cur_end, *cur_middle;
    tt_u32_t cur_num;

    TT_ASSERT(key != NULL);
    TT_ASSERT(base != NULL);
    TT_ASSERT(num != 0);
    TT_ASSERT(width != 0);
    TT_ASSERT(cmp != NULL);

    cur_start = (tt_u8_t *)base;
    cur_end = cur_start + (num - 1) * width;
    cur_middle = NULL;

sub_bsearch_max_less:

    cur_num = (tt_u32_t)((cur_end - cur_start) / width + 1);

#define SHORT_BSEARCH_MAX_LESS_THRESHOLD 8

    if (cur_num <= SHORT_BSEARCH_MAX_LESS_THRESHOLD) {
        return __short_bsearch_lower(key, base, num, width, cmp);
    } else {
        tt_s32_t comp_ret = 0;

        cur_middle = cur_start + (cur_num >> 1) * width;

        comp_ret = cmp(key, cur_middle);
        if (comp_ret < 0) {
            if (cur_start < cur_middle) {
                cur_end = cur_middle - width;
                goto sub_bsearch_max_less;
            } else {
                return NULL;
            }
        } else if (comp_ret > 0) {
            if (cur_middle < cur_end) {
                cur_start = cur_middle + width;
                if (cmp(key, cur_start) < 0) {
                    return cur_middle;
                }
                goto sub_bsearch_max_less;
            } else {
                return cur_middle;
            }
        } else {
            return cur_middle;
        }
    }

#undef SHORT_BSEARCH_MAX_LESS_THRESHOLD
}

void *__short_bsearch(IN void *key,
                      IN void *base,
                      IN tt_u32_t num,
                      IN tt_u32_t width,
                      IN tt_cmp_t cmp)
{
    tt_u8_t *start, *end, *pos;

    TT_ASSERT(num > 0);

    start = (tt_u8_t *)base;
    end = start + (num - 1) * width;

    for (pos = start; pos <= end; pos += width) {
        if (cmp(key, pos) == 0) {
            return pos;
        }
    }

    return NULL;
}

void *__short_bsearch_upper(IN void *key,
                            IN void *base,
                            IN tt_u32_t num,
                            IN tt_u32_t width,
                            IN tt_cmp_t cmp)
{
    tt_u8_t *start, *end, *pos;

    TT_ASSERT(num > 0);

    start = (tt_u8_t *)base;
    end = start + (num - 1) * width;

    for (pos = start; pos <= end; pos += width) {
        if (cmp(key, pos) <= 0) {
            return pos;
        }
    }

    return NULL;
}

void *__short_bsearch_lower(IN void *key,
                            IN void *base,
                            IN tt_u32_t num,
                            IN tt_u32_t width,
                            IN tt_cmp_t cmp)
{
    tt_u8_t *start, *end, *pos;

    TT_ASSERT(num > 0);

    start = (tt_u8_t *)base;
    end = start + (num - 1) * width;

    for (pos = end; pos >= start; pos -= width) {
        if (cmp(key, pos) >= 0) {
            return pos;
        }
    }

    return NULL;
}
