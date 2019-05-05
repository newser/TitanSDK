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

#include <tt_cstd_api.h>

#include <misc/tt_assert.h>
#include <misc/tt_util.h>

#include <ctype.h>
#include <errno.h>
#include <limits.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __CASE_DIFF ((tt_char_t)('a' - 'A'))

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

tt_s32_t tt_strnicmp(IN const tt_char_t *s1, IN const tt_char_t *s2,
                     IN tt_u32_t n)
{
    tt_u32_t i = 0;

    TT_ASSERT(s1 != NULL);
    TT_ASSERT(s2 != NULL);

    while (i < n) {
        tt_char_t c1 = s1[i];
        tt_char_t c2 = s2[i];

        if ((c1 == 0) || (c2 == 0)) { return c1 - c2; }

        if (c1 == c2) {
            ++i;
            continue;
        }

        if (((c1 + __CASE_DIFF) == c2) || ((c2 + __CASE_DIFF) == c1)) {
            // note this allow different non-character bytes
            ++i;
            continue;
        }

        return c1 - c2;
    }

    return 0;
}

tt_s32_t tt_memicmp(IN const tt_u8_t *s1, IN const tt_u8_t *s2, IN tt_u32_t n)
{
    tt_u32_t i = 0;

    TT_ASSERT(s1 != NULL);
    TT_ASSERT(s2 != NULL);

    while (i < n) {
        tt_char_t c1 = s1[i];
        tt_char_t c2 = s2[i];

        if (c1 == c2) {
            ++i;
            continue;
        }

        if (((c1 + __CASE_DIFF) == c2) || ((c2 + __CASE_DIFF) == c1)) {
            ++i;
            continue;
        }

        TT_INFO("c1: %c, c2: %c", c1, c2);
        return c1 - c2;
    }

    return 0;
}

tt_result_t tt_strtou32(const char *str, char **endptr, int base, tt_u32_t *val)
{
    const char *__str;
    char *__endptr;
    unsigned long long ull_val;
    // on any platform, l_val is at least 4 bytes long

    // check if it's an empty string
    __str = str;
    while ((*__str != 0) && isspace(*__str)) { ++__str; }
    if (*__str == 0) { return TT_FAIL; }
    if (*__str == '-') { return TT_FAIL; }

    ull_val = strtoull(__str, &__endptr, base);

    // check if any unexpected char found
    if ((*__endptr != 0) && (isspace(*__endptr) == 0)) { return TT_FAIL; }
    TT_SAFE_ASSIGN(endptr, __endptr);

    // check overflow
    if ((ull_val == ULONG_LONG_MAX) & (errno == ERANGE)) { return TT_FAIL; }
    if (ull_val > (unsigned long long)0xFFFFFFFF) { return TT_FAIL; }

    *val = (tt_u32_t)ull_val;
    return TT_SUCCESS;
}

tt_result_t tt_strtos32(const char *str, char **endptr, int base, tt_s32_t *val)
{
    const char *__str;
    char *__endptr;
    long long ll_val;
    // on any platform, l_val is at least 4 bytes long

    // check if it's an empty string
    __str = str;
    while ((*__str != 0) && isspace(*__str)) { ++__str; }
    if (*__str == 0) { return TT_FAIL; }

    ll_val = strtoll(__str, &__endptr, base);

    // check if any unexpected char found
    if ((*__endptr != 0) && (isspace(*__endptr) == 0)) { return TT_FAIL; }
    TT_SAFE_ASSIGN(endptr, __endptr);

    // check overflow
    if (((ll_val == LONG_LONG_MIN) || (ll_val == LONG_LONG_MAX)) &
        (errno == ERANGE)) {
        return TT_FAIL;
    }
    if ((ll_val > (long long)0x7FFFFFFF) ||
        (ll_val < (long long)(int)0x80000000)) {
        return TT_FAIL;
    }

    *val = (tt_s32_t)ll_val;
    return TT_SUCCESS;
}

char *tt_strrstr(const char *haystack, const char *needle)
{
    size_t nlen, hlen;
    const char *p;
    char *sss1;
    char *sss2;

    nlen = strlen(needle);
    if (nlen == 0) { return (char *)haystack; }

    hlen = strlen(haystack);
    if (hlen < nlen) { return NULL; }

    p = haystack + hlen - nlen;
    while (p >= haystack) {
        if (memcmp(p, needle, nlen) == 0) { return (char *)p; }
        --p;
    }

    return NULL;
}
