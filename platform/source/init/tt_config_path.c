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

#include <init/tt_config_path.h>

#include <algorithm/tt_buffer_common.h>
#include <algorithm/tt_buffer_format.h>
#include <init/tt_config_group.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_cfgnode_t *__cfgnode_goto(IN tt_cfgnode_t *root,
                                    IN tt_cfgnode_t *current,
                                    IN tt_blob_t *name);

static tt_cfgnode_t *__cfgnode_parent(IN tt_cfgnode_t *cnode);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_cfgnode_t *tt_cfgpath_p2n(IN tt_cfgnode_t *root,
                             IN tt_cfgnode_t *current,
                             IN tt_blob_t *path)
{
    const tt_char_t *pc;
    tt_u32_t len, prev_pos, pos;
    tt_cfgnode_t *cnode;
    tt_blob_t name;

    TT_ASSERT(path != NULL);
    pc = (const tt_char_t *)path->addr;
    len = path->len;

    if (len == 0) {
        return NULL;
    }

    // init
    if (pc[0] == '/') {
        prev_pos = 1;
        cnode = root;
    } else {
        prev_pos = 0;
        cnode = current;
    }
    TT_ASSERT(cnode != NULL);
    if (cnode->type != TT_CFGNODE_TYPE_GROUP) {
        return NULL;
    }

// find next '/'
next_slash:
    pos = prev_pos;
    while ((pos < len) && (pc[pos] != '/')) {
        ++pos;
    }
    if (pos < len) {
        name.addr = (tt_u8_t *)&pc[prev_pos];
        name.len = pos - prev_pos;
        cnode = __cfgnode_goto(root, cnode, &name);
        if ((cnode == NULL) || (cnode->type != TT_CFGNODE_TYPE_GROUP)) {
            // here we must have seen "/xxx/", so cnode must be group
            return NULL;
        }

        prev_pos = pos + 1;
        goto next_slash;
    }
    TT_ASSERT(pos == len);

    // last node
    if (prev_pos < len) {
        name.addr = (tt_u8_t *)&pc[prev_pos];
        name.len = pos - prev_pos;
        cnode = __cfgnode_goto(root, cnode, &name);
    }

    return cnode;
}

tt_result_t tt_cfgpath_n2p(IN OPT tt_cfgnode_t *root,
                           IN tt_cfgnode_t *current,
                           OUT tt_buf_t *path)
{
    tt_cfgnode_t *cnode;
    tt_u32_t pos;
    tt_u8_t slash = '/';

    TT_ASSERT(current != NULL);
    TT_ASSERT(path != NULL);

    if (root == current) {
        return TT_SUCCESS;
    }

    pos = path->wpos;
    TT_DO(tt_buf_insert(path,
                        pos,
                        (tt_u8_t *)current->name,
                        (tt_u32_t)tt_strlen(current->name)));

    cnode = current;
    while ((cnode = __cfgnode_parent(cnode)) != NULL) {
        if (cnode == root) {
            break;
        }
        TT_DO(tt_buf_insert(path, pos, (tt_u8_t *)&slash, 1));
        TT_DO(tt_buf_insert(path,
                            pos,
                            (tt_u8_t *)cnode->name,
                            (tt_u32_t)tt_strlen(cnode->name)));
    }

    return TT_SUCCESS;
}

