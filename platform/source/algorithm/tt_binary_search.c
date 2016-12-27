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

#define __SHORT_THRESHOLD 8

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
    tt_u8_t *start, *end, *middle;
    tt_u32_t n;

    TT_ASSERT(key != NULL);
    TT_ASSERT(base != NULL);
    TT_ASSERT(num != 0);
    TT_ASSERT(width != 0);
    TT_ASSERT(cmp != NULL);

    start = (tt_u8_t *)base;
    end = start + (num - 1) * width;
    middle = NULL;

sub_search:
    n = (tt_u32_t)((end - start) / width + 1);

    if (n <= __SHORT_THRESHOLD) {
        return __short_bsearch(key, base, num, width, cmp);
    } else {
        tt_s32_t cmp_ret = 0;

        middle = start + (n >> 1) * width;

        cmp_ret = cmp(key, middle);
        if (cmp_ret < 0) {
            if (start < middle) {
                end = middle - width;
                goto sub_search;
            } else {
                return NULL;
            }
        } else if (cmp_ret > 0) {
            if (middle < end) {
                start = middle + width;
                goto sub_search;
            } else {
                return NULL;
            }
        } else {
            return middle;
        }
    }
}

void *tt_bsearch_upper(IN void *key,
                       IN void *base,
                       IN tt_u32_t num,
                       IN tt_u32_t width,
                       IN tt_cmp_t cmp)
{
    tt_u8_t *start, *end, *middle;
    tt_u32_t n;

    TT_ASSERT(key != NULL);
    TT_ASSERT(base != NULL);
    TT_ASSERT(num != 0);
    TT_ASSERT(width != 0);
    TT_ASSERT(cmp != NULL);

    start = (tt_u8_t *)base;
    end = start + (num - 1) * width;
    middle = NULL;

sub_search:
    n = (tt_u32_t)((end - start) / width + 1);

    if (n <= __SHORT_THRESHOLD) {
        return __short_bsearch_upper(key, base, num, width, cmp);
    } else {
        tt_s32_t cmp_ret = 0;

        middle = start + (n >> 1) * width;

        cmp_ret = cmp(key, middle);
        if (cmp_ret < 0) {
            if (start < middle) {
                end = middle - width;
                if (cmp(key, end) > 0) {
                    return middle;
                }

                goto sub_search;
            } else {
                return middle;
            }
        } else if (cmp_ret > 0) {
            if (middle < end) {
                start = middle + width;
                goto sub_search;
            } else {
                return NULL;
            }
        } else {
            return middle;
        }
    }
}

void *tt_bsearch_lower(IN void *key,
                       IN void *base,
                       IN tt_u32_t num,
                       IN tt_u32_t width,
                       IN tt_cmp_t cmp)
{
    tt_u8_t *start, *end, *middle;
    tt_u32_t n;

    TT_ASSERT(key != NULL);
    TT_ASSERT(base != NULL);
    TT_ASSERT(num != 0);
    TT_ASSERT(width != 0);
    TT_ASSERT(cmp != NULL);

    start = (tt_u8_t *)base;
    end = start + (num - 1) * width;
    middle = NULL;

sub_search:
    n = (tt_u32_t)((end - start) / width + 1);

    if (n <= __SHORT_THRESHOLD) {
        return __short_bsearch_lower(key, base, num, width, cmp);
    } else {
        tt_s32_t cmp_ret = 0;

        middle = start + (n >> 1) * width;

        cmp_ret = cmp(key, middle);
        if (cmp_ret < 0) {
            if (start < middle) {
                end = middle - width;
                goto sub_search;
            } else {
                return NULL;
            }
        } else if (cmp_ret > 0) {
            if (middle < end) {
                start = middle + width;
                if (cmp(key, start) < 0) {
                    return middle;
                }
                goto sub_search;
            } else {
                return middle;
            }
        } else {
            return middle;
        }
    }
}

void *__short_bsearch(IN void *key,
                      IN void *base,
                      IN tt_u32_t num,
                      IN tt_u32_t width,
                      IN tt_cmp_t cmp)
{
    tt_u8_t *start, *end, *pos;

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

    start = (tt_u8_t *)base;
    end = start + (num - 1) * width;

    for (pos = end; pos >= start; pos -= width) {
        if (cmp(key, pos) >= 0) {
            return pos;
        }
    }

    return NULL;
}
