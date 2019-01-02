/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
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

#include <param/tt_param_path.h>

#include <algorithm/tt_buffer_common.h>
#include <algorithm/tt_buffer_format.h>
#include <param/tt_param_dir.h>

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

static tt_param_t *__param_goto(IN tt_param_t *root,
                                IN tt_param_t *current,
                                IN const tt_char_t *name,
                                IN tt_u32_t len);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_param_t *tt_param_path_p2n(IN tt_param_t *root,
                              IN tt_param_t *current,
                              IN const tt_char_t *path,
                              IN tt_u32_t len)
{
    tt_u32_t prev, pos;
    tt_param_t *p;

    TT_ASSERT(path != NULL);

    if (len == 0) {
        return NULL;
    }

    if (path[0] == '/') {
        prev = 1;
        p = root;
    } else {
        prev = 0;
        p = current;
    }
    TT_ASSERT(p != NULL);
    if (p->type != TT_PARAM_DIR) {
        return NULL;
    }

// find next '/'
next_slash:
    pos = prev;
    while ((pos < len) && (path[pos] != '/')) {
        ++pos;
    }
    if (pos < len) {
        p = __param_goto(root, p, &path[prev], pos - prev);
        if ((p == NULL) || (p->type != TT_PARAM_DIR)) {
            // here we must have seen "/xxx/", so p must be a dir
            return NULL;
        }

        prev = pos + 1;
        goto next_slash;
    }
    TT_ASSERT(pos == len);

    // last node
    if (prev < len) {
        p = __param_goto(root, p, &path[prev], pos - prev);
    }

    return p;
}

tt_result_t tt_param_path_n2p(IN OPT tt_param_t *root,
                              IN tt_param_t *current,
                              OUT tt_buf_t *path)
{
    tt_u32_t pos;
    tt_param_t *p;
    tt_u8_t slash = '/';

    TT_ASSERT(current != NULL);
    TT_ASSERT(path != NULL);

    if (root == current) {
        return TT_SUCCESS;
    }

    pos = path->wpos;
    TT_DO(tt_buf_insert_cstr(path, pos, current->name));

    p = current;
    while ((p = tt_param_parent(p)) != NULL) {
        if (p == root) {
            break;
        }
        TT_DO(tt_buf_insert(path, pos, &slash, 1));
        TT_DO(tt_buf_insert_cstr(path, pos, p->name));
    }

    return TT_SUCCESS;
}

