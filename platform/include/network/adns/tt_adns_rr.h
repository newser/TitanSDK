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
@file tt_adns_rr.h
@brief async dns resource record

this file defines async dns resource record
*/

#ifndef __TT_ADNS_RR__
#define __TT_ADNS_RR__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_double_linked_list.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_ADRR_CAST(rr, type) TT_PTR_INC(type, rr, sizeof(tt_adns_rr_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_adns_rr_s;
struct tt_buf_s;

typedef tt_result_t (*tt_adns_rr_create_t)(IN struct tt_adns_rr_s *rr);

typedef void (*tt_adns_rr_destroy_t)(IN struct tt_adns_rr_s *rr);

typedef tt_result_t (*tt_adns_rr_copy_t)(IN struct tt_adns_rr_s *dst,
                                         IN struct tt_adns_rr_s *src);

typedef void (*tt_adns_rr_dump_t)(IN struct tt_adns_rr_s *rr);

// only count bytes of rdata
typedef tt_result_t (*tt_adns_rr_render_prepare_t)(IN struct tt_adns_rr_s *rr,
                                                   OUT tt_u32_t *len);

// only render rdata
typedef tt_result_t (*tt_adns_rr_render_t)(IN struct tt_adns_rr_s *rr,
                                           OUT struct tt_buf_s *buf);

// must parse rdlength and rdata
typedef tt_result_t (*tt_adns_rr_parse_t)(OUT struct tt_adns_rr_s *rr,
                                          IN struct tt_buf_s *buf,
                                          IN tt_u8_t *pkt,
                                          IN tt_u32_t pkt_len);

typedef struct
{
    // if null, all are set to 0
    tt_adns_rr_create_t create;
    tt_adns_rr_destroy_t destroy;
    // if null, directly copy bytes
    tt_adns_rr_copy_t copy;
    tt_adns_rr_dump_t dump;

    tt_adns_rr_render_prepare_t render_prepare;
    tt_adns_rr_render_t render;
    tt_adns_rr_parse_t parse;
} tt_adns_rr_itf_t;

typedef enum {
    TT_ADNS_RR_A_IN,
    TT_ADNS_RR_AAAA_IN,
    TT_ADNS_RR_SRV_IN,
    TT_ADNS_RR_CNAME_IN,

    TT_ADNS_RR_TYPE_NUM,
} tt_adns_rr_type_t;
#define TT_ADNS_RR_TYPE_VALID(t) ((t) < TT_ADNS_RR_TYPE_NUM)

typedef struct tt_adns_rr_s
{
    tt_dnode_t node;
    tt_adns_rr_itf_t *itf;
    tt_u32_t rr_len;

    tt_bool_t name_owner : 1;
    tt_bool_t is_quest : 1;

    /*
     0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                                               |
     /                                               /
     /                      NAME                     /
     |                                               |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                      TYPE                     |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                     CLASS                     |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                      TTL                      |
     |                                               |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     |                   RDLENGTH                    |
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
     /                     RDATA                     /
     /                                               /
     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     */

    const tt_char_t *name; // NAME
    tt_u32_t name_len; // including term null
    tt_adns_rr_type_t type; // TYPE CLASS
    tt_u32_t ttl; // TTL
    tt_u16_t rdlength; // RDLENGTH
} tt_adns_rr_t;

typedef struct tt_adrr_filter_s
{
    tt_adns_rr_type_t type;

    const tt_char_t *name;
    tt_u32_t name_len;

    tt_bool_t has_type : 1;
    tt_bool_t has_name : 1;
} tt_adrr_filter_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_adns_rr_t *tt_adns_rr_create(IN tt_u32_t extra_size,
                                       IN tt_adns_rr_itf_t *itf,
                                       IN const tt_char_t *name,
                                       IN tt_u32_t name_ownership,
                                       IN tt_adns_rr_type_t type,
                                       IN tt_u32_t ttl);
// name_ownership
#define TT_ADNS_RR_COPY_NAME (0)
#define TT_ADNS_RR_TAKE_NAME (1)
#define TT_ADNS_RR_REF_NAME (2)