tt_result_t tt_cfgpath_complete(IN tt_cfgnode_t *root,
                                IN tt_cfgnode_t *current,
                                IN tt_blob_t *path,
                                OUT tt_u32_t *status,
                                OUT tt_buf_t *output)
{
    tt_blob_t child;
    tt_cfggrp_t *cgrp;
    tt_lnode_t *node;
    tt_bool_t head = TT_TRUE;
    tt_cfgnode_t *cnode;

    const tt_char_t *common = NULL;
    tt_u32_t match_num = 0, common_len = 0;
    const tt_char_t *name;

    TT_ASSERT(current != NULL);
    TT_ASSERT(path != NULL);
    TT_ASSERT(status != NULL);
    TT_ASSERT(output != NULL);

    child.addr = path->addr;
    child.len = path->len;
    if (path->len > 0) {
        tt_u8_t *p;
        tt_u32_t i;

        p = path->addr;
        i = path->len - 1;
        while ((i != ~0) && (p[i] != '/')) {
            --i;
        }

        if (i != ~0) {
            tt_blob_t parent;

            parent.addr = p;
            parent.len = i;
            current = tt_cfgpath_p2n(root, current, &parent);
            if (current == NULL) {
                // invalid path
                *status = TT_CFGPCP_NONE;
                return TT_SUCCESS;
            }

            TT_ASSERT((i < path->len) && (path->addr[i] == '/'));
            child.addr = &path->addr[i + 1];
            child.len = path->len - i - 1;
        }
    }
    // now current is parent node, and child include child name

    if (current->type != TT_CFGNODE_TYPE_GROUP) {
        *status = TT_CFGPCP_NONE;
        return TT_SUCCESS;
    }
    cgrp = TT_CFGNODE_CAST(current, tt_cfggrp_t);

    if (child.len == 0) {
        if (tt_list_count(&cgrp->child) == 1) {
            // case 1.1: none matching, but only 1 choice
            node = tt_list_head(&cgrp->child);
            cnode = TT_CONTAINER(node, tt_cfgnode_t, node);
            TT_DO(tt_buf_put(output,
                             (tt_u8_t *)cnode->name,
                             (tt_u32_t)tt_strlen(cnode->name)));
            if (cnode->type == TT_CFGNODE_TYPE_GROUP) {
                TT_DO(tt_buf_put_u8(output, '/'));
                *status = TT_CFGPCP_FULL_MORE;
            } else {
                *status = TT_CFGPCP_FULL;
            }
            return TT_SUCCESS;
        } else {
            // case 1.2: none matching, list all children
            for (node = tt_list_head(&cgrp->child); node != NULL;
                 node = node->next) {
                cnode = TT_CONTAINER(node, tt_cfgnode_t, node);
                if (head) {
                    head = TT_FALSE;
                } else {
                    TT_DO(tt_buf_put_u8(output, ' '));
                }
                TT_DO(tt_buf_put(output,
                                 (tt_u8_t *)cnode->name,
                                 (tt_u32_t)tt_strlen(cnode->name)));
            }

            *status = TT_CFGPCP_NONE;
            return TT_SUCCESS;
        }
    }

    // find all matching children
    for (node = tt_list_head(&cgrp->child); node != NULL; node = node->next) {
        cnode = TT_CONTAINER(node, tt_cfgnode_t, node);
        name = cnode->name;

        if (tt_memcmp(name, child.addr, child.len) == 0) {
            if (match_num == 0) {
                // init the common part
                common = name + child.len;
                common_len = (tt_u32_t)tt_strlen(name) - child.len;
            } else {
                // reduce common part
                tt_u32_t k = 0;
                tt_u32_t n =
                    TT_MIN(common_len, (tt_u32_t)tt_strlen(name) - child.len);
                while ((k < n) && (common[k] == name[child.len + k])) {
                    ++k;
                }
                common_len -= (n - k);
            }

            ++match_num;
        }
    }

    if (match_num == 0) {
        // case 2: none matching, nothing to complete
        *status = TT_CFGPCP_NONE;
    } else if (match_num == 1) {
        // case 3: fully matched
        for (node = tt_list_head(&cgrp->child); node != NULL;
             node = node->next) {
            cnode = TT_CONTAINER(node, tt_cfgnode_t, node);
            name = cnode->name;

            if (tt_memcmp(name, child.addr, child.len) == 0) {
                TT_DO(tt_buf_put(output,
                                 (tt_u8_t *)name + child.len,
                                 (tt_u32_t)tt_strlen(name) - child.len));
                if (cnode->type == TT_CFGNODE_TYPE_GROUP) {
                    TT_DO(tt_buf_put_u8(output, '/'));
                    *status = TT_CFGPCP_FULL_MORE;
                } else {
                    *status = TT_CFGPCP_FULL;
                }
                break;
            }
        }
    } else if (common_len > 0) {
        // case 4: partial matched
        TT_DO(tt_buf_put(output, (tt_u8_t *)common, common_len));
        *status = TT_CFGPCP_PARTIAL;
    } else {
        // case 5: nothing to complete, list all candidates
        for (node = tt_list_head(&cgrp->child); node != NULL;
             node = node->next) {
            cnode = TT_CONTAINER(node, tt_cfgnode_t, node);
            name = cnode->name;

            if (tt_memcmp(name, child.addr, child.len) == 0) {
                if (head) {
                    head = TT_FALSE;
                } else {
                    TT_DO(tt_buf_put_u8(output, ' '));
                }
                TT_DO(tt_buf_put(output,
                                 (tt_u8_t *)name,
                                 (tt_u32_t)tt_strlen(name)));
            }
        }
        *status = TT_CFGPCP_NONE;
    }

    return TT_SUCCESS;
}

tt_cfgnode_t *__cfgnode_goto(IN tt_cfgnode_t *root,
                             IN tt_cfgnode_t *current,
                             IN tt_blob_t *name)
{
    if (tt_blob_cmpcstr(name, ".")) {
        return current;
    } else if (tt_blob_cmpcstr(name, "..")) {
        return __cfgnode_parent(current);
    } else {
        return tt_cfggrp_find_blob(current, name);
    }
}

tt_cfgnode_t *__cfgnode_parent(IN tt_cfgnode_t *cnode)
{
    tt_cfggrp_t *cgrp;

    if (cnode->node.lst == NULL) {
        return NULL;
    }
    // be sure the cnode is not a uncommitted node
    cgrp = TT_CONTAINER(cnode->node.lst, tt_cfggrp_t, child);

    return TT_CFGNODE_OF(cgrp);
}