tt_result_t tt_param_path_complete(IN tt_param_t *root,
                                   IN tt_param_t *current,
                                   IN const tt_char_t *path,
                                   IN tt_u32_t path_len,
                                   OUT tt_u32_t *status,
                                   OUT tt_buf_t *output)
{
    const tt_char_t *tail_name;
    tt_u32_t tail_len;
    tt_param_dir_t *cdir;
    tt_lnode_t *node;
    tt_param_t *p;

    const tt_char_t *common;
    tt_u32_t match_num, common_len;

    TT_ASSERT(current != NULL);
    TT_ASSERT(path != NULL);
    TT_ASSERT(status != NULL);
    TT_ASSERT(output != NULL);

    tail_name = path;
    tail_len = path_len;
    if (path_len > 0) {
        const tt_char_t *p;
        tt_u32_t i;

        p = path;
        i = path_len - 1;
        while ((i != ~0) && (p[i] != '/')) {
            --i;
        }

        if (i != ~0) {
            current = tt_param_path_p2n(root, current, p, i);
            if (current == NULL) {
                // invalid path
                *status = TT_PPCP_NONE;
                return TT_SUCCESS;
            }

            TT_ASSERT((i < path_len) && (path[i] == '/'));
            tail_name = &path[i + 1];
            tail_len = path_len - i - 1;
        }
    }
    // now current is parent node, and has child name and length

    if (current->type != TT_PARAM_DIR) {
        *status = TT_PPCP_NONE;
        return TT_SUCCESS;
    }
    cdir = TT_PARAM_CAST(current, tt_param_dir_t);

    // special case: end with "." or ".."
    if ((tail_len == 1) && (tail_name[0] == '.')) {
        TT_DO(tt_buf_put_u8(output, '/'));
        *status = TT_PPCP_FULL_MORE;
        return TT_SUCCESS;
    } else if ((tail_len == 2) && (tail_name[0] == '.') &&
               (tail_name[1] == '.')) {
        TT_DO(tt_buf_put_u8(output, '/'));
        *status = TT_PPCP_FULL_MORE;
        return TT_SUCCESS;
    }

    if (tail_len == 0) {
        if (tt_list_count(&cdir->child) == 1) {
            // case 1.1: none matching, but only 1 choice
            node = tt_list_head(&cdir->child);
            p = TT_CONTAINER(node, tt_param_t, node);
            TT_DO(tt_buf_put_cstr(output, p->name));
            if (p->type == TT_PARAM_DIR) {
                TT_DO(tt_buf_put_u8(output, '/'));
                *status = TT_PPCP_FULL_MORE;
            } else {
                *status = TT_PPCP_FULL;
            }
            return TT_SUCCESS;
        } else {
            tt_bool_t head = TT_TRUE;

            // case 1.2: none matching, list all children
            for (node = tt_list_head(&cdir->child); node != NULL;
                 node = node->next) {
                p = TT_CONTAINER(node, tt_param_t, node);
                if (head) {
                    head = TT_FALSE;
                } else {
                    TT_DO(tt_buf_put_u8(output, ' '));
                }
                TT_DO(tt_buf_put_cstr(output, p->name));
            }

            *status = TT_PPCP_NONE;
            return TT_SUCCESS;
        }
    }

    // find all matching children
    match_num = 0;
    common = NULL;
    common_len = 0;
    for (node = tt_list_head(&cdir->child); node != NULL; node = node->next) {
        const tt_char_t *name;

        p = TT_CONTAINER(node, tt_param_t, node);

        name = p->name;
        if (tt_memcmp(name, tail_name, tail_len) == 0) {
            if (match_num == 0) {
                // init the common part
                common = name + tail_len;
                common_len = (tt_u32_t)tt_strlen(name) - tail_len;
            } else {
                // reduce common part
                tt_u32_t k = 0;
                tt_u32_t n =
                    TT_MIN(common_len, (tt_u32_t)tt_strlen(name) - tail_len);
                while ((k < n) && (common[k] == name[tail_len + k])) {
                    ++k;
                }
                common_len -= (n - k);
            }

            ++match_num;
        }
    }

    if (match_num == 0) {
        // case 2: none matching, nothing to complete
        *status = TT_PPCP_NONE;
    } else if (match_num == 1) {
        // case 3: fully matched
        for (node = tt_list_head(&cdir->child); node != NULL;
             node = node->next) {
            const tt_char_t *name;

            p = TT_CONTAINER(node, tt_param_t, node);

            name = p->name;
            if (tt_memcmp(name, tail_name, tail_len) == 0) {
                TT_DO(tt_buf_put_cstr(output, name + tail_len));
                if (p->type == TT_PARAM_DIR) {
                    TT_DO(tt_buf_put_u8(output, '/'));
                    *status = TT_PPCP_FULL_MORE;
                } else {
                    *status = TT_PPCP_FULL;
                }
                break;
            }
        }
    } else if (common_len > 0) {
        // case 4: partial matched
        TT_DO(tt_buf_put(output, (tt_u8_t *)common, common_len));
        *status = TT_PPCP_PARTIAL;
    } else {
        tt_bool_t head = TT_TRUE;

        // case 5: nothing to complete, list all candidates
        for (node = tt_list_head(&cdir->child); node != NULL;
             node = node->next) {
            const tt_char_t *name;

            p = TT_CONTAINER(node, tt_param_t, node);
            name = p->name;
            if (tt_memcmp(name, tail_name, tail_len) == 0) {
                if (head) {
                    head = TT_FALSE;
                } else {
                    TT_DO(tt_buf_put_u8(output, ' '));
                }
                TT_DO(tt_buf_put_cstr(output, name));
            }
        }
        *status = TT_PPCP_NONE;
    }

    return TT_SUCCESS;
}

tt_param_t *__param_goto(IN tt_param_t *root,
                         IN tt_param_t *current,
                         IN const tt_char_t *name,
                         IN tt_u32_t len)
{
    if ((len == 1) && (tt_strncmp(name, ".", 1) == 0)) {
        return current;
    } else if ((len == 2) && (tt_strncmp(name, "..", 2) == 0)) {
        return tt_param_parent(current);
    } else {
        return tt_param_dir_find(TT_PARAM_CAST(current, tt_param_dir_t),
                                 name,
                                 len);
    }
}
