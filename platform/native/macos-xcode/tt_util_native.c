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

#include <tt_util_native.h>

#include <memory/tt_memory_alloc.h>

#include <tt_cstd_api.h>
#include <tt_sys_error.h>

#include <Security/Security.h>

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

tt_result_t tt_kevent(IN int kq, IN uint64_t ident, IN int16_t filter,
                      IN uint16_t flags, IN uint64_t udata)
{
    struct kevent kev = {0};
    const struct timespec timeout = {0};

    EV_SET(&kev, ident, filter, flags, 0, 0, udata);
    if (kevent(kq, &kev, 1, NULL, 0, &timeout) == 0) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("[%d]fail", filter);
        return TT_FAIL;
    }
}

char *tt_cfstring_ptr(IN CFStringRef cfstr, OUT OPT tt_u32_t *len)
{
    CFIndex __len;
    CFIndex max_len;
    char *ptr;

    if (cfstr == NULL) { return NULL; }

    __len = CFStringGetLength(cfstr);
    max_len = CFStringGetMaximumSizeForEncoding(__len, kCFStringEncodingUTF8);
    // kCFStringEncodingUTF8 or kCFStringEncodingASCII?

    ptr = tt_malloc(max_len);
    if (ptr == NULL) { return NULL; }

    tt_memset(ptr, 0, max_len);
    if (!CFStringGetCString(cfstr, ptr, max_len, kCFStringEncodingUTF8)) {
        tt_free(ptr);
        return NULL;
    }

    if (len != NULL) { *len = max_len; }
    return ptr;
}
