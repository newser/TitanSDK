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

#include <tt_wchar.h>

#include <memory/tt_memory_alloc.h>
#include <misc/tt_util.h>

#include <tt_sys_error.h>

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

wchar_t *tt_wchar_create(IN const tt_char_t *utf8_str,
    IN tt_u32_t len,
                         OUT OPT tt_u32_t *wchar_bytes)
{
    wchar_t *wchar_str;
    tt_u32_t char_num, byte_num;
    int cbMultiByte = TT_COND(len > 0, len, (int)-1);
    
    // If this parameter is -1, the function processes the entire input
    // string, including the terminating null character. Therefore, the
    // resulting Unicode string has a terminating null character, and
    // the length returned by the function includes this character.
    char_num = MultiByteToWideChar(CP_UTF8, 0, utf8_str, cbMultiByte, NULL, 0);
    if (char_num == 0) {
        TT_ERROR_NTV("fail to get wchar len");
        return NULL;
    }

    byte_num = char_num * sizeof(wchar_t);
    wchar_str = (wchar_t *)tt_malloc(char_num * sizeof(wchar_t));
    if (wchar_str == NULL) {
        TT_ERROR("no mem for wchar string");
        return NULL;
    }

    char_num =
        MultiByteToWideChar(CP_UTF8, 0, utf8_str, cbMultiByte, wchar_str, char_num);
    if (char_num == 0) {
        TT_ERROR_NTV("fail to convert to wchar");

        tt_free(wchar_str);
        return NULL;
    }

    TT_SAFE_ASSIGN(wchar_bytes, byte_num);
    return wchar_str;
}

wchar_t *tt_wchar_create_ex(IN const tt_char_t *utf8_str,
    IN tt_u32_t len,
                            OUT OPT tt_u32_t *wchar_bytes,
                            IN tt_u32_t flag,
                            IN void *val)
{
    wchar_t *wchar_str;
    tt_u32_t char_num, byte_num;
    int cbMultiByte = TT_COND(len > 0, len, (int)-1);

    // If this parameter is -1, the function processes the entire input
    // string, including the terminating null character. Therefore, the
    // resulting Unicode string has a terminating null character, and
    // the length returned by the function includes this character.
    char_num = MultiByteToWideChar(CP_UTF8, 0, utf8_str, cbMultiByte, NULL, 0);
    if (char_num == 0) {
        TT_ERROR_NTV("fail to get wchar len");
        return NULL;
    }

    if (flag & TT_WCHAR_CREATE_LONGER) {
        char_num += (tt_u32_t)(tt_uintptr_t)val;
    }

    byte_num = char_num * sizeof(wchar_t);
    wchar_str = (wchar_t *)tt_malloc(byte_num);
    if (wchar_str == NULL) {
        TT_ERROR("no mem for wchar string");
        return NULL;
    }
    memset(wchar_str, 0, byte_num);

    char_num =
        MultiByteToWideChar(CP_UTF8, 0, utf8_str, cbMultiByte, wchar_str, char_num);
    if (char_num == 0) {
        TT_ERROR_NTV("fail to convert to wchar");

        tt_free(wchar_str);
        return NULL;
    }

    TT_SAFE_ASSIGN(wchar_bytes, byte_num);
    return wchar_str;
}

void tt_wchar_destroy(IN wchar_t *wchar_str)
{
    tt_free(wchar_str);
}

tt_char_t *tt_utf8_create(IN wchar_t *wchar_str, 
    IN tt_u32_t len,
    OUT OPT tt_u32_t *utf8_bytes)
{
    tt_char_t *utf8_str;
    tt_u32_t char_num;
    int cchWideChar = TT_COND(len > 0, len, (int)-1);

    // If this parameter is -1, the function processes the entire input
    // string, including the terminating null character. Therefore, the
    // resulting character string has a terminating null character, and
    // the length returned by the function includes this character.
    char_num =
        WideCharToMultiByte(CP_UTF8, 0, wchar_str, cchWideChar, NULL, 0, NULL, NULL);
    if (char_num == 0) {
        TT_ERROR_NTV("fail to get utf8 len");
        return NULL;
    }

    utf8_str = (tt_char_t *)tt_malloc(char_num);
    if (utf8_str == NULL) {
        TT_ERROR("no mem for utf8 string");
        return NULL;
    }

    char_num = WideCharToMultiByte(CP_UTF8,
                                   0,
                                   wchar_str,
                                   cchWideChar,
                                   utf8_str,
                                   char_num,
                                   NULL,
                                   NULL);
    if (char_num == 0) {
        TT_ERROR_NTV("fail to convert to wchar");

        tt_free(utf8_str);
        return NULL;
    }

    TT_SAFE_ASSIGN(utf8_bytes, char_num);
    return utf8_str;
}

void tt_utf8_destroy(IN tt_char_t *utf8_str)
{
    tt_free(utf8_str);
}
