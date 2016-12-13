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

#include <network/adns/tt_adns_packet.h>

#include <algorithm/tt_buffer_format.h>
#include <memory/tt_memory_alloc.h>
#include <network/adns/tt_adns_question.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __MAX_QD_NUM 100
#define __MAX_AN_NUM 100
#define __MAX_NS_NUM 100
#define __MAX_AR_NUM 100

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

#if 1

tt_atomic_s32_t tt_g_adns_pkt_num;
#define __ADNS_PKT_NUM_INC() tt_atomic_s32_inc(&tt_g_adns_pkt_num)
#define __ADNS_PKT_NUM_DEC() tt_atomic_s32_dec(&tt_g_adns_pkt_num)

#else

#define __ADNS_RR_NUM_INC()
#define __ADNS_RR_NUM_DEC()

#endif

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_u32_t __parse_raw_flag(IN tt_u32_t raw_flag,
                                 OUT tt_u8_t *opcode,
                                 OUT tt_u8_t *rcode);
static tt_u16_t __render_raw_flag(IN tt_u32_t flag,
                                  OUT tt_u8_t opcode,
                                  OUT tt_u8_t rcode);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_adns_pkt_t *tt_adns_pkt_create(IN tt_u16_t __id,
                                  IN tt_u8_t opcode,
                                  IN tt_u8_t rcode,
                                  IN tt_u32_t flag)
{
    tt_adns_pkt_t *pkt;
    tt_buf_attr_t buf_attr;

    pkt = (tt_adns_pkt_t *)tt_mem_alloc(sizeof(tt_adns_pkt_t));
    if (pkt == NULL) {
        TT_ERROR("no mem for adns pkt");
        return NULL;
    }

    tt_buf_attr_default(&buf_attr);
    buf_attr.min_expand_order = 9; // 512B
    buf_attr.max_expand_order = 10; // 1K
    buf_attr.max_size_order = 16; // 64K
    // 512 -> 1k -> 2k -> 3k, ...

    if (!TT_OK(tt_buf_create(&pkt->buf, 0, &buf_attr))) {
        TT_ERROR("fail to create adns pkt buf");

        tt_mem_free(pkt);
        return NULL;
    }

    // add ref: creator
    tt_atomic_s32_init(&pkt->ref, 1);

    pkt->__id = __id;
    pkt->opcode = opcode;
    pkt->rcode = rcode;
    pkt->flag = flag;

    pkt->question_num = 0;
    tt_dlist_init(&pkt->question);

    pkt->answer_num = 0;
    tt_dlist_init(&pkt->answer);

    pkt->authority_num = 0;
    tt_dlist_init(&pkt->authority);

    pkt->additional_num = 0;
    tt_dlist_init(&pkt->additional);

    __ADNS_PKT_NUM_INC();
    return pkt;
}

void __adns_pkt_destroy(IN tt_adns_pkt_t *pkt)
{
    tt_adns_rrlist_destroy(&pkt->question);
    tt_adns_rrlist_destroy(&pkt->answer);
    tt_adns_rrlist_destroy(&pkt->authority);
    tt_adns_rrlist_destroy(&pkt->additional);

    tt_buf_destroy(&pkt->buf);

    tt_mem_free(pkt);

    __ADNS_PKT_NUM_DEC();
}

