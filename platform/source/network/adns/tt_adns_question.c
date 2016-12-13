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

#include <network/adns/tt_adns_question.h>

#include <algorithm/tt_buffer.h>
#include <network/adns/tt_adns_domain_name.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

extern tt_result_t __adrr_render_type(IN tt_buf_t *ds,
                                      IN tt_adns_rr_type_t type);
extern tt_result_t __adrr_parse_type(IN tt_buf_t *ds,
                                     OUT tt_adns_rr_type_t *type);

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_adns_rr_itf_t __quest_itf = {NULL,
                                       NULL,
                                       NULL,
                                       NULL,

                                       NULL,
                                       NULL,
                                       NULL};

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

struct tt_adns_rr_s *tt_adns_quest_create(IN const tt_char_t *name,
                                          IN tt_u32_t name_ownership,
                                          IN tt_adns_rr_type_t type)
{
    tt_adns_rr_t *rr;

    rr = tt_adns_rr_create(0, &__quest_itf, name, name_ownership, type, 0);
    if (rr == NULL) {
        return NULL;
    }

    rr->is_quest = TT_TRUE;

    return rr;
}

tt_result_t tt_adns_quest_render_prepare(IN struct tt_adns_rr_s *rr,
                                         OUT tt_u32_t *len)
{
    tt_result_t result;
    tt_u32_t __len;

    /*
     0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                                               |
     /                     QNAME                     /
     /                                               /
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                     QTYPE                     |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                     QCLASS                    |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     */

    TT_ASSERT(rr->is_quest);

    result = tt_adns_name_render_prepare(rr->name, rr->name_len, 0, &__len);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    *len = __len + 4;
    return TT_SUCCESS;
}

tt_result_t tt_adns_quest_render(IN struct tt_adns_rr_s *rr,
                                 OUT struct tt_buf_s *buf)
{
    TT_ASSERT(rr->is_quest);

    // qname
    TT_DO(tt_adns_name_render(buf, rr->name, rr->name_len, 0));

    // qtype, qclass
    TT_DO(__adrr_render_type(buf, rr->type));

    return TT_SUCCESS;
}

tt_result_t tt_adns_quest_parse_prepare(IN struct tt_buf_s *buf)
{
    // qname
    TT_DO(tt_adns_name_parse_prepare(buf));

    // type, class
    TT_DO(tt_buf_inc_rp(buf, 4));

    return TT_SUCCESS;
}

struct tt_adns_rr_s *tt_adns_quest_parse(IN struct tt_buf_s *buf,
                                         IN tt_u8_t *pkt,
                                         IN tt_u32_t pkt_len)
{
    const tt_char_t *name;
    tt_u32_t name_len;
    tt_adns_rr_type_t type;

    // name
    name = tt_adns_name_parse(buf, NULL, &name_len, pkt, pkt_len);
    if (name == NULL) {
        return NULL;
    }
    // TT_ASSERT(name_len != 0);

    // type
    TT_DO_R(NULL, __adrr_parse_type(buf, &type));

    return tt_adns_quest_create(name, TT_ADNS_RR_TAKE_NAME, type);
}
