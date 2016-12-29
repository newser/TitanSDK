/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a new_rr of the License at
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

#include <network/adns/tt_adns_rr.h>

#include <algorithm/tt_buffer_format.h>
#include <memory/tt_memory_alloc.h>
#include <network/adns/tt_adns_domain_name.h>
#include <network/adns/tt_adns_rr_a_in.h>
#include <network/adns/tt_adns_rr_aaaa_in.h>
#include <network/adns/tt_adns_rr_cname_in.h>
#include <os/tt_atomic.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

//#define __MAX_RDLENGTH 300

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

enum
{
    __ADR_TYPE_A = 1,
    __ADR_TYPE_AAAA = 28,
    __ADR_TYPE_CNAME = 5,
};

enum
{
    __ADR_CLASS_IN = 1,
};

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

#if 1

tt_atomic_s32_t tt_g_adns_rr_num;
#define __ADRR_NUM_INC() tt_atomic_s32_inc(&tt_g_adns_rr_num)
#define __ADRR_NUM_DEC() tt_atomic_s32_dec(&tt_g_adns_rr_num)

#else

#define __ADRR_NUM_INC()
#define __ADRR_NUM_DEC()

#endif

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __check_adrr_itf(tt_adns_rr_itf_t *itf);

tt_result_t __adrr_render_type(IN tt_buf_t *buf, IN tt_adns_rr_type_t type);
tt_result_t __adrr_parse_type(IN tt_buf_t *buf, OUT tt_adns_rr_type_t *type);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_adns_rr_t *tt_adns_rr_create(IN tt_u32_t extra_size,
                                IN tt_adns_rr_itf_t *itf,
                                IN const tt_char_t *name,
                                IN tt_u32_t name_ownership,
                                IN tt_adns_rr_type_t type,
                                IN tt_u32_t ttl)
{
    tt_adns_rr_t *rr;
    tt_u32_t rr_len = sizeof(tt_adns_rr_t) + extra_size;
    tt_u32_t name_len = 0;

    if (!TT_OK(__check_adrr_itf(itf))) {
        return NULL;
    }

    if (name != NULL) {
        name_len = (tt_u32_t)tt_strnlen(name, TT_MAX_DOMAIN_NAME_LEN);
        if (name_len >= TT_MAX_DOMAIN_NAME_LEN) {
            TT_ERROR("too long domain name[%d]", name_len);
            return NULL;
        }
        // ending null
        name_len += 1;
    }

    // alloc
    rr = (tt_adns_rr_t *)tt_malloc(rr_len);
    if (rr == NULL) {
        TT_ERROR("no mem for adns rr");
        return NULL;
    }
    tt_memset(rr, 0, rr_len);

    // init
    tt_dnode_init(&rr->node);
    rr->itf = itf;
    rr->rr_len = rr_len;

    if (name_ownership == TT_ADNS_RR_TAKE_NAME) {
        rr->name = name;
        rr->name_len = name_len;
        rr->name_owner = TT_TRUE;
    } else if (name_ownership == TT_ADNS_RR_REF_NAME) {
        rr->name = name;
        rr->name_len = name_len;
        rr->name_owner = TT_FALSE;
    } else {
        TT_ASSERT(name_ownership == TT_ADNS_RR_COPY_NAME);
        TT_ASSERT(name != NULL);

        rr->name = (tt_char_t *)tt_malloc(name_len);
        if (rr->name == NULL) {
            TT_ERROR("no memory for rr name");

            tt_free(rr);
            return NULL;
        }
        tt_memcpy((void *)rr->name, name, name_len);

        rr->name_len = name_len;
        rr->name_owner = TT_TRUE;
    }

    rr->type = type;
    rr->ttl = ttl;

    rr->rdlength = 0;

    if ((rr->itf->create != NULL) && !TT_OK(rr->itf->create(rr))) {
        if (rr->name_owner) {
            tt_free((void *)rr->name);
        }
        tt_c_free(rr);
        return NULL;
    }

    __ADRR_NUM_INC();
    return rr;
}

void tt_adns_rr_destroy(IN tt_adns_rr_t *rr)
{
    if (rr->itf->destroy != NULL) {
        rr->itf->destroy(rr);
    }

    if (rr->name_owner) {
        tt_free((void *)rr->name);
    }

    tt_free(rr);

    __ADRR_NUM_DEC();
}