tt_result_t tt_adns_pkt_generate(IN tt_adns_pkt_t *pkt)
{
    tt_u32_t len;
    tt_buf_t *buf = &pkt->buf;

    if (!TT_OK(tt_adns_pkt_render_prepare(pkt, &len))) {
        return TT_FAIL;
    }

    tt_buf_reset_rwp(buf);
    if (!TT_OK(tt_buf_reserve(buf, len))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_adns_pkt_render(pkt, buf))) {
        tt_buf_reset_rwp(buf);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_adns_pkt_render_prepare(IN tt_adns_pkt_t *pkt, OUT tt_u32_t *len)
{
    tt_u32_t total_len = 12, n; // header has 12 bytes
    tt_dnode_t *node;

    // question
    node = tt_dlist_head(&pkt->question);
    while (node != NULL) {
        tt_adns_rr_t *rr;

        rr = TT_CONTAINER(node, tt_adns_rr_t, node);
        node = tt_dlist_next(&pkt->question, node);

        if (!TT_OK(tt_adns_quest_render_prepare(rr, &n))) {
            return TT_FAIL;
        }
        total_len += n;
    }

    // answer
    node = tt_dlist_head(&pkt->answer);
    while (node != NULL) {
        tt_adns_rr_t *rr;

        rr = TT_CONTAINER(node, tt_adns_rr_t, node);
        node = tt_dlist_next(&pkt->answer, node);

        if (!TT_OK(tt_adns_rr_render_prepare(rr, &n))) {
            return TT_FAIL;
        }
        total_len += n;
    }

    // authority
    node = tt_dlist_head(&pkt->authority);
    while (node != NULL) {
        tt_adns_rr_t *rr;

        rr = TT_CONTAINER(node, tt_adns_rr_t, node);
        node = tt_dlist_next(&pkt->authority, node);

        if (!TT_OK(tt_adns_rr_render_prepare(rr, &n))) {
            return TT_FAIL;
        }
        total_len += n;
    }

    // additional
    node = tt_dlist_head(&pkt->additional);
    while (node != NULL) {
        tt_adns_rr_t *rr;

        rr = TT_CONTAINER(node, tt_adns_rr_t, node);
        node = tt_dlist_next(&pkt->additional, node);

        if (!TT_OK(tt_adns_rr_render_prepare(rr, &n))) {
            return TT_FAIL;
        }
        total_len += n;
    }

    *len = total_len;
    return TT_SUCCESS;
}

tt_result_t tt_adns_pkt_render(IN tt_adns_pkt_t *pkt, OUT tt_buf_t *buf)
{
    tt_u16_t raw_flag;
    tt_dnode_t *node;

    // id
    TT_DO(tt_buf_put_u16_n(buf, pkt->__id));

    // flags
    raw_flag = __render_raw_flag(pkt->flag, pkt->opcode, pkt->rcode);
    TT_DO(tt_buf_put_u16(buf, raw_flag));

    // counts
    TT_DO(tt_buf_put_u16_n(buf, pkt->question_num));
    TT_DO(tt_buf_put_u16_n(buf, pkt->answer_num));
    TT_DO(tt_buf_put_u16_n(buf, pkt->authority_num));
    TT_DO(tt_buf_put_u16_n(buf, pkt->additional_num));

    // question
    node = tt_dlist_head(&pkt->question);
    while (node != NULL) {
        tt_adns_rr_t *rr;

        rr = TT_CONTAINER(node, tt_adns_rr_t, node);
        node = tt_dlist_next(&pkt->question, node);

        TT_DO(tt_adns_quest_render(rr, buf));
    }

    // answer
    node = tt_dlist_head(&pkt->answer);
    while (node != NULL) {
        tt_adns_rr_t *rr;

        rr = TT_CONTAINER(node, tt_adns_rr_t, node);
        node = tt_dlist_next(&pkt->answer, node);

        TT_DO(tt_adns_rr_render(rr, buf));
    }

    // authority
    node = tt_dlist_head(&pkt->authority);
    while (node != NULL) {
        tt_adns_rr_t *rr;

        rr = TT_CONTAINER(node, tt_adns_rr_t, node);
        node = tt_dlist_next(&pkt->authority, node);

        TT_DO(tt_adns_rr_render(rr, buf));
    }

    // additional
    node = tt_dlist_head(&pkt->additional);
    while (node != NULL) {
        tt_adns_rr_t *rr;

        rr = TT_CONTAINER(node, tt_adns_rr_t, node);
        node = tt_dlist_next(&pkt->additional, node);

        TT_DO(tt_adns_rr_render(rr, buf));
    }

    return TT_SUCCESS;
}

tt_result_t tt_adns_pkt_parse_prepare(IN tt_buf_t *buf)
{
    tt_u16_t qdcount;
    tt_u16_t ancount;
    tt_u16_t nscount;
    tt_u16_t arcount;
    tt_u16_t i;

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

    // id, flags
    TT_DO(tt_buf_inc_rp(buf, 4));

    TT_DO(tt_buf_get_u16_h(buf, &qdcount));
    if (qdcount > __MAX_QD_NUM) {
        TT_ERROR("qdcount[%d] exceeds limit[%d]", qdcount, __MAX_QD_NUM);
        return TT_FAIL;
    }
    TT_DETAIL("qdcount: %d", qdcount);

    TT_DO(tt_buf_get_u16_h(buf, &ancount));
    if (ancount > __MAX_AN_NUM) {
        TT_ERROR("ancount[%d] exceeds limit[%d]", ancount, __MAX_AN_NUM);
        return TT_FAIL;
    }
    TT_DETAIL("ancount: %d", ancount);

    TT_DO(tt_buf_get_u16_h(buf, &nscount));
    if (nscount > __MAX_NS_NUM) {
        TT_ERROR("nscount[%d] exceeds limit[%d]", nscount, __MAX_NS_NUM);
        return TT_FAIL;
    }
    TT_DETAIL("nscount: %d", nscount);

    TT_DO(tt_buf_get_u16_h(buf, &arcount));
    if (arcount > __MAX_AR_NUM) {
        TT_ERROR("arcount[%d] exceeds limit[%d]", arcount, __MAX_AR_NUM);
        return TT_FAIL;
    }
    TT_DETAIL("arcount: %d", arcount);

    // qdcount
    for (i = 0; i < qdcount; ++i) {
        TT_DO(tt_adns_quest_parse_prepare(buf));
    }

    // ancount
    for (i = 0; i < ancount; ++i) {
        TT_DO(tt_adns_rr_parse_prepare(buf));
    }

    // nscount
    for (i = 0; i < nscount; ++i) {
        TT_DO(tt_adns_rr_parse_prepare(buf));
    }

    // arcount
    for (i = 0; i < arcount; ++i) {
        TT_DO(tt_adns_rr_parse_prepare(buf));
    }

    return TT_SUCCESS;
}

tt_adns_pkt_t *tt_adns_pkt_parse(IN tt_buf_t *buf, IN tt_u32_t parse_flag)
{
    tt_adns_pkt_t *pkt;
    tt_u16_t i;
    tt_blob_t pkt_data;
    tt_u32_t brp, bwp;

    tt_u16_t __id;
    tt_u8_t opcode;
    tt_u8_t rcode;
    tt_u16_t raw_flag;
    tt_u32_t flag;
    tt_u16_t qdcount;
    tt_u16_t ancount;
    tt_u16_t nscount;
    tt_u16_t arcount;

    tt_buf_getptr_rpblob(buf, &pkt_data);

    // id
    TT_DO_R(NULL, tt_buf_get_u16_h(buf, &__id));

    // flags
    TT_DO_R(NULL, tt_buf_get_u16(buf, &raw_flag));
    flag = __parse_raw_flag(raw_flag, &opcode, &rcode);

    // create pkt
    pkt = tt_adns_pkt_create(__id, opcode, rcode, flag);
    if (pkt == NULL) {
        return NULL;
    }
    // later should use TT_DO_G to release pkt when
    // some function failed

    // counts
    TT_DO_G(__pp_fail, tt_buf_get_u16_h(buf, &qdcount));
    TT_ASSERT(qdcount < __MAX_QD_NUM);

    TT_DO_G(__pp_fail, tt_buf_get_u16_h(buf, &ancount));
    TT_ASSERT(ancount < __MAX_AN_NUM);

    TT_DO_G(__pp_fail, tt_buf_get_u16_h(buf, &nscount));
    TT_ASSERT(nscount < __MAX_NS_NUM);

    TT_DO_G(__pp_fail, tt_buf_get_u16_h(buf, &arcount));
    TT_ASSERT(arcount < __MAX_AR_NUM);

    // qdcount
    for (i = 0; i < qdcount; ++i) {
        if (parse_flag & TT_ADNS_PKT_PARSE_QUESTION) {
            tt_adns_rr_t *question;

            // we allow skipping unrecognized rr, to achieve this:
            //  - back up pos before parsing
            //  - if parsing is ok, then go ahead. otherwise restore pos
            //    and use parse_prepare() to skip the unknown rr
            tt_buf_backup_rwp(buf, &brp, &bwp);
            question = tt_adns_quest_parse(buf, pkt_data.addr, pkt_data.len);
            if (question != NULL) {
                tt_adns_pkt_add_question(pkt, question);
                continue;
            }
            tt_buf_restore_rwp(buf, &brp, &bwp);
        }
        TT_DO_G(__pp_fail, tt_adns_quest_parse_prepare(buf));
    }

    // ancount
    for (i = 0; i < ancount; ++i) {
        if (parse_flag & TT_ADNS_PKT_PARSE_ANSWER) {
            tt_adns_rr_t *rr;

            tt_buf_backup_rwp(buf, &brp, &bwp);
            rr = tt_adns_rr_parse(buf, pkt_data.addr, pkt_data.len);
            if (rr != NULL) {
                tt_adns_pkt_add_answer(pkt, rr);
                continue;
            }
            tt_buf_restore_rwp(buf, &brp, &bwp);
        }
        TT_DO_G(__pp_fail, tt_adns_rr_parse_prepare(buf));
    }

    // nscount
    for (i = 0; i < nscount; ++i) {
        if (parse_flag & TT_ADNS_PKT_PARSE_AUTHORITY) {
            tt_adns_rr_t *rr;

            tt_buf_backup_rwp(buf, &brp, &bwp);
            rr = tt_adns_rr_parse(buf, pkt_data.addr, pkt_data.len);
            if (rr != NULL) {
                tt_adns_pkt_add_authority(pkt, rr);
                continue;
            }
            tt_buf_restore_rwp(buf, &brp, &bwp);
        }
        TT_DO_G(__pp_fail, tt_adns_rr_parse_prepare(buf));
    }

    // arcount
    for (i = 0; i < arcount; ++i) {
        if (parse_flag & TT_ADNS_PKT_PARSE_ADDITIONAL) {
            tt_adns_rr_t *rr;

            tt_buf_backup_rwp(buf, &brp, &bwp);
            rr = tt_adns_rr_parse(buf, pkt_data.addr, pkt_data.len);
            if (rr != NULL) {
                tt_adns_pkt_add_additional(pkt, rr);
                continue;
            }
            tt_buf_restore_rwp(buf, &brp, &bwp);
        }
        TT_DO_G(__pp_fail, tt_adns_rr_parse_prepare(buf));
    }

    return pkt;

__pp_fail:

    if (pkt != NULL) {
        tt_adns_pkt_release(pkt);
    }

    return NULL;
}

tt_u32_t __parse_raw_flag(IN tt_u32_t raw_flag,
                          OUT tt_u8_t *opcode,
                          OUT tt_u8_t *rcode)
{
    tt_u16_t val = tt_ntoh16(raw_flag);
    tt_u32_t flag = 0;

    /*
      15 14 13 12 11 10 9  8  7  6  5  4  3  2  1  0
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     */

    if (val & (1 << 15)) {
        flag |= __ADNS_PKT_RESP;
    }

    *opcode = (tt_u8_t)((val >> 11) & 0xF);

    if (val & (1 << 10)) {
        flag |= __ADNS_PKT_AUTH_ANS;
    }

    if (val & (1 << 8)) {
        flag |= __ADNS_PKT_RECUR_DESIRED;
    }

    if (val & (1 << 7)) {
        flag |= __ADNS_PKT_RECUR_AVAIL;
    }

    *rcode = (tt_u8_t)(val & 0xF);

    return flag;
}

tt_u16_t __render_raw_flag(IN tt_u32_t flag,
                           OUT tt_u8_t opcode,
                           OUT tt_u8_t rcode)
{
    tt_u16_t val = 0;

    /*
      15 14 13 12 11 10 9  8  7  6  5  4  3  2  1  0
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     */

    if (flag & __ADNS_PKT_RESP) {
        val |= (1 << 15);
    }

    val |= (((tt_u16_t)(opcode & 0xF)) << 11);

    if (flag & __ADNS_PKT_AUTH_ANS) {
        val |= (1 << 10);
    }

    if (flag & __ADNS_PKT_RECUR_DESIRED) {
        val |= (1 << 8);
    }

    if (flag & __ADNS_PKT_RECUR_AVAIL) {
        val |= (1 << 7);
    }

    val |= ((tt_u16_t)(rcode & 0xF));

    return tt_hton16(val);
}
