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

tt_result_t __skt_kevent(IN int kq,
                         IN uint64_t ident,
                         IN int16_t filter,
                         IN uint16_t flags,
                         IN uint64_t udata)
{
    struct kevent ev = {0};
    const struct timespec timeout = {0};

    EV_SET(&ev, ident, filter, flags, 0, 0, udata);
    if (kevent(kq, &ev, 1, NULL, 0, &timeout) == 0) {
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

    if (cfstr == NULL) {
        return NULL;
    }

    __len = CFStringGetLength(cfstr);
    max_len = CFStringGetMaximumSizeForEncoding(__len, kCFStringEncodingUTF8);
    // kCFStringEncodingUTF8 or kCFStringEncodingASCII?

    ptr = tt_mem_alloc(max_len);
    if (ptr == NULL) {
        return NULL;
    }

    tt_memset(ptr, 0, max_len);
    if (!CFStringGetCString(cfstr, ptr, max_len, kCFStringEncodingUTF8)) {
        tt_mem_free(ptr);
        return NULL;
    }

    if (len != NULL) {
        *len = (tt_u32_t)max_len;
    }
    return ptr;
}

void tt_osstatus_show(IN OSStatus osst)
{
    switch (osst) {
        case errSecSuccess:
            TT_ERROR("errSecSuccess: No error");
            break;
        case errSecUnimplemented:
            TT_ERROR(
                "errSecUnimplemented: Function or operation not implemented");
            break;
        case errSecIO:
            TT_ERROR("errSecIO: I/O error (bummers)");
            break;
        case errSecOpWr:
            TT_ERROR(
                "errSecOpWr: file already open with with write permission");
            break;
        case errSecParam:
            TT_ERROR(
                "errSecParam: One or more parameters passed to a function "
                "where not valid");
            break;
        case errSecAllocate:
            TT_ERROR("errSecAllocate: Failed to allocate memory");
            break;
        case errSecUserCanceled:
            TT_ERROR("errSecUserCanceled: User canceled the operation");
            break;
        case errSecBadReq:
            TT_ERROR(
                "errSecBadReq: Bad parameter or invalid state for operation");
            break;
        case errSecInternalComponent:
            TT_ERROR("errSecInternalComponent: errSecInternalComponent");
            break;
        case errSecDuplicateItem:
            TT_ERROR(
                "errSecDuplicateItem: The specified item already exists in the "
                "keychain");
            break;
        case errSecNotAvailable:
            TT_ERROR(
                "errSecNotAvailable: No keychain is available. You may need to "
                "restart your computer");
            break;
        case errSecItemNotFound:
            TT_ERROR(
                "errSecItemNotFound: The specified item could not be found in "
                "the keychain");
            break;
        case errSecInteractionNotAllowed:
            TT_ERROR(
                "errSecInteractionNotAllowed: User interaction is not allowed");
            break;
        case errSecDecode:
            TT_ERROR("errSecDecode: Unable to decode the provided data");
            break;
        case errSecAuthFailed:
            TT_ERROR(
                "errSecAuthFailed: The user name or passphrase you entered is "
                "not correct");
            break;

        default:
            TT_ERROR("OSStatus: %d", osst);
            break;
    }
}