tt_result_t tt_adns_rr_set_name(IN tt_adns_rr_t *rr,
                                IN const tt_char_t *name,
                                IN tt_u32_t name_len,
                                IN tt_u32_t name_ownership)
{
    TT_ASSERT(rr != NULL);

    // allow 0 name length?
    if ((name != NULL) && (name_len == 0)) {
        name_len = (tt_u32_t)tt_strnlen(name, TT_MAX_DOMAIN_NAME_LEN);
        if (name_len >= TT_MAX_DOMAIN_NAME_LEN) {
            TT_ERROR("too long domain name[%d]", name_len);
            return TT_FAIL;
        }
        // ending null
        name_len += 1;
    }

    // alloc new name before freeing original name to avoid
    // partial status(null name pointer)
    if (name_ownership == TT_ADNS_RR_TAKE_NAME) {
        if (rr->name_owner) {
            TT_ASSERT(rr->name != NULL);
            tt_free((void *)rr->name);
        }

        rr->name = name;
        rr->name_len = name_len;
        rr->name_owner = TT_TRUE;

        return TT_SUCCESS;
    } else if (name_ownership == TT_ADNS_RR_REF_NAME) {
        if (rr->name_owner) {
            TT_ASSERT(rr->name != NULL);
            tt_free((void *)rr->name);
        }

        rr->name = name;
        rr->name_len = name_len;
        rr->name_owner = TT_FALSE;

        return TT_SUCCESS;
    } else {
        tt_char_t *new_name;

        TT_ASSERT(name_ownership == TT_ADNS_RR_COPY_NAME);
        TT_ASSERT(name != NULL);

        new_name = (tt_char_t *)tt_malloc(name_len);
        if (new_name == NULL) {
            TT_ERROR("no mem for new adns rr name");
            return TT_FAIL;
        }

        if (rr->name_owner) {
            TT_ASSERT(rr->name != NULL);
            tt_free((void *)rr->name);
        }

        tt_memcpy(new_name, name, name_len);
        rr->name = new_name;
        rr->name_len = name_len;
        rr->name_owner = TT_TRUE;

        return TT_SUCCESS;
    }
}

tt_adns_rr_t *tt_adns_rr_copy(IN tt_adns_rr_t *rr)
{
    tt_adns_rr_t *new_rr;

    TT_ASSERT(rr != NULL);

    new_rr = (tt_adns_rr_t *)tt_malloc(rr->rr_len);
    if (new_rr == NULL) {
        TT_ERROR("no mem for copying adns rr");
        return NULL;
    }
    tt_memcpy(new_rr, rr, rr->rr_len);

    // deep copy
    tt_dnode_init(&new_rr->node);

    if (rr->name_owner) {
        TT_ASSERT(new_rr->name_owner);

        new_rr->name = (tt_char_t *)tt_malloc(rr->name_len);
        if (new_rr->name == NULL) {
            TT_ERROR("no mem for copying adns rr name");

            tt_free(new_rr);
            return NULL;
        }
        tt_memcpy((tt_char_t *)new_rr->name, rr->name, rr->name_len);
    }

    if ((new_rr->itf->copy != NULL) && !TT_OK(new_rr->itf->copy(new_rr, rr))) {
        if (new_rr->name_owner) {
            tt_free((void *)new_rr->name);
        }
        tt_free(new_rr);
        return NULL;
    }

    __ADRR_NUM_INC();
    return new_rr;
}

// ========================================
// rr list
// ========================================

tt_result_t tt_adns_rrlist_copy(IN tt_dlist_t *dst,
                                IN tt_dlist_t *src,
                                IN tt_u32_t flag)
{
    tt_adns_rr_t *new_rr;

    TT_ASSERT(dst != NULL);
    TT_ASSERT(src != NULL);

    if (tt_dlist_empty(src)) {
        tt_adns_rrlist_clear(dst);
        return TT_SUCCESS;
    }

    if (flag & TT_ADNS_RRLIST_COPY_ALL) {
        tt_dlist_t tmp_list;
        tt_dnode_t *cur_node;
        tt_adns_rr_t *cur_rr;

        tt_dlist_init(&tmp_list);

        cur_node = tt_dlist_head(src);
        while (cur_node != NULL) {
            cur_rr = TT_CONTAINER(cur_node, tt_adns_rr_t, node);
            cur_node = tt_dlist_next(src, cur_node);

            new_rr = tt_adns_rr_copy(cur_rr);
            if (new_rr == NULL) {
                TT_ERROR("fail to copy adns rr");

                tt_adns_rrlist_destroy(&tmp_list);
                return TT_FAIL;
            }
            tt_dlist_push_tail(&tmp_list, &new_rr->node);
        }

        tt_adns_rrlist_clear(dst);
        tt_dlist_move(dst, &tmp_list);
        return TT_SUCCESS;
    } else {
        tt_dnode_t *cur_node = tt_dlist_head(src);
        tt_adns_rr_t *cur_rr = TT_CONTAINER(cur_node, tt_adns_rr_t, node);

        new_rr = tt_adns_rr_copy(cur_rr);
        if (new_rr != NULL) {
            tt_adns_rrlist_clear(dst);
            tt_dlist_push_tail(dst, &new_rr->node);
            return TT_SUCCESS;
        } else {
            return TT_FAIL;
        }
    }
}

