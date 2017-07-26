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
@file tt_socket_option.h
@brief get/set socket option

this file specifies apis to get or set socket option
*/

#ifndef __TT_SOCKET_OPTION__
#define __TT_SOCKET_OPTION__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_skt_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// ========================================
// ipv6 only
// ========================================

tt_export tt_result_t tt_skt_set_ipv6only(IN struct tt_skt_s *skt,
                                          IN tt_bool_t ipv6only);

tt_export tt_result_t tt_skt_get_ipv6only(IN struct tt_skt_s *skt,
                                          OUT tt_bool_t *ipv6only);

// ========================================
// reuse address
// ========================================

tt_export tt_result_t tt_skt_set_reuseaddr(IN struct tt_skt_s *skt,
                                           IN tt_bool_t reuse_addr);

tt_export tt_result_t tt_skt_get_reuseaddr(IN struct tt_skt_s *skt,
                                           OUT tt_bool_t *reuse_addr);

// ========================================
// reuse port
// ========================================

// - on windows, reuseaddr implies reuseport. but note the usage
//   of SO_EXCLUSIVEADDRUSE
// - on linux, reuseaddr of udp socket implies reuseport, tcp
//   socket has no such option untill 3.9
// - on mac os, both are supported
tt_export tt_result_t tt_skt_set_reuseport(IN struct tt_skt_s *skt,
                                           IN tt_bool_t reuse_port);

tt_export tt_result_t tt_skt_get_reuseport(IN struct tt_skt_s *skt,
                                           OUT tt_bool_t *reuse_port);

// ========================================
// tcp no delay
// ========================================

tt_export tt_result_t tt_skt_set_tcp_nodelay(IN struct tt_skt_s *skt,
                                             IN tt_bool_t nodelay);

tt_export tt_result_t tt_skt_get_tcp_nodelay(IN struct tt_skt_s *skt,
                                             OUT tt_bool_t *nodelay);

// ========================================
// non-block
// ========================================

tt_export tt_result_t tt_skt_set_nonblock(IN struct tt_skt_s *skt,
                                          IN tt_bool_t nonblock);


// ========================================
// linger
// ========================================

tt_export tt_result_t tt_skt_set_linger(IN struct tt_skt_s *skt,
                                        IN tt_bool_t enable,
                                        IN tt_u16_t linger_sec);

#endif // __TT_SOCKET_OPTION__
