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
@file tt_util_native.h
@brief some utilities

this file provided some basic utilities for platform usage
*/

#ifndef __TT_UTIL_NATIVE__
#define __TT_UTIL_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_sys_error.h>

#include <errno.h>
#include <unistd.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// continue if interrupted
#define __RETRY_IF_EINTR(fail_expression)                                      \
    do {                                                                       \
        int __failed;                                                          \
        while ((__failed = (fail_expression)) && (errno == EINTR))             \
            ;                                                                  \
        if (__failed) {                                                        \
            TT_ERROR_NTV("failed: %s, %d[%s]", #fail_expression, errno,        \
                         strerror(errno));                                     \
        }                                                                      \
    } while (0)

#define tt_ep_read(epfd, fd, ptr)                                              \
    tt_epoll((epfd), EPOLL_CTL_MOD, (fd),                                      \
             (EPOLLIN | EPOLLRDHUP | EPOLLONESHOT), (ptr))
#define tt_ep_unread(epfd, fd, ptr)                                            \
    tt_epoll((epfd), EPOLL_CTL_MOD, (fd), (EPOLLRDHUP | EPOLLONESHOT), (ptr))

#define tt_ep_write(epfd, fd, ptr)                                             \
    tt_epoll((epfd), EPOLL_CTL_MOD, (fd), (EPOLLOUT | EPOLLONESHOT), (ptr))

#define tt_ep_read_write(epfd, fd, ptr)                                        \
    tt_epoll((epfd), EPOLL_CTL_MOD, (fd),                                      \
             (EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLONESHOT), (ptr))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_epoll(IN int epfd, IN int op, IN int fd,
                            IN uint32_t events, IN void *ptr);

#endif /* __TT_UTIL_NATIVE__ */
