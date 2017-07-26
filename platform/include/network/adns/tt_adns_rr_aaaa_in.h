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
 @file tt_adns_rr_aaaa_in.h
 @brief adns rr AAAA IN

 this file defines adns rr AAAA IN
 */

#ifndef __TT_ADNS_RR_AAAA_IN__
#define __TT_ADNS_RR_AAAA_IN__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <io/tt_socket_addr.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_adns_rr_s;

typedef struct
{
    /*
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                    ADDRESS                    |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     */

    tt_sktaddr_ip_t addr;
} tt_adrr_aaaa_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_adns_rr_s *tt_adrr_aaaa_create(IN const tt_char_t *name,
                                                IN tt_u32_t name_ownership,
                                                IN tt_u32_t ttl,
                                                IN OPT tt_adrr_aaaa_t *rdata);

tt_export void tt_adrr_aaaa_set_addr(IN struct tt_adns_rr_s *rr,
                                  tt_sktaddr_ip_t *addr);

#endif /* __TT_ADNS_RR_AAAA_IN__ */
