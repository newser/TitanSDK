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
@file tt_network_io_def.h
@brief network io definitions

this file specifies network io definitions
*/

#ifndef __TT_NETWORK_IO_DEF__
#define __TT_NETWORK_IO_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_NET_AF_INET,
    TT_NET_AF_INET6,

    TT_NET_AF_NUM,
    TT_NET_AF_INVALID,
} tt_net_family_t;
#define TT_NET_AF_VALID(af) ((af) < TT_NET_AF_NUM)

typedef enum {
    TT_NET_PROTO_UDP,
    TT_NET_PROTO_TCP,

    TT_NET_PROTO_NUM,
    TT_NET_PROTO_INVALID,
} tt_net_protocol_t;
#define TT_NET_PROTO_VALID(p) ((p) < TT_NET_PROTO_NUM)

typedef enum {
    TT_SKT_SHUT_RD,
    TT_SKT_SHUT_WR,
    TT_SKT_SHUT_RDWR,

    TT_SKT_SHUT_NUM
} tt_skt_shut_t;
#define TT_SKT_SHUT_VALID(s) ((s) < TT_SKT_SHUT_NUM)

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif // __TT_NETWORK_IO_DEF__
