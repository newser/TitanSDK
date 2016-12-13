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

#include <network/adns/tt_adns_rr_cname_in.h>

#include <algorithm/tt_buffer_format.h>
#include <memory/tt_memory_alloc.h>
#include <network/adns/tt_adns_domain_name.h>
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

static void __cname_destroy(IN struct tt_adns_rr_s *rr);

static void __cname_dump(IN struct tt_adns_rr_s *rr);

static tt_result_t __cname_render_prepare(IN struct tt_adns_rr_s *rr,
                                          OUT tt_u32_t *len);

static tt_result_t __cname_render(IN struct tt_adns_rr_s *rr,
                                  OUT struct tt_buf_s *buf);

static tt_result_t __cname_parse(OUT struct tt_adns_rr_s *rr,
                                 IN struct tt_buf_s *buf,
                                 IN tt_u8_t *pkt,
                                 IN tt_u32_t pkt_len);

static tt_adns_rr_itf_t __cname_itf = {NULL,
                                       __cname_destroy,
                                       NULL,
                                       __cname_dump,

                                       __cname_render_prepare,
                                       __cname_render,
                                       __cname_parse};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_adns_rr_t *tt_adrr_cname_create(IN const tt_char_t *name,
                                   IN tt_u32_t name_ownership,
                                   IN tt_u32_t ttl,
                                   IN tt_adrr_cname_t *rdata)
{
    tt_adns_rr_t *rr;

    rr = tt_adns_rr_create((tt_u32_t)sizeof(tt_adrr_cname_t),
                           &__cname_itf,
                           name,
                           name_ownership,
                           TT_ADNS_RR_CNAME_IN,
                           ttl);
    if (rr == NULL) {
        return NULL;
    }

    if ((rdata != NULL) &&
        !TT_OK(tt_adrr_cname_set_cname(rr, rdata->cname, rdata->cname_len))) {
        tt_adns_rr_destroy(rr);
        return NULL;
    }

    return rr;
}

tt_result_t tt_adrr_cname_set_cname(IN struct tt_adns_rr_s *rr,
                                    IN tt_char_t *cname,
                                    IN tt_u32_t cname_len)
{
    tt_adrr_cname_t *rdata;
    tt_char_t *new_cname;

    TT_ASSERT(rr->type == TT_ADNS_RR_CNAME_IN);
    rdata = TT_ADRR_CAST(rr, tt_adrr_cname_t);

    if ((cname != NULL) && (cname_len == 0)) {
        cname_len = (tt_u32_t)tt_strlen(cname) + 1;
    }

    new_cname = (tt_char_t *)tt_mem_alloc(cname_len);
    if (new_cname == NULL) {
        TT_ERROR("fail to alloc new cname");
        return TT_FAIL;
    }
    tt_memcpy(new_cname, cname, cname_len);

    if (rdata->cname != NULL) {
        tt_free(rdata->cname);
    }
    rdata->cname = new_cname;
    rdata->cname_len = cname_len;

    return TT_SUCCESS;
}

void __cname_destroy(IN struct tt_adns_rr_s *rr)
{
    tt_adrr_cname_t *rdata = TT_ADRR_CAST(rr, tt_adrr_cname_t);

    if (rdata->cname != NULL) {
        tt_mem_free(rdata->cname);
    }
}

void __cname_dump(IN struct tt_adns_rr_s *rr)
{
    tt_adrr_cname_t *rdata = TT_ADRR_CAST(rr, tt_adrr_cname_t);

    TT_INFO("- domain: %s", rr->name);
    TT_INFO("  type clase: CNAME IN");
    TT_INFO("  cname: %s", rdata->cname);
}

tt_result_t __cname_render_prepare(IN struct tt_adns_rr_s *rr,
                                   OUT tt_u32_t *len)
{
    tt_adrr_cname_t *rdata = TT_ADRR_CAST(rr, tt_adrr_cname_t);
    tt_u32_t __len;

    TT_DO(
        tt_adns_name_render_prepare(rdata->cname, rdata->cname_len, 0, &__len));

    *len = __len;
    return TT_SUCCESS;
}

tt_result_t __cname_render(IN struct tt_adns_rr_s *rr, OUT struct tt_buf_s *buf)
{
    tt_adrr_cname_t *rdata = TT_ADRR_CAST(rr, tt_adrr_cname_t);

    // rdata
    TT_DO(tt_adns_name_render(buf, rdata->cname, rdata->cname_len, 0));

    return TT_SUCCESS;
}

tt_result_t __cname_parse(OUT struct tt_adns_rr_s *rr,
                          IN struct tt_buf_s *buf,
                          IN tt_u8_t *pkt,
                          IN tt_u32_t pkt_len)
{
    tt_adrr_cname_t *rdata = TT_ADRR_CAST(rr, tt_adrr_cname_t);
    tt_char_t *cname;
    tt_u32_t cname_len;

    // rdata
    cname = tt_adns_name_parse(buf, NULL, &cname_len, pkt, pkt_len);
    if (cname == NULL) {
        return TT_FAIL;
    }

    if (rdata->cname != NULL) {
        tt_mem_free(rdata->cname);
    }
    rdata->cname = cname;
    rdata->cname_len = cname_len;

    return TT_SUCCESS;
}
