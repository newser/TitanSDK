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
@file tt_adns_packet.h
@brief async dns pkt

this file defines async dns pkt APIs
*/

#ifndef __TT_ADNS_PACKET__
#define __TT_ADNS_PACKET__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <algorithm/tt_double_linked_list.h>
#include <misc/tt_reference_counter.h>
#include <network/adns/tt_adns_rr.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_adns_pkt_s
{
    tt_buf_t buf;
    tt_atomic_s32_t ref;

    // ========================================
    // pkt format
    // ========================================

    /*
     0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                      ID                       |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                    QDCOUNT                    |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                    ANCOUNT                    |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                    NSCOUNT                    |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                    ARCOUNT                    |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     */

    tt_u16_t __id;
    tt_u8_t opcode;
    tt_u8_t rcode;

    tt_u32_t flag;
#define __ADNS_PKT_RESP (1 << 0) // QR
#define __ADNS_PKT_AUTH_ANS (1 << 1) // AA
#define __ADNS_PKT_RECUR_DESIRED (1 << 2) // RD
#define __ADNS_PKT_RECUR_AVAIL (1 << 3) // RA

    tt_u32_t question_num; // QDCOUNT
    tt_u32_t answer_num; // ANCOUNT
    tt_u32_t authority_num; // NSCOUNT
    tt_u32_t additional_num; // ARCOUNT

    tt_dlist_t question;
    tt_dlist_t answer;
    tt_dlist_t authority;
    tt_dlist_t additional;
} tt_adns_pkt_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_adns_pkt_t *tt_adns_pkt_create(IN tt_u16_t __id,
                                         IN tt_u8_t opcode,
                                         IN tt_u8_t rcode,
                                         IN tt_u32_t flag);

tt_export void __adns_pkt_destroy(IN tt_adns_pkt_t *pkt);

#define tt_adns_pkt_ref(p) TT_REF_ADD(tt_adns_pkt_t, p, ref)
#define tt_adns_pkt_release(p)                                                 \
    TT_REF_RELEASE(tt_adns_pkt_t, p, ref, __adns_pkt_destroy)

tt_inline void tt_adns_pkt_add_question(IN tt_adns_pkt_t *pkt,
                                        IN tt_adns_rr_t *rr)
{
    tt_dlist_push_tail(&pkt->question, &rr->node);
    ++pkt->question_num;
}

tt_inline void tt_adns_pkt_add_answer(IN tt_adns_pkt_t *pkt,
                                      IN tt_adns_rr_t *rr)
{
    tt_dlist_push_tail(&pkt->answer, &rr->node);
    ++pkt->answer_num;
}

tt_inline void tt_adns_pkt_add_authority(IN tt_adns_pkt_t *pkt,
                                         IN tt_adns_rr_t *rr)
{
    tt_dlist_push_tail(&pkt->authority, &rr->node);
    ++pkt->authority_num;
}

tt_inline void tt_adns_pkt_add_additional(IN tt_adns_pkt_t *pkt,
                                          IN tt_adns_rr_t *rr)
{
    tt_dlist_push_tail(&pkt->additional, &rr->node);
    ++pkt->additional_num;
}

// ========================================
// render & parse
// ========================================

tt_export tt_result_t tt_adns_pkt_generate(IN tt_adns_pkt_t *pkt);

tt_export tt_result_t tt_adns_pkt_render_prepare(IN tt_adns_pkt_t *pkt,
                                              OUT tt_u32_t *len);
tt_export tt_result_t tt_adns_pkt_render(IN tt_adns_pkt_t *pkt, OUT tt_buf_t *buf);

tt_export tt_result_t tt_adns_pkt_parse_prepare(IN tt_buf_t *data);

tt_export tt_adns_pkt_t *tt_adns_pkt_parse(IN tt_buf_t *buf,
                                        IN tt_u32_t parse_flag);
// parse_flag
#define TT_ADNS_PKT_PARSE_QUESTION (1 << 0)
#define TT_ADNS_PKT_PARSE_ANSWER (1 << 1)
#define TT_ADNS_PKT_PARSE_AUTHORITY (1 << 2)
#define TT_ADNS_PKT_PARSE_ADDITIONAL (1 << 3)
#define TT_ADNS_PKT_PARSE_ALL                                                  \
    (TT_ADNS_PKT_PARSE_QUESTION | TT_ADNS_PKT_PARSE_ANSWER |                   \
     TT_ADNS_PKT_PARSE_AUTHORITY | TT_ADNS_PKT_PARSE_ADDITIONAL)

#endif /* __TT_ADNS_PACKET__ */
