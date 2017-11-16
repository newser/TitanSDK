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
@file tt_buffer_native.h
@brief buffer operation native

this file defines buffer operation native APIs
*/

#ifndef __TT_BUFFER_NATIVE__
#define __TT_BUFFER_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_util.h>

#include <arpa/inet.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define tt_hton16_ntv htons
#define tt_ntoh16_ntv ntohs

#define tt_hton32_ntv htonl
#define tt_ntoh32_ntv ntohl

#define tt_hton64_ntv __hton64
#define tt_ntoh64_ntv __ntoh64

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_u64_t __hton64(IN tt_u64_t val)
{
    if (!tt_g_big_endian) {
        tt_u8_t *u8 = (tt_u8_t *)&val;
        TT_SWAP_U8(u8[0], u8[7]);
        TT_SWAP_U8(u8[1], u8[6]);
        TT_SWAP_U8(u8[2], u8[5]);
        TT_SWAP_U8(u8[3], u8[4]);
    }
    return val;
}

tt_inline tt_u64_t __ntoh64(IN tt_u64_t val)
{
    if (!tt_g_big_endian) {
        tt_u8_t *u8 = (tt_u8_t *)&val;
        TT_SWAP_U8(u8[0], u8[7]);
        TT_SWAP_U8(u8[1], u8[6]);
        TT_SWAP_U8(u8[2], u8[5]);
        TT_SWAP_U8(u8[3], u8[4]);
    }
    return val;
}

#endif /* __TT_BUFFER_NATIVE__ */
