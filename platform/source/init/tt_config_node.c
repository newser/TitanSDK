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

#include <init/tt_config_node.h>

#include <algorithm/tt_buffer_format.h>
#include <init/tt_config_group.h>
#include <init/tt_config_s32.h>
#include <init/tt_config_string.h>
#include <init/tt_config_u32.h>
#include <memory/tt_memory_alloc.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __CFGNODE_PERM_NUM 4

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __cfgnode_get_perm(IN tt_cfgnode_t *cnode,
                               OUT tt_cfgnode_perm_t *perm);
static tt_u32_t __cfgnode_perm2str(IN tt_cfgnode_perm_t *perm,
                                   OUT tt_char_t *str);

static tt_result_t __validate_name(IN const tt_char_t *name);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_cfgnode_t *tt_cfgnode_create(IN tt_u32_t len,
                                IN tt_cfgnode_type_t type,
                                IN const tt_char_t *name,
                                IN tt_cfgnode_itf_t *itf,
                                IN OPT void *opaque,
                                IN OPT tt_cfgnode_attr_t *attr)
{
    tt_cfgnode_t *cnode;
    tt_cfgnode_attr_t __attr;

    TT_ASSERT(TT_CFGNODE_TYPE_VALID(type));
    TT_ASSERT(name != NULL);
    TT_ASSERT(itf != NULL);

    if (!TT_OK(__validate_name(name))) {
        return NULL;
    }

    cnode = (tt_cfgnode_t *)tt_malloc(sizeof(tt_cfgnode_t) + len);
    if (cnode == NULL) {
        TT_ERROR("no mem for cfg node");
        return NULL;
    }
    tt_memset(cnode, 0, sizeof(tt_cfgnode_t) + len);

    if (attr == NULL) {
        tt_cfgnode_attr_default(&__attr);
        attr = &__attr;
    }

    cnode->type = type;
    cnode->opaque = opaque;
    cnode->itf = itf;
    tt_lnode_init(&cnode->node);

    cnode->name = name;
    cnode->display_name = attr->display_name;
    cnode->brief = attr->brief;
    cnode->detail = attr->detail;

    cnode->removing = TT_FALSE;
    cnode->modified = TT_FALSE;
    cnode->need_reboot = attr->need_reboot;

    return cnode;
}

void tt_cfgnode_destroy(IN tt_cfgnode_t *cnode, IN tt_bool_t committed)
{
    TT_ASSERT(cnode != NULL);

    if ((cnode->itf != NULL) && (cnode->itf->on_destroy != NULL)) {
        cnode->itf->on_destroy(cnode, committed);
    }

    tt_free(cnode);
}

void tt_cfgnode_attr_default(IN tt_cfgnode_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->display_name = "";
    attr->brief = "";
    attr->detail = "";

    attr->need_reboot = TT_FALSE;
}

tt_result_t tt_cfgnode_describe(IN tt_cfgnode_t *cnode,
                                IN tt_u32_t max_name_len,
                                OUT tt_buf_t *output)
{
    tt_cfgnode_perm_t perm;
    tt_char_t prefix[__CFGNODE_PERM_NUM + 12];
    tt_u32_t i = 0, slash_len, name_len;
    tt_char_t type[TT_CFGNODE_TYPE_NUM][4] = {
        "u32 ", "s32 ", "grp ", "str ", "bool",
    };

    TT_ASSERT(cnode != NULL);
    TT_ASSERT(output != NULL);
    TT_ASSERT(cnode->itf != NULL);

    tt_memset(prefix, ' ', sizeof(prefix));

    // permissions, 8 chars
    __cfgnode_get_perm(cnode, &perm);
    i += __cfgnode_perm2str(&perm, &prefix[i]);
    i += 4;

    // type, 8 chars
    tt_memcpy(&prefix[i], type[cnode->type], 4);
    i += 8;
    TT_ASSERT(i == sizeof(prefix));

    TT_DO(tt_buf_put(output, (tt_u8_t *)prefix, sizeof(prefix)));

    // name, at least 8 chars
    slash_len = TT_COND(cnode->type == TT_CFGNODE_TYPE_GROUP, 1, 0);
    name_len = (tt_u32_t)tt_strlen(cnode->name) + slash_len;
    if (max_name_len != 0) {
        if (max_name_len < name_len) {
            TT_DO(tt_buf_put_rep(output, '?', max_name_len));
        } else {
            TT_DO(tt_buf_put_cstr(output, cnode->name));
            TT_DO(tt_buf_put_subcstr(output, "/", slash_len));
            TT_DO(tt_buf_put_rep(output, ' ', max_name_len - name_len));
        }
    } else {
        TT_DO(tt_buf_put_cstr(output, cnode->name));
        TT_DO(tt_buf_put_subcstr(output, "/", slash_len));
    }
    TT_DO(tt_buf_put_rep(output, ' ', 4));

    // information
    TT_DO(tt_buf_put_cstr(output, tt_cfgnode_brief(cnode)));

    return TT_SUCCESS;
}