extern void tt_adns_rr_destroy(IN tt_adns_rr_t *rr);

// name_len includes terminating null
extern tt_result_t tt_adns_rr_set_name(IN tt_adns_rr_t *rr,
                                       IN const tt_char_t *name,
                                       IN tt_u32_t name_len,
                                       IN tt_u32_t name_ownership);

// - the new rr does not belong to any dlist whether rr is
//   already in some dlist,
extern tt_adns_rr_t *tt_adns_rr_copy(IN tt_adns_rr_t *rr);

// ========================================
// rr list
// ========================================

// - this function will destroy dst first
// - this function return success if src is empty
extern tt_result_t tt_adns_rrlist_copy(IN tt_dlist_t *dst,
                                       IN tt_dlist_t *src,
                                       IN tt_u32_t flag);
#define TT_ADNS_RRLIST_COPY_ALL (1 << 0)

extern void tt_adns_rrlist_destroy(IN tt_dlist_t *rrlist);

tt_inline void tt_adns_rrlist_clear(IN tt_dlist_t *rrlist)
{
    tt_adns_rrlist_destroy(rrlist);
    tt_dlist_init(rrlist);
}

extern tt_result_t tt_adns_rrlist_set_name(IN tt_dlist_t *rrlist,
                                           IN const tt_char_t *name,
                                           IN tt_u32_t name_len,
                                           IN tt_u32_t name_ownership);

extern void tt_adns_rrlist_dump(IN tt_dlist_t *rrlist);

extern void tt_adns_rrlist_filter(IN tt_dlist_t *in_rrlist,
                                  IN tt_adrr_filter_t *filter,
                                  OUT tt_dlist_t *out_rrlist);
// flag
#define TT_ADNS_RRLIST_FILTER_COPY (1 << 0)

#define tt_adns_rrlist_filter_t(in_rrlist, t, out_rrlist)                      \
    do {                                                                       \
        tt_adrr_filter_t f = {0};                                              \
                                                                               \
        f.type = (t);                                                          \
        f.has_type = TT_TRUE;                                                  \
                                                                               \
        tt_adns_rrlist_filter((in_rrlist), &f, (out_rrlist));                  \
    } while (0)

#define tt_adns_rrlist_filter_n(in_rrlist, n, n_len, out_rrlist)               \
    do {                                                                       \
        tt_adrr_filter_t f = {0};                                              \
                                                                               \
        f.name = (n);                                                          \
        f.name_len = (n_len);                                                  \
        f.has_name = TT_TRUE;                                                  \
                                                                               \
        tt_adns_rrlist_filter((in_rrlist), &f, (out_rrlist));                  \
    } while (0)

#define tt_adns_rrlist_filter_tn(in_rrlist, t, n, n_len, out_rrlist)           \
    do {                                                                       \
        tt_adrr_filter_t f = {0};                                              \
                                                                               \
        f.type = (t);                                                          \
        f.has_type = TT_TRUE;                                                  \
                                                                               \
        f.name = (n);                                                          \
        f.name_len = (n_len);                                                  \
        f.has_name = TT_TRUE;                                                  \
                                                                               \
        tt_adns_rrlist_filter((in_rrlist), &f, (out_rrlist));                  \
    } while (0)

// ========================================
// render & parse
// ========================================

extern tt_result_t tt_adns_rr_render_prepare(IN tt_adns_rr_t *rr,
                                             OUT tt_u32_t *len);

extern tt_result_t tt_adns_rr_render(IN tt_adns_rr_t *rr,
                                     OUT struct tt_buf_s *buf);

extern tt_result_t tt_adns_rr_parse_prepare(IN struct tt_buf_s *buf);

extern tt_adns_rr_t *tt_adns_rr_parse(IN struct tt_buf_s *buf,
                                      IN tt_u8_t *pkt,
                                      IN tt_u32_t pkt_len);

#endif /* __TT_ADNS_RR__ */
