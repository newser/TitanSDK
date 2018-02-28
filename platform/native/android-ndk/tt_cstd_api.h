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

/**
@file tt_cstd_api.h
@brief c standard APIs wrapper

this file wraps neccesary c standard library operations for purpose:
- 1. some cstd api is not safe enough
- 2. even some cstd apis are not supported in all platforms
*/

#ifndef __TT_CSTD_AIP__
#define __TT_CSTD_AIP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <android/log.h>
#include <ctype.h>
#include <memory.h>
#include <stdarg.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

/**
@def tt_memset
wrapped c memset()
*/
#define tt_memset memset

/**
@def tt_memcpy
wrapped c memcpy()
*/
#define tt_memcpy memcpy

/**
@def tt_memcmp
wrapped c memcmp()
*/
#define tt_memcmp memcmp

/**
@def tt_memmove
wrapped c memmove()
*/
#define tt_memmove memmove

/**
@def tt_c_malloc
wrapped c malloc()
*/
#define tt_c_malloc malloc

/**
 @def tt_c_realloc
 wrapped c realloc()
 */
#define tt_c_realloc realloc

/**
@def tt_c_free
wrapped c free()
*/
#define tt_c_free free

/**
@def tt_strncpy
wrapped c strncpy()
*/
#define tt_strncpy strncpy

/**
@def tt_strncmp
wrapped c strncmp()
*/
#define tt_strncmp strncmp
/**
@def tt_strcmp
wrapped c strcmp()
*/
#define tt_strcmp strcmp

/**
@def tt_strlen
wrapped c strlen()
*/
#define tt_strlen strlen
/**
@def tt_strnlen
wrapped c strncmp()
*/
#define tt_strnlen strnlen

/**
@def tt_strstr
wrapped c strstr()
*/
#define tt_strstr strstr

/**
@def tt_strchr
wrapped c strchr()
*/
#define tt_strchr strchr

#define tt_strrchr strrchr

#define tt_isspace isspace

#define tt_isdigit isdigit

#define tt_isalpha isalpha

#define tt_isalnum isalnum

#define tt_isxdigit isxdigit

#define tt_tolower tolower

#define tt_toupper toupper

//#define tt_printf printf
#define tt_printf(...)                                                         \
    do {                                                                       \
        __android_log_print(ANDROID_LOG_INFO, "platform", __VA_ARGS__);        \
    } while (0)

#define tt_c_rand rand

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_s32_t tt_strnicmp(IN const tt_char_t *s1,
                            IN const tt_char_t *s2,
                            IN tt_u32_t n);

extern tt_s32_t tt_memicmp(IN const tt_u8_t *s1,
                           IN const tt_u8_t *s2,
                           IN tt_u32_t n);

// return how many bytes are put to str, not including 0
tt_inline int tt_vsnprintf(char *str,
                           size_t size,
                           const char *format,
                           va_list ap)
{
    int n = vsnprintf(str, size, format, ap);
    if ((n < 0) || (n >= (int)size)) {
        n = size - 1;
    }
    return n;
}

// return how many bytes are put to str, not including 0
tt_inline int tt_snprintf(char *str, size_t size, const char *format, ...)
{
    int n;

    va_list args;
    va_start(args, format);
    n = tt_vsnprintf(str, size, format, args);
    va_end(args);

    return n;
}

extern tt_result_t tt_strtou32(const char *str,
                               char **endptr,
                               int base,
                               tt_u32_t *val);

extern tt_result_t tt_strtos32(const char *str,
                               char **endptr,
                               int base,
                               tt_s32_t *val);

extern char *tt_strrstr(const char *haystack, const char *needle);

#endif /* __TT_CSTD_AIP__ */
