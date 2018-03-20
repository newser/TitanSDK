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
@file tt_mac_addr.h
@brief mac addr

mac addr
*/

#ifndef __TT_MAC_ADDR__
#define __TT_MAC_ADDR__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_MACADDR_LEN 6

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_macaddr_s
{
    tt_u8_t addr[TT_MACADDR_LEN];
} tt_macaddr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// addr must has TT_MACADDR_LEN bytes
tt_export void tt_macaddr_init(IN tt_macaddr_t *ma, IN OPT tt_u8_t *addr);

tt_export tt_result_t tt_macaddr_p2n(IN tt_macaddr_t *ma,
                                     IN const tt_char_t *cstr);

tt_export tt_result_t tt_macaddr_n2p(IN tt_macaddr_t *ma,
                                     OUT tt_char_t *cstr,
                                     IN tt_u32_t len,
                                     IN tt_u32_t flag);

#endif // __TT_MAC_ADDR__