void tt_adns_rrlist_destroy(IN tt_dlist_t *rrlist)
{
    tt_dnode_t *cur_node;

    TT_ASSERT(rrlist != NULL);

    while ((cur_node = tt_dlist_pop_head(rrlist)) != NULL) {
        tt_adns_rr_destroy(TT_CONTAINER(cur_node, tt_adns_rr_t, node));
    }
}

tt_result_t tt_adns_rrlist_set_name(IN tt_dlist_t *rrlist,
                                    IN const tt_char_t *name,
                                    IN tt_u32_t name_len,
                                    IN tt_u32_t name_ownership)
{
    tt_dnode_t *cur_node;

    cur_node = tt_dlist_head(rrlist);
    while (cur_node != NULL) {
        tt_adns_rr_t *cur_rr;

        cur_rr = TT_CONTAINER(cur_node, tt_adns_rr_t, node);
        cur_node = tt_dlist_next(rrlist, cur_node);

        if (!TT_OK(
                tt_adns_rr_set_name(cur_rr, name, name_len, name_ownership))) {
            TT_FATAL("fail to set adns rr name in list");
            // keep it in the rr list, we do not know how to handle it
        }
    }

    return TT_SUCCESS;
}

void tt_adns_rrlist_dump(IN tt_dlist_t *rrlist)
{
    tt_dnode_t *cur_node;

    cur_node = tt_dlist_head(rrlist);
    while (cur_node != NULL) {
        tt_adns_rr_t *cur_rr;

        cur_rr = TT_CONTAINER(cur_node, tt_adns_rr_t, node);
        cur_node = tt_dlist_next(rrlist, cur_node);

        if (cur_rr->itf->dump != NULL) {
            cur_rr->itf->dump(cur_rr);
        } else {
            TT_INFO("- dump interface is not implemented");
        }
    }
}

void tt_adns_rrlist_filter(IN tt_dlist_t *in_rrlist,
                           IN tt_adrr_filter_t *filter,
                           OUT tt_dlist_t *out_rrlist)
{
    tt_dnode_t *node;

    TT_ASSERT(in_rrlist != out_rrlist);

    node = tt_dlist_head(in_rrlist);
    while (node != NULL) {
        tt_adns_rr_t *rr = TT_CONTAINER(node, tt_adns_rr_t, node);
        node = tt_dlist_next(in_rrlist, node);

        if (filter->has_name &&
            ((filter->name_len != rr->name_len) ||
             (tt_strncmp(filter->name, rr->name, rr->name_len) != 0))) {
            // not match
            continue;
        }

        if (filter->has_type && (filter->type != rr->type)) {
            // not match
            continue;
        }

        tt_dlist_remove(&rr->node);
        tt_dlist_push_tail(out_rrlist, &rr->node);
    }
}

// ========================================
// render & parse
// ========================================

tt_result_t tt_adns_rr_render_prepare(IN tt_adns_rr_t *rr, OUT tt_u32_t *len)
{
    tt_result_t result;
    tt_u32_t __len, n;

    // name: name_len
    // type: 2
    // class: 2
    // ttl: 4
    // rdlength: 2
    __len = 10;

    // name
    result = tt_adns_name_render_prepare(rr->name, rr->name_len, 0, &n);
    if (!TT_OK(result)) {
        return TT_FAIL;
    }
    __len += n;

    // rdata
    if ((rr->itf->render_prepare != NULL) &&
        !TT_OK(rr->itf->render_prepare(rr, &n))) {
        return TT_FAIL;
    }
    rr->rdlength = n;
    __len += n;

    *len = __len;
    return TT_SUCCESS;
}

tt_result_t tt_adns_rr_render(IN tt_adns_rr_t *rr, OUT struct tt_buf_s *buf)
{
    // name
    TT_DO(tt_adns_name_render(buf, rr->name, rr->name_len, 0));

    // type & class
    TT_DO(__adrr_render_type(buf, rr->type));

    // ttl
    TT_DO(tt_buf_put_u32_n(buf, rr->ttl));

    // rdlength
    TT_DO(tt_buf_put_u16_n(buf, rr->rdlength));

    // rdata
    if (rr->itf->render != NULL) {
        TT_DO(rr->itf->render(rr, buf));
    }

    return TT_SUCCESS;
}

