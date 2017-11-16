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
@file tt_platform_util_native.h
@brief some utilities

this file provided some basic utilities for platform usage
*/

#ifndef __TT_PLATFORM_UTIL_NATIVE__
#define __TT_PLATFORM_UTIL_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

#include <CoreFoundation/CoreFoundation.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_CF_REF_SHOW(var)                                                    \
    do {                                                                       \
        TT_DEBUG(#var " ref[%d]", CFGetRetainCount(var));                      \
    } while (0)

// continue if interrupted
#define __RETRY_IF_EINTR(fail_expression)                                      \
    do {                                                                       \
        int __failed;                                                          \
        while ((__failed = (fail_expression)) && (errno == EINTR))             \
            ;                                                                  \
        if (__failed) {                                                        \
            TT_ERROR_NTV("failed: %s, %d[%s]",                                 \
                         #fail_expression,                                     \
                         errno,                                                \
                         strerror(errno));                                     \
        }                                                                      \
    } while (0)

#define tt_kq_read(kq, ident, udata)                                           \
    tt_kevent((kq), (ident), EVFILT_READ, EV_ADD | EV_ONESHOT, (udata))
#define tt_kq_unread(kq, ident, udata)                                         \
    tt_kevent((kq), (ident), EVFILT_READ, EV_DELETE, (udata))

#define tt_kq_write(kq, ident, udata)                                          \
    tt_kevent((kq), (ident), EVFILT_WRITE, EV_ADD | EV_ONESHOT, (udata))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_kevent(IN int kq,
                             IN uint64_t ident,
                             IN int16_t filter,
                             IN uint16_t flags,
                             IN uint64_t udata);

// remember use tt_free to free the return value
extern char *tt_cfstring_ptr(IN CFStringRef cfstr, OUT OPT tt_u32_t *len);

#endif /* __TT_PLATFORM_UTIL_NATIVE__ */
