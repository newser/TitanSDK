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

#include <network/adns/tt_adns_rr_a_in.h>

#include <algorithm/tt_buffer_format.h>
#include <network/adns/tt_adns_rr.h>

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

static void __a_dump(IN struct tt_adns_rr_s *rr);

static tt_result_t __a_render_prepare(IN struct tt_adns_rr_s *rr,
                                      OUT tt_u32_t *len);

static tt_result_t __a_render(IN struct tt_adns_rr_s *rr,
                              OUT struct tt_buf_s *buf);

static tt_result_t __a_parse(OUT struct tt_adns_rr_s *rr,
                             IN struct tt_buf_s *buf,
                             IN tt_u8_t *pkt,
                             IN tt_u32_t pkt_len);

static tt_adns_rr_itf_t __a_itf = {NULL,
                                   NULL,
                                   NULL,
                                   __a_dump,

                                   __a_render_prepare,
                                   __a_render,
                                   __a_parse};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_adns_rr_t *tt_adrr_a_create(IN const tt_char_t *name,
                               IN tt_u32_t name_ownership,
                               IN tt_u32_t ttl,
                               IN OPT tt_adrr_a_t *rdata)
{
    tt_adns_rr_t *rr;

    rr = tt_adns_rr_create((tt_u32_t)sizeof(tt_adrr_a_t),
                           &__a_itf,
                           name,
                           name_ownership,
                           TT_ADNS_RR_A_IN,
                           ttl);
    if (rr == NULL) {
        return NULL;
    }

    if (rdata != NULL) {
        tt_adrr_a_set_addr(rr, &rdata->addr);
    }

    return rr;
}

void tt_adrr_a_set_addr(IN struct tt_adns_rr_s *rr, tt_sktaddr_addr_t *addr)
{
    tt_adrr_a_t *rdata;

    TT_ASSERT(rr->type == TT_ADNS_RR_A_IN);
    rdata = TT_ADRR_CAST(rr, tt_adrr_a_t);

    tt_memcpy(&rdata->addr, addr, sizeof(tt_sktaddr_addr_t));
}

void __a_dump(IN struct tt_adns_rr_s *rr)
{
    tt_adrr_a_t *rdata = TT_ADRR_CAST(rr, tt_adrr_a_t);
    tt_char_t ip_addr[30] = {0};

    tt_sktaddr_addr_n2p(TT_NET_AF_INET,
                        &rdata->addr,
                        ip_addr,
                        sizeof(ip_addr) - 1);

    TT_INFO("- domain: %s", rr->name);
    TT_INFO("  type clase: A IN");
    TT_INFO("  ip: %s", ip_addr);
}

tt_result_t __a_render_prepare(IN struct tt_adns_rr_s *rr, OUT tt_u32_t *len)
{
    // 4 bytes for rdata
    *len = 4;
    return TT_SUCCESS;
}

tt_result_t __a_render(IN struct tt_adns_rr_s *rr, OUT struct tt_buf_s *buf)
{
    tt_adrr_a_t *rdata = TT_ADRR_CAST(rr, tt_adrr_a_t);

    // rdata, already in network order
    TT_DO(tt_buf_put_u32(buf, rdata->addr.addr32.__u32));

    return TT_SUCCESS;
}

tt_result_t __a_parse(OUT struct tt_adns_rr_s *rr,
                      IN struct tt_buf_s *buf,
                      IN tt_u8_t *pkt,
                      IN tt_u32_t pkt_len)
{
    tt_adrr_a_t *rdata = TT_ADRR_CAST(rr, tt_adrr_a_t);

    // rdata, already in network order
    TT_DO(tt_buf_get_u32(buf, &rdata->addr.addr32.__u32));

    return TT_SUCCESS;
}
