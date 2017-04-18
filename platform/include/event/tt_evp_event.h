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

/**
@file tt_evp_event.h
@brief evpoller common definition

this file defines evpoller common definition
*/

#ifndef __TT_EVP_EVENT__
#define __TT_EVP_EVENT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <event/tt_event_base.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

enum
{
    TT_EVP_RANGE_CONTROL,
    TT_EVP_RANGE_MARK,
};

enum
{
    // ========================================
    // evpoller control event
    // ========================================

    TT_EVP_CONTROL_EV_START =
        TT_EV_MKID_SUB(TT_EV_RANGE_INTERNAL_EVPOLLER, TT_EVP_RANGE_CONTROL, 0),

    TT_EVP_CTL_EXIT,

    TT_EVP_CONTROL_EV_END,

    // ========================================
    // evpoller mark event
    // ========================================

    TT_EVP_MARK_EV_START =
        TT_EV_MKID_SUB(TT_EV_RANGE_INTERNAL_EVPOLLER, TT_EVP_RANGE_MARK, 0),

    // thread event
    TT_EVP_MARK_THREAD_EV,

    // epoll
    TT_EVP_MARK_EPOLL_SOCKET,
    TT_EVP_MARK_EPOLL_IPC,

    TT_EVP_MARK_EV_END,
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_EVP_EVENT__ */
