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

#include <init/tt_config_directory.h>

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

static void __cdir_on_destroy(IN tt_cfgobj_t *co);

static tt_cfgobj_itf_t __cfgdir_itf = {
    __cdir_on_destroy, NULL, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_cfgobj_t *tt_cfgdir_create(IN const tt_char_t *name,
                              IN OPT tt_cfgobj_attr_t *attr)
{
    tt_cfgobj_t *co;
    tt_cfgdir_t *cdir;

    co = tt_cfgobj_create(sizeof(tt_cfgdir_t),
                          TT_CFGOBJ_DIR,
                          name,
                          &__cfgdir_itf,
                          NULL,
                          attr);
    if (co == NULL) {
        return NULL;
    }

    cdir = TT_CFGOBJ_CAST(co, tt_cfgdir_t);

    tt_list_init(&cdir->child);
    cdir->child_name_len = 0;

    return co;
}

tt_result_t tt_cfgdir_add(IN tt_cfgdir_t *cdir, IN tt_cfgobj_t *child)
{
    tt_u32_t n;
    tt_s32_t cmp;

    n = (tt_u32_t)tt_strlen(child->name);
    if (tt_cfgdir_find(cdir, child->name, n)) {
        TT_ERROR("duplicate node name");
        return TT_FAIL;
    }

    // name + slash
    if (child->type == TT_CFGOBJ_DIR) {
        n += 1;
    }
    if (cdir->child_name_len < n) {
        cdir->child_name_len = n;
    }

    if (child->type == TT_CFGOBJ_DIR) {
        tt_lnode_t *node;

        for (node = tt_list_head(&cdir->child); node != NULL;
             node = node->next) {
            tt_cfgobj_t *cur = TT_CONTAINER(node, tt_cfgobj_t, node);

            if (cur->type != TT_CFGOBJ_DIR) {
                break;
            }

            if (tt_strcmp(child->name, cur->name) < 0) {
                tt_list_insert_before(node, &child->node);
                return TT_SUCCESS;
            }
            // else continue
        }
        if (node != NULL) {
            tt_list_insert_before(node, &child->node);
        } else {
            tt_list_push_tail(&cdir->child, &child->node);
        }
    } else {
        tt_lnode_t *node;

        for (node = tt_list_head(&cdir->child); node != NULL;
             node = node->next) {
            tt_cfgobj_t *cur = TT_CONTAINER(node, tt_cfgobj_t, node);
            if (cur->type != TT_CFGOBJ_DIR) {
                break;
            }
        }

        if (node == NULL) {
            tt_list_push_tail(&cdir->child, &child->node);
            return TT_SUCCESS;
        }

        for (; node != NULL; node = node->next) {
            tt_cfgobj_t *cur = TT_CONTAINER(node, tt_cfgobj_t, node);

            TT_ASSERT(cur->type != TT_CFGOBJ_DIR);

            if (tt_strcmp(child->name, cur->name) < 0) {
                tt_list_insert_before(node, &child->node);
                return TT_SUCCESS;
            }
            // else continue
        }
        tt_list_push_tail(&cdir->child, &child->node);
    }

    return TT_SUCCESS;
}

void tt_cfgdir_remove(IN tt_cfgdir_t *cdir, IN tt_cfgobj_t *child)
{
    TT_ASSERT(child->node.lst == &cdir->child);
    tt_list_remove(&child->node);
}

tt_cfgobj_t *tt_cfgdir_find(IN tt_cfgdir_t *cdir,
                            IN const tt_char_t *name,
                            IN tt_u32_t name_len)
{
    tt_lnode_t *node;

    for (node = tt_list_head(&cdir->child); node != NULL; node = node->next) {
        tt_cfgobj_t *child = TT_CONTAINER(node, tt_cfgobj_t, node);

        if ((tt_strlen(child->name) == name_len) &&
            (tt_strncmp(child->name, name, name_len) == 0)) {
            return child;
        }
    }
    return NULL;
}

tt_result_t tt_cfgdir_ls(IN tt_cfgdir_t *cdir,
                         IN const tt_char_t *col_sep,
                         IN const tt_char_t *line_sep,
                         OUT tt_buf_t *output)
{
    tt_lnode_t *node;
    tt_u32_t len = (tt_u32_t)tt_strlen(line_sep);
    tt_bool_t empty = TT_TRUE;

    for (node = tt_list_head(&cdir->child); node != NULL; node = node->next) {
        tt_cfgobj_t *child = TT_CONTAINER(node, tt_cfgobj_t, node);

        TT_DO(tt_cfgobj_line(child, col_sep, cdir->child_name_len, output));
        TT_DO(tt_buf_put(output, (tt_u8_t *)line_sep, len));
        empty = TT_FALSE;
    }
    if (!empty) {
        // do not need final line's line_sep
        tt_buf_dec_wp(output, len);
    }

    return TT_SUCCESS;
}

void __cdir_on_destroy(IN tt_cfgobj_t *co)
{
    tt_cfgdir_t *cdir = TT_CFGOBJ_CAST(co, tt_cfgdir_t);
    tt_lnode_t *node;

    TT_ASSERT(co->node.lst == NULL);

    while ((node = tt_list_pop_tail(&cdir->child)) != NULL) {
        tt_cfgobj_destroy(TT_CONTAINER(node, tt_cfgobj_t, node));
    }
}
