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
@file tt_epoll_compat.h
@brief epoll compatibility

this file defines epoll compatibility
*/

#ifndef __TT_EPOLL_COMPAT__
#define __TT_EPOLL_COMPAT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <sys/epoll.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#ifndef EPOLLRDHUP
#warning EPOLLRDHUP is not defined, io may not work
#define EPOLLRDHUP 0x00002000
#endif

#ifndef EPOLLONESHOT
#warning EPOLLONESHOT is not defined, io may not work
#define EPOLLONESHOT 0x40000000
#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_EPOLL_COMPAT__ */
