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

#include <io/tt_mac_addr.h>

#include <misc/tt_assert.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>

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

void tt_macaddr_init(IN tt_macaddr_t *ma, IN OPT tt_u8_t *addr)
{
    TT_ASSERT(ma != NULL);

    if (addr != NULL) {
        tt_memcpy(ma->addr, addr, TT_MACADDR_LEN);
    } else {
        tt_memset(ma->addr, 0, TT_MACADDR_LEN);
    }
}

tt_result_t tt_macaddr_p2n(IN tt_macaddr_t *ma, IN const tt_char_t *cstr)
{
    tt_u32_t i;
    const tt_char_t *p, *next;

    TT_ASSERT(ma != NULL);
    TT_ASSERT(cstr != NULL);

    i = 0;
    p = cstr;
    while (i < TT_MACADDR_LEN) {
        next = tt_strchr(p, ':');
        if (next == NULL) {
            if (i == (TT_MACADDR_LEN - 1)) {
                next = cstr + tt_strlen(cstr);
            } else {
                TT_ERROR("invalid mac addr: %s", cstr);
                return TT_E_BADARG;
            }
        }

        if ((p + 1) == next) {
            if (tt_isalnum(p[0])) {
                ma->addr[i++] = tt_c2h(p[0], 0);
            } else {
                TT_ERROR("not alpha or num: [%x]", p[0]);
                return TT_E_BADARG;
            }
        } else if ((p + 2) == next) {
            if (tt_isalnum(p[0]) && tt_isalnum(p[1])) {
                ma->addr[i++] = (tt_c2h(p[0], 0) << 4) | tt_c2h(p[1], 0);
            } else {
                TT_ERROR("not alpha or num: [%x, %x]", p[0], p[1]);
                return TT_E_BADARG;
            }
        } else {
            TT_ERROR("invalid mac addr: %s", cstr);
            return TT_E_BADARG;
        }

        p = next + 1;
    }

    if ((i == TT_MACADDR_LEN) && (*next == 0)) {
        return TT_SUCCESS;
    } else {
        TT_ERROR("invalid mac addr: %s", cstr);
        return TT_E_BADARG;
    }
}

tt_result_t tt_macaddr_n2p(IN tt_macaddr_t *ma, OUT tt_char_t *cstr,
                           IN tt_u32_t len, IN tt_u32_t flag)
{
    TT_ASSERT(ma != NULL);
    TT_ASSERT(cstr != NULL);

    if (len < 18) {
        TT_ERROR("not enough space for mac string");
        return TT_E_NOSPC;
    }

    tt_snprintf(cstr, 18, "%02X:%02X:%02X:%02x:%02x:%02x", ma->addr[0],
                ma->addr[1], ma->addr[2], ma->addr[3], ma->addr[4],
                ma->addr[5]);
    return TT_SUCCESS;
}
