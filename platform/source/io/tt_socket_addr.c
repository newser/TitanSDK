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

#include <io/tt_socket_addr.h>

#include <log/tt_log.h>
#include <misc/tt_assert.h>

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

tt_result_t tt_sktaddr_set_addr_p(IN tt_sktaddr_t *addr, IN tt_char_t *pa)
{
    tt_sktaddr_addr_t na;

    if (pa == TT_SKTADDR_ANY) {
        tt_sktaddr_set_addr_n(addr, TT_SKTADDR_ANY);
        return TT_SUCCESS;
    }

    if (!TT_OK(tt_sktaddr_addr_p2n(tt_sktaddr_get_family(addr), pa, &na))) {
        return TT_FAIL;
    }
    tt_sktaddr_set_addr_n(addr, &na);

    return TT_SUCCESS;
}

tt_result_t tt_sktaddr_get_addr_p(IN tt_sktaddr_t *addr,
                                  OUT tt_char_t *pa,
                                  IN tt_u32_t pa_len)
{
    tt_sktaddr_addr_t na;

    tt_sktaddr_get_addr_n(addr, &na);
    return tt_sktaddr_addr_n2p(tt_sktaddr_get_family(addr), &na, pa, pa_len);
}

tt_sktaddr_t *tt_sktaddr_map4to6(IN tt_sktaddr_t *in4, OUT tt_sktaddr_t *in6)
{
    if (tt_sktaddr_get_family(in4) == TT_NET_AF_INET) {
        tt_sktaddr_map4to6_ntv(in4, in6);
        return in6;
    } else {
        return in4;
    }
}
