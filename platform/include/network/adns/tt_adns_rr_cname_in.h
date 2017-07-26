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
@file tt_adns_rr_cname_in.h
@brief adns rr cname in

this file defines adns resource record: cname in
*/

#ifndef __TT_ADNS_RR_CNAME_IN__
#define __TT_ADNS_RR_CNAME_IN__

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

struct tt_adns_rr_s;

typedef struct
{
    /*
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     /                     CNAME                     /
     /                                               /
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     */

    tt_char_t *cname;
    tt_u32_t cname_len;
} tt_adrr_cname_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export struct tt_adns_rr_s *tt_adrr_cname_create(IN const tt_char_t *name,
                                                 IN tt_u32_t name_ownership,
                                                 IN tt_u32_t ttl,
                                                 IN tt_adrr_cname_t *rdata);

// cname_len must count terminating null, if it's 0, cname_len will be
// calculated
tt_export tt_result_t tt_adrr_cname_set_cname(IN struct tt_adns_rr_s *rr,
                                           IN tt_char_t *cname,
                                           IN tt_u32_t cname_len);

#endif /* __TT_ADNS_RR_CNAME_IN__ */
