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
 @file tt_log_io_tcp.h
 @brief log io tcp

 this file defines log io tcp output
 */

#ifndef __TT_LOG_IO_TCP__
#define __TT_LOG_IO_TCP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_socket.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_logio_s;

typedef struct
{
    tt_skt_attr_t skt_attr;
} tt_logio_tcp_attr_t;

typedef struct
{
    tt_skt_t *skt;
    tt_sktaddr_t addr;
    tt_u8_t state;
#define __LC_INIT 0
#define __LC_CONNECTED 1
#define __LC_DISCONNECTED 2
} tt_logio_tcp_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_logio_s *tt_logio_tcp_create(
    IN tt_net_family_t family, IN tt_sktaddr_t *addr,
    IN OPT tt_logio_tcp_attr_t *attr);

tt_export struct tt_logio_s *tt_logio_tcp_create_skt(IN TO tt_skt_t *skt,
                                                     IN tt_sktaddr_t *addr);

tt_export void tt_logio_tcp_attr_default(IN tt_logio_tcp_attr_t *attr);

#endif /* __TT_LOG_IO_TCP__ */
