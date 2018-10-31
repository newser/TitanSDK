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

#include <param/tt_param_dir.h>

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

static void __dir_on_destroy(IN tt_param_t *p);

static tt_param_itf_t __cfgdir_itf = {
    __dir_on_destroy, NULL, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_param_t *tt_param_dir_create(IN const tt_char_t *name,
                                IN OPT tt_param_attr_t *attr)
{
    tt_param_t *p;
    tt_param_dir_t *pd;

    p = tt_param_create(sizeof(tt_param_dir_t),
                        TT_PARAM_DIR,
                        name,
                        &__cfgdir_itf,
                        NULL,
                        attr);
    if (p == NULL) {
        return NULL;
    }

    pd = TT_PARAM_CAST(p, tt_param_dir_t);

    tt_list_init(&pd->child);
    pd->child_name_len = 0;

    return p;
}

tt_result_t tt_param_dir_add(IN tt_param_dir_t *pd, IN tt_param_t *child)
{
    tt_u32_t n;

    n = (tt_u32_t)tt_strlen(child->name);
    if (tt_param_dir_find(pd, child->name, n)) {
        TT_ERROR("duplicate node name");
        return TT_FAIL;
    }

    // name + slash
    if (child->type == TT_PARAM_DIR) {
        n += 1;
    }
    if (pd->child_name_len < n) {
        pd->child_name_len = n;
    }

    if (child->type == TT_PARAM_DIR) {
        tt_lnode_t *node;

        for (node = tt_list_head(&pd->child); node != NULL; node = node->next) {
            tt_param_t *cur = TT_CONTAINER(node, tt_param_t, node);

            if (cur->type != TT_PARAM_DIR) {
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
            tt_list_push_tail(&pd->child, &child->node);
        }
    } else {
        tt_lnode_t *node;

        for (node = tt_list_head(&pd->child); node != NULL; node = node->next) {
            tt_param_t *cur = TT_CONTAINER(node, tt_param_t, node);
            if (cur->type != TT_PARAM_DIR) {
                break;
            }
        }

        if (node == NULL) {
            tt_list_push_tail(&pd->child, &child->node);
            return TT_SUCCESS;
        }

        for (; node != NULL; node = node->next) {
            tt_param_t *cur = TT_CONTAINER(node, tt_param_t, node);

            TT_ASSERT(cur->type != TT_PARAM_DIR);

            if (tt_strcmp(child->name, cur->name) < 0) {
                tt_list_insert_before(node, &child->node);
                return TT_SUCCESS;
            }
            // else continue
        }
        tt_list_push_tail(&pd->child, &child->node);
    }

    return TT_SUCCESS;
}

void tt_param_dir_remove(IN tt_param_dir_t *pd, IN tt_param_t *child)
{
    TT_ASSERT(child->node.lst == &pd->child);
    tt_list_remove(&child->node);
}

tt_param_t *tt_param_dir_find(IN tt_param_dir_t *pd,
                              IN const tt_char_t *name,
                              IN tt_u32_t name_len)
{
    tt_lnode_t *node;

    for (node = tt_list_head(&pd->child); node != NULL; node = node->next) {
        tt_param_t *child = TT_CONTAINER(node, tt_param_t, node);

        if ((tt_strlen(child->name) == name_len) &&
            (tt_strncmp(child->name, name, name_len) == 0)) {
            return child;
        }
    }
    return NULL;
}

void __dir_on_destroy(IN tt_param_t *p)
{
    tt_param_dir_t *pd = TT_PARAM_CAST(p, tt_param_dir_t);
    tt_lnode_t *node;

    TT_ASSERT(p->node.lst == NULL);

    while ((node = tt_list_pop_tail(&pd->child)) != NULL) {
        tt_param_destroy(TT_CONTAINER(node, tt_param_t, node));
    }
}
