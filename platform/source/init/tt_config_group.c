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

#include <init/tt_config_group.h>

#include <algorithm/tt_buffer_format.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __cgrp_on_destroy(IN tt_cfgnode_t *cnode, IN tt_bool_t committed);

static tt_cfgnode_itf_t __cgrp_itf_l = {
    __cgrp_on_destroy,
    NULL,
    NULL,
    tt_cfggrp_ls,
    NULL,
    NULL,
    NULL,
    tt_cfggrp_commit,
};

static tt_cfgnode_itf_t __cgrp_itf_arl = {
    __cgrp_on_destroy,
    tt_cfggrp_add_child,
    tt_cfggrp_rm_child,
    tt_cfggrp_ls,
    NULL,
    NULL,
    NULL,
    tt_cfggrp_commit,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_cfgnode_t *__find_by_name(IN tt_list_t *list, IN tt_blob_t *name);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_cfgnode_t *tt_cfggrp_create(IN const tt_char_t *name,
                               IN OPT tt_cfgnode_itf_t *itf,
                               IN OPT void *opaque,
                               IN OPT tt_cfggrp_cb_t *cb,
                               IN OPT tt_cfggrp_attr_t *attr)
{
    tt_cfggrp_attr_t cgrp_attr;
    tt_cfgnode_t *cnode;
    tt_cfggrp_t *cgrp;

    if (attr == NULL) {
        tt_cfggrp_attr_default(&cgrp_attr);
        attr = &cgrp_attr;
    }

    if (itf == NULL) {
        if (attr->mode == TT_CFGGRP_MODE_ARL) {
            itf = &__cgrp_itf_arl;
        } else {
            itf = &__cgrp_itf_l;
        }
    }

    cnode = tt_cfgnode_create(sizeof(tt_cfggrp_t),
                              TT_CFGNODE_TYPE_GROUP,
                              name,
                              itf,
                              opaque,
                              &attr->cnode_attr);
    if (cnode == NULL) {
        return NULL;
    }

    cgrp = TT_CFGNODE_CAST(cnode, tt_cfggrp_t);

    tt_list_init(&cgrp->child);
    tt_list_init(&cgrp->new_child);
    cgrp->child_name_len = 0;

    cgrp->cb = cb;

    return cnode;
}

void tt_cfggrp_attr_default(IN tt_cfggrp_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_cfgnode_attr_default(&attr->cnode_attr);

    attr->mode = TT_CFGGRP_MODE_L;
}

void tt_cfggrp_itf_default(OUT tt_cfgnode_itf_t *itf, IN tt_u32_t mode)
{
    switch (mode) {
        case TT_CFGGRP_MODE_ARL: {
            tt_memcpy(itf, &__cgrp_itf_arl, sizeof(tt_cfgnode_itf_t));
        } break;

        case TT_CFGGRP_MODE_L:
        default: {
            tt_memcpy(itf, &__cgrp_itf_l, sizeof(tt_cfgnode_itf_t));
        } break;
    }
}

tt_result_t tt_cfggrp_add(IN tt_cfgnode_t *cnode, IN tt_cfgnode_t *child)
{
    tt_cfggrp_t *cgrp;
    tt_u32_t n;
    tt_s32_t cmp;

    TT_ASSERT(cnode->type == TT_CFGNODE_TYPE_GROUP);
    cgrp = TT_CFGNODE_CAST(cnode, tt_cfggrp_t);

    // name + sp
    n = (tt_u32_t)tt_strlen(child->name);
    if (child->type == TT_CFGNODE_TYPE_GROUP) {
        n += 1;
    }
    if (cgrp->child_name_len < n) {
        cgrp->child_name_len = n;
    }

    if (child->type == TT_CFGNODE_TYPE_GROUP) {
        tt_lnode_t *node;

        for (node = tt_list_head(&cgrp->child); node != NULL;
             node = node->next) {
            tt_cfgnode_t *cur_child = TT_CONTAINER(node, tt_cfgnode_t, node);

            if (cur_child->type != TT_CFGNODE_TYPE_GROUP) {
                break;
            }

            cmp = tt_strcmp(child->name, cur_child->name);
            if (cmp == 0) {
                TT_ERROR("duplicate node name");
                return TT_FAIL;
            } else if (cmp < 0) {
                tt_list_insert_prev(node, &child->node);
                return TT_SUCCESS;
            }
            // else continue
        }
        if (node != NULL) {
            tt_list_insert_prev(node, &child->node);
        } else {
            tt_list_addtail(&cgrp->child, &child->node);
        }
    } else {
        tt_lnode_t *node;

        for (node = tt_list_head(&cgrp->child); node != NULL;
             node = node->next) {
            tt_cfgnode_t *cur_child = TT_CONTAINER(node, tt_cfgnode_t, node);
            if (cur_child->type != TT_CFGNODE_TYPE_GROUP) {
                break;
            }
        }

        if (node == NULL) {
            tt_list_addtail(&cgrp->child, &child->node);
            return TT_SUCCESS;
        }

        for (; node != NULL; node = node->next) {
            tt_cfgnode_t *cur_child = TT_CONTAINER(node, tt_cfgnode_t, node);

            TT_ASSERT(cur_child->type != TT_CFGNODE_TYPE_GROUP);

            cmp = tt_strcmp(child->name, cur_child->name);
            if (cmp == 0) {
                TT_ERROR("duplicate node name");
                return TT_FAIL;
            } else if (cmp < 0) {
                tt_list_insert_prev(node, &child->node);
                return TT_SUCCESS;
            }
            // else continue
        }
        tt_list_addtail(&cgrp->child, &child->node);
    }

    return TT_SUCCESS;
}

tt_cfgnode_t *tt_cfggrp_find_blob(IN tt_cfgnode_t *cnode, IN tt_blob_t *name)
{
    tt_cfggrp_t *cgrp;
    tt_lnode_t *node;

    TT_ASSERT(cnode->type == TT_CFGNODE_TYPE_GROUP);
    cgrp = TT_CFGNODE_CAST(cnode, tt_cfggrp_t);

    for (node = tt_list_head(&cgrp->child); node != NULL; node = node->next) {
        tt_cfgnode_t *child = TT_CONTAINER(node, tt_cfgnode_t, node);

        if (tt_blob_cmpcstr(name, child->name)) {
            return child;
        }
    }
    return NULL;
}

tt_cfgnode_t *tt_cfggrp_find(IN tt_cfgnode_t *cnode, IN const tt_char_t *name)
{
    tt_cfggrp_t *cgrp;
    tt_lnode_t *node;

    TT_ASSERT(cnode->type == TT_CFGNODE_TYPE_GROUP);
    cgrp = TT_CFGNODE_CAST(cnode, tt_cfggrp_t);

    for (node = tt_list_head(&cgrp->child); node != NULL; node = node->next) {
        tt_cfgnode_t *child = TT_CONTAINER(node, tt_cfgnode_t, node);

        if (tt_strcmp(name, child->name) == 0) {
            return child;
        }
    }
    return NULL;
}

tt_result_t tt_cfggrp_add_child(IN tt_cfgnode_t *cnode,
                                IN tt_blob_t *name,
                                IN tt_blob_t *val)
{
    tt_cfggrp_t *cgrp = TT_CFGNODE_CAST(cnode, tt_cfggrp_t);
    tt_cfgnode_t *child;
    tt_blob_t child_name;

    if ((cgrp->cb == NULL) || (cgrp->cb->create_child == NULL)) {
        return TT_BAD_PARAM;
    }

    if ((__find_by_name(&cgrp->child, name) != NULL) ||
        (__find_by_name(&cgrp->new_child, name) != NULL)) {
        return TT_BAD_PARAM;
    }

    child = cgrp->cb->create_child(cgrp, name, val);
    if (child == NULL) {
        return TT_FAIL;
    }
    TT_ASSERT(child->type != TT_CFGNODE_TYPE_GROUP);

    // created child may have other name
    child_name.addr = (tt_u8_t *)child->name;
    child_name.len = (tt_u32_t)tt_strlen(child->name);
    if ((__find_by_name(&cgrp->child, &child_name) != NULL) ||
        (__find_by_name(&cgrp->new_child, &child_name) != NULL)) {
        tt_cfgnode_destroy(child, TT_FALSE);
        return TT_BAD_PARAM;
    }

    tt_list_addtail(&cgrp->new_child, &child->node);

    cnode->modified = TT_TRUE;

    return TT_SUCCESS;
}

tt_result_t tt_cfggrp_rm_child(IN tt_cfgnode_t *cnode, IN tt_blob_t *name)
{
    tt_cfggrp_t *cgrp = TT_CFGNODE_CAST(cnode, tt_cfggrp_t);
    tt_cfgnode_t *child;

    child = __find_by_name(&cgrp->new_child, name);
    if (child == NULL) {
        child = __find_by_name(&cgrp->child, name);
    }
    if ((child == NULL) || (child->type == TT_CFGNODE_TYPE_GROUP)) {
        return TT_BAD_PARAM;
    }

    child->removing = TT_TRUE;

    cnode->modified = TT_TRUE;

    return TT_SUCCESS;
}

tt_result_t tt_cfggrp_ls(IN tt_cfgnode_t *cnode,
                         IN OPT const tt_char_t *seperator,
                         OUT struct tt_buf_s *output)
{
    tt_cfggrp_t *cgrp = TT_CFGNODE_CAST(cnode, tt_cfggrp_t);
    tt_lnode_t *node;
    tt_u32_t sep_len, child_nl, n;
    tt_bool_t has_sep = TT_FALSE;

    if (seperator == NULL) {
        seperator = "\n";
    }
    sep_len = (tt_u32_t)tt_strlen(seperator);

    child_nl = cgrp->child_name_len;
    n = child_nl + 4;
    n = TT_MAX(n, 8);

    // 1st line
    TT_DO(tt_buf_put_cstr(output, "PERM    TYPE    NAME"));
    TT_DO(tt_buf_put_rep(output, ' ', n - 4));
    TT_DO(tt_buf_put_cstr(output, "DESCRIPTION"));
    TT_DO(tt_buf_put_cstr(output, seperator));

    // content
    for (node = tt_list_head(&cgrp->child); node != NULL; node = node->next) {
        tt_cfgnode_t *child_node = TT_CONTAINER(node, tt_cfgnode_t, node);

        TT_DO(tt_cfgnode_describe(child_node, n - 4, output));
        TT_DO(tt_buf_put(output, (tt_u8_t *)seperator, sep_len));
        has_sep = TT_TRUE;
    }
    if (has_sep) {
        // do not need final line's seperator
        tt_buf_dec_wp(output, sep_len);
    }

    return TT_SUCCESS;
}

tt_result_t tt_cfggrp_commit(IN tt_cfgnode_t *cnode)
{
    tt_cfggrp_t *cgrp = TT_CFGNODE_CAST(cnode, tt_cfggrp_t);
    tt_lnode_t *node;
    tt_result_t result = TT_SUCCESS;

    // add new children
    while ((node = tt_list_pophead(&cgrp->new_child)) != NULL) {
        tt_cfgnode_t *child = TT_CONTAINER(node, tt_cfgnode_t, node);

        tt_cfggrp_add(cnode, child);

        if ((cgrp->cb != NULL) && (cgrp->cb->on_add_child != NULL) &&
            cgrp->cb->on_add_child(cnode, child)) {
            result = TT_END;
        }
    }

    // remove or commit children
    node = tt_list_head(&cgrp->child);
    while (node != NULL) {
        tt_cfgnode_t *child = TT_CONTAINER(node, tt_cfgnode_t, node);

        node = node->next;

        if (child->removing) {
            tt_list_remove(&child->node);

            if ((cgrp->cb != NULL) && (cgrp->cb->on_rm_child != NULL) &&
                cgrp->cb->on_rm_child(cnode, child)) {
                result = TT_END;
            }

            tt_cfgnode_destroy(child, TT_TRUE);
        } else {
            if (tt_cfgnode_commit(child) == TT_END) {
                result = TT_END;
            }
        }
    }

    return result;
}

void __cgrp_on_destroy(IN tt_cfgnode_t *cnode, IN tt_bool_t committed)
{
    tt_cfggrp_t *cgrp = TT_CFGNODE_CAST(cnode, tt_cfggrp_t);
    tt_lnode_t *node;

    TT_ASSERT(cnode->node.lst == NULL);

    // destroy uncommited first
    while ((node = tt_list_poptail(&cgrp->new_child)) != NULL) {
        tt_cfgnode_destroy(TT_CONTAINER(node, tt_cfgnode_t, node), TT_FALSE);
    }

    while ((node = tt_list_poptail(&cgrp->child)) != NULL) {
        tt_cfgnode_destroy(TT_CONTAINER(node, tt_cfgnode_t, node), TT_FALSE);
    }
}

tt_cfgnode_t *__find_by_name(IN tt_list_t *list, IN tt_blob_t *name)
{
    tt_lnode_t *node;

    node = tt_list_head(list);
    while (node != NULL) {
        tt_cfgnode_t *cnode = TT_CONTAINER(node, tt_cfgnode_t, node);

        node = node->next;

        if ((tt_strlen(cnode->name) == name->len) &&
            (tt_strncmp(cnode->name, (tt_char_t *)name->addr, name->len) ==
             0)) {
            return cnode;
        }
    }

    return NULL;
}