void __cfgnode_get_perm(IN tt_cfgnode_t *cnode, OUT tt_cfgnode_perm_t *perm)
{
    tt_cfgnode_itf_t *itf = cnode->itf;

    perm->can_add = TT_BOOL(itf->add_child != NULL);
    perm->can_rm = TT_BOOL(itf->rm_child != NULL);
    perm->can_ls = TT_BOOL(itf->ls != NULL);
    perm->can_get = TT_BOOL(itf->get != NULL);
    perm->can_set = TT_BOOL(itf->set != NULL);
    perm->can_check = TT_BOOL(itf->check != NULL);
}

tt_u32_t __cfgnode_perm2str(IN tt_cfgnode_perm_t *perm, OUT tt_char_t *str)
{
    TT_ASSERT(__CFGNODE_PERM_NUM == 4);

    str[0] = TT_COND(perm->can_add, 'a', '-');
    str[1] = TT_COND(perm->can_rm, 'r', '-');
    str[2] = TT_COND(perm->can_get, 'g', '-');
    str[3] = TT_COND(perm->can_set, 's', '-');

    return __CFGNODE_PERM_NUM;
}

tt_result_t __validate_name(IN const tt_char_t *name)
{
    tt_u32_t i, n;

    n = (tt_u32_t)tt_strlen(name);
    for (i = 0; i < n; ++i) {
        tt_char_t c = name[i];

        if (!tt_isalnum(c) && (c != '-')) {
            TT_ERROR("invalid char: %c, 0x%x", c, c);
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

// ========================================
// node operations
// ========================================

tt_result_t tt_cfgnode_add(IN tt_cfgnode_t *cnode,
                           IN tt_blob_t *name,
                           IN tt_blob_t *val)
{
    TT_ASSERT(cnode != NULL);
    TT_ASSERT((name != NULL) && (name->addr != NULL));
    TT_ASSERT((val != NULL) && (val->addr != NULL));

    TT_ASSERT(cnode->itf != NULL);
    if (cnode->itf->add_child == NULL) {
        return TT_BAD_PARAM;
    }
    return cnode->itf->add_child(cnode, name, val);
}

tt_result_t tt_cfgnode_rm(IN tt_cfgnode_t *cnode, IN tt_blob_t *name)
{
    TT_ASSERT(cnode != NULL);
    TT_ASSERT((name != NULL) && (name->addr != NULL));

    TT_ASSERT(cnode->itf != NULL);
    if (cnode->itf->rm_child == NULL) {
        return TT_BAD_PARAM;
    }
    return cnode->itf->rm_child(cnode, name);
}

tt_result_t tt_cfgnode_ls(IN struct tt_cfgnode_s *cnode,
                          IN OPT const tt_char_t *newline,
                          OUT tt_buf_t *output)
{
    TT_ASSERT(cnode != NULL);
    TT_ASSERT(output != NULL);

    TT_ASSERT(cnode->itf != NULL);
    if (cnode->itf->ls == NULL) {
        return TT_BAD_PARAM;
    }
    return cnode->itf->ls(cnode, newline, output);
}

tt_result_t tt_cfgnode_get(IN tt_cfgnode_t *cnode, OUT tt_buf_t *output)
{
    TT_ASSERT(cnode != NULL);
    TT_ASSERT(output != NULL);

    TT_ASSERT(cnode->itf != NULL);
    if (cnode->itf->get == NULL) {
        return TT_BAD_PARAM;
    }
    return cnode->itf->get(cnode, output);
}

tt_result_t tt_cfgnode_set(IN tt_cfgnode_t *cnode, IN tt_blob_t *val)
{
    TT_ASSERT(cnode != NULL);
    TT_ASSERT((val != NULL) && (val->addr != NULL));

    TT_ASSERT(cnode->itf != NULL);
    if (cnode->itf->set == NULL) {
        return TT_BAD_PARAM;
    }
    return cnode->itf->set(cnode, val);
}

tt_result_t tt_cfgnode_check(IN tt_cfgnode_t *cnode, IN tt_blob_t *val)
{
    TT_ASSERT(cnode != NULL);
    TT_ASSERT((val != NULL) && (val->addr != NULL));

    TT_ASSERT(cnode->itf != NULL);
    if (cnode->itf->check == NULL) {
        return TT_SUCCESS;
    }
    return cnode->itf->check(cnode, val);
}

tt_result_t tt_cfgnode_commit(IN tt_cfgnode_t *cnode)
{
    TT_ASSERT(cnode != NULL);

    TT_ASSERT(cnode->itf != NULL);
    if (cnode->itf->commit == NULL) {
        return TT_BAD_PARAM;
    }
    return cnode->itf->commit(cnode);
}

void tt_cfgnode_restore(IN tt_cfgnode_t *cnode)
{
    if (cnode->type == TT_CFGNODE_TYPE_GROUP) {
        tt_cfggrp_t *cgrp = TT_CFGNODE_CAST(cnode, tt_cfggrp_t);
        tt_lnode_t *node;

        // restore children
        node = tt_list_head(&cgrp->child);
        while (node != NULL) {
            tt_cfgnode_t *child = TT_CONTAINER(node, tt_cfgnode_t, node);

            node = node->next;

            tt_cfgnode_restore(child);
        }

        // remove added children
        while ((node = tt_list_pophead(&cgrp->new_child)) != NULL) {
            tt_cfgnode_destroy(TT_CONTAINER(node, tt_cfgnode_t, node),
                               TT_FALSE);
        }
    } else {
        cnode->modified = TT_FALSE;
        cnode->removing = TT_FALSE;
    }
}
