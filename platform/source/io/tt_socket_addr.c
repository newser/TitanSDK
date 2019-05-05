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

tt_result_t tt_sktaddr_set_ip_p(IN tt_sktaddr_t *addr,
                                IN const tt_char_t *ip_str)
{
    tt_sktaddr_ip_t ip;

    if (ip_str == TT_IP_ANY) {
        tt_sktaddr_set_ip_n(addr, TT_IP_ANY);
        return TT_SUCCESS;
    }

    if (!TT_OK(tt_sktaddr_ip_p2n(tt_sktaddr_get_family(addr), ip_str, &ip))) {
        return TT_FAIL;
    }
    tt_sktaddr_set_ip_n(addr, &ip);

    return TT_SUCCESS;
}

tt_result_t tt_sktaddr_get_ip_p(IN tt_sktaddr_t *addr, OUT tt_char_t *buf,
                                IN tt_u32_t buf_len)
{
    tt_sktaddr_ip_t ip;

    tt_sktaddr_get_ip_n(addr, &ip);
    return tt_sktaddr_ip_n2p(tt_sktaddr_get_family(addr), &ip, buf, buf_len);
}

tt_sktaddr_t *tt_sktaddr_map4to6(IN tt_sktaddr_t *ip4, OUT tt_sktaddr_t *ip6)
{
    if (tt_sktaddr_get_family(ip4) == TT_NET_AF_INET) {
        tt_sktaddr_map4to6_ntv(ip4, ip6);
        return ip6;
    } else {
        return ip4;
    }
}
