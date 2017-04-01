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
@file tt_socket_addr_native.h
@brief socket address native definitions

this file specifies socket address native definitions
*/

#ifndef __TT_SOCKET_ADDR_NATIVE__
#define __TT_SOCKET_ADDR_NATIVE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_network_io_def.h>
#include <tt_basic_type.h>

#include <arpa/inet.h>
#include <netinet/in.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

union tt_sktaddr_ip_s;

typedef struct sockaddr_storage tt_sktaddr_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern void tt_sktaddr_init_ntv(IN tt_sktaddr_ntv_t *addr,
                                IN tt_net_family_t family);

extern tt_net_family_t tt_sktaddr_get_family_ntv(IN tt_sktaddr_ntv_t *addr);

extern void tt_sktaddr_set_ip_n_ntv(IN tt_sktaddr_ntv_t *addr,
                                    IN union tt_sktaddr_ip_s *na);

// returned value is unspecified if addr is set to TT_SKT_IP_ANY
extern void tt_sktaddr_get_ip_n_ntv(IN tt_sktaddr_ntv_t *addr,
                                    OUT union tt_sktaddr_ip_s *na);

extern void tt_sktaddr_set_port_ntv(IN tt_sktaddr_ntv_t *addr,
                                    IN tt_u16_t port);

extern tt_u16_t tt_sktaddr_get_port_ntv(IN tt_sktaddr_ntv_t *addr);

extern void tt_sktaddr_set_scope_ntv(IN tt_sktaddr_ntv_t *addr,
                                     IN tt_u32_t scope_id);

extern void tt_sktaddr_set_scope_p_ntv(IN tt_sktaddr_ntv_t *addr,
                                       IN tt_char_t *scope_name);

extern tt_result_t tt_sktaddr_ip_n2p_ntv(IN tt_net_family_t family,
                                         IN union tt_sktaddr_ip_s *na,
                                         OUT tt_char_t *buf,
                                         IN tt_u32_t buf_len);

extern tt_result_t tt_sktaddr_ip_p2n_ntv(IN tt_net_family_t family,
                                         IN tt_char_t *buf,
                                         OUT union tt_sktaddr_ip_s *na);

extern void tt_sktaddr_map4to6_ntv(IN tt_sktaddr_ntv_t *in4,
                                   OUT tt_sktaddr_ntv_t *in6);

extern tt_bool_t tt_sktaddr_ipv4mapped_ntv(IN tt_sktaddr_ntv_t *addr);

#endif // __TT_SOCKET_ADDR_NATIVE__