tt_result_t tt_adns_rr_parse_prepare(IN tt_buf_t *buf)
{
    tt_u16_t raw_rdlength;

    // name
    TT_DO(tt_adns_name_parse_prepare(buf));

    // type, class, ttl
    TT_DO(tt_buf_inc_rp(buf, 8));

    // rdlength
    TT_DO(tt_buf_get_u16_h(buf, &raw_rdlength));
    if (raw_rdlength > 512) {
        TT_ERROR("too long rdlength: %d, limit: %d", raw_rdlength, 512);
        return TT_FAIL;
    }

    // check rdlength
    TT_DO(tt_buf_inc_rp(buf, raw_rdlength));

    return TT_SUCCESS;
}

tt_adns_rr_t *tt_adns_rr_parse(IN struct tt_buf_s *buf,
                               IN tt_u8_t *pkt,
                               IN tt_u32_t pkt_len)
{
    const tt_char_t *name;
    tt_u32_t name_len;
    tt_adns_rr_type_t type;
    tt_u32_t ttl;
    tt_u16_t rdlength;

    tt_adns_rr_t *rr = NULL;

    // name
    name = tt_adns_name_parse(buf, NULL, &name_len, pkt, pkt_len);
    if (name == NULL) {
        return NULL;
    }
    // TT_ASSERT(name_len != 0);

    // type
    TT_DO_R(NULL, __adrr_parse_type(buf, &type));

    // ttl
    TT_DO_R(NULL, tt_buf_get_u32_h(buf, &ttl));

    // rdlength
    TT_DO_R(NULL, tt_buf_get_u16_h(buf, &rdlength));

    // create rr and parse rdata
    switch (type) {
        case TT_ADNS_RR_A_IN: {
            rr = tt_adrr_a_create(name, TT_ADNS_RR_TAKE_NAME, ttl, NULL);
        } break;
        case TT_ADNS_RR_AAAA_IN: {
            rr = tt_adrr_aaaa_create(name, TT_ADNS_RR_TAKE_NAME, ttl, NULL);
        } break;
        case TT_ADNS_RR_CNAME_IN: {
            rr = tt_adrr_cname_create(name, TT_ADNS_RR_TAKE_NAME, ttl, NULL);
        } break;

        default: {
            TT_ERROR("unknown rr type: %d", type);
        } break;
    }
    if (rr == NULL) {
        tt_free((void *)name);
        return NULL;
    }

    rr->rdlength = rdlength;

    if ((rr->itf->parse != NULL) &&
        !TT_OK(rr->itf->parse(rr, buf, pkt, pkt_len))) {
        tt_adns_rr_destroy(rr);
        return NULL;
    }
    return rr;
}

tt_result_t __check_adrr_itf(tt_adns_rr_itf_t *itf)
{
    if (itf == NULL) {
        TT_ERROR("null adrr itf");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __adrr_render_type(IN tt_buf_t *buf, IN tt_adns_rr_type_t type)
{
    tt_u16_t rr_type;
    tt_u16_t rr_class;

    switch (type) {
        case TT_ADNS_RR_A_IN: {
            rr_type = __ADR_TYPE_A;
            rr_class = __ADR_CLASS_IN;
        } break;
        case TT_ADNS_RR_AAAA_IN: {
            rr_type = __ADR_TYPE_AAAA;
            rr_class = __ADR_CLASS_IN;
        } break;
        case TT_ADNS_RR_CNAME_IN: {
            rr_type = __ADR_TYPE_CNAME;
            rr_class = __ADR_CLASS_IN;
        } break;

        default: {
            return TT_FAIL;
        } break;
    }

    TT_DO(tt_buf_put_u16_n(buf, rr_type));
    TT_DO(tt_buf_put_u16_n(buf, rr_class));

    return TT_SUCCESS;
}

tt_result_t __adrr_parse_type(IN tt_buf_t *buf, OUT tt_adns_rr_type_t *type)
{
    tt_u16_t rr_type;
    tt_u16_t rr_class;

    TT_DO(tt_buf_get_u16_h(buf, &rr_type));
    TT_DO(tt_buf_get_u16_h(buf, &rr_class));

    if ((rr_type == __ADR_TYPE_A) && (rr_class == __ADR_CLASS_IN)) {
        *type = TT_ADNS_RR_A_IN;
        return TT_SUCCESS;
    }

    if ((rr_type == __ADR_TYPE_AAAA) && (rr_class == __ADR_CLASS_IN)) {
        *type = TT_ADNS_RR_AAAA_IN;
        return TT_SUCCESS;
    }

    if ((rr_type == __ADR_TYPE_CNAME) && (rr_class == __ADR_CLASS_IN)) {
        *type = TT_ADNS_RR_CNAME_IN;
        return TT_SUCCESS;
    }

    return TT_FAIL;
}
