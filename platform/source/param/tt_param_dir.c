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

#include <algorithm/ptr/tt_ptr_hashmap.h>
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

static tt_param_itf_t __dir_itf = {
    __dir_on_destroy, NULL, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __tidmap_add(IN tt_ptrhmap_t *m, IN tt_param_t *p);

static void __tidmap_remove(IN tt_ptrhmap_t *m, IN tt_param_t *p);

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
                        &__dir_itf,
                        NULL,
                        attr);
    if (p == NULL) {
        return NULL;
    }

    pd = TT_PARAM_CAST(p, tt_param_dir_t);

    pd->tidmap = NULL;

    tt_list_init(&pd->child);
    pd->child_name_len = 0;

    pd->tidmap_updated = TT_FALSE;

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

#if 0
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
#else
    tt_list_push_tail(&pd->child, &child->node);
#endif

    pd->tidmap_updated = TT_FALSE;

    return TT_SUCCESS;
}

void tt_param_dir_remove(IN tt_param_dir_t *pd, IN tt_param_t *child)
{
    TT_ASSERT(child->node.lst == &pd->child);
    tt_list_remove(&child->node);

    pd->tidmap_updated = TT_FALSE;
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

        if (child->type == TT_PARAM_DIR) {
            tt_param_t *sub;
            sub = tt_param_dir_find(TT_PARAM_CAST(child, tt_param_dir_t),
                                    name,
                                    name_len);
            if (sub != NULL) {
                return sub;
            }
        }
    }
    return NULL;
}

tt_param_t *tt_param_dir_find_tid(IN tt_param_dir_t *pd, IN tt_s32_t tid)
{
    if ((pd->tidmap != NULL) && pd->tidmap_updated) {
        return tt_ptrhmap_find(pd->tidmap, (tt_u8_t *)&tid, sizeof(tt_s32_t));
    } else {
        tt_param_t *p;
        for (p = tt_param_dir_head(pd); p != NULL; p = tt_param_dir_next(p)) {
            if (p->tid == tid) {
                return p;
            } else if (p->type == TT_PARAM_DIR) {
                tt_param_t *sub;
                sub = tt_param_dir_find_tid(TT_PARAM_CAST(p, tt_param_dir_t),
                                            tid);
                if (sub != NULL) {
                    return sub;
                }
            }
        }
        return NULL;
    }
}

tt_result_t tt_param_dir_build_tidmap(IN tt_param_dir_t *pd,
                                      IN tt_u32_t slot_num,
                                      IN OPT tt_ptrhmap_attr_t *attr)
{
    if (pd->tidmap == NULL) {
        pd->tidmap = tt_malloc(sizeof(tt_ptrhmap_t));
        if (pd->tidmap == NULL) {
            TT_ERROR("no mem for tidmap");
            return TT_FAIL;
        }

        if (!TT_OK(tt_ptrhmap_create(pd->tidmap, slot_num, attr))) {
            tt_free(pd->tidmap);
            pd->tidmap = NULL;
            return TT_FAIL;
        }
    }
    TT_ASSERT(pd->tidmap != NULL);

    tt_ptrhmap_clear(pd->tidmap);
    if (TT_OK(__tidmap_add(pd->tidmap, TT_PARAM_OF(pd)))) {
        pd->tidmap_updated = TT_TRUE;
        return TT_SUCCESS;
    } else {
        pd->tidmap_updated = TT_FALSE;
        return TT_FAIL;
    }
}

void __dir_on_destroy(IN tt_param_t *p)
{
    tt_param_dir_t *pd = TT_PARAM_CAST(p, tt_param_dir_t);
    tt_lnode_t *node;

    TT_ASSERT(p->node.lst == NULL);

    if (pd->tidmap != NULL) {
        // no need to care about elements in tidmap, as they would all be
        // destroyed later
        tt_ptrhmap_destroy(pd->tidmap);
        tt_free(pd->tidmap);
    }

    while ((node = tt_list_pop_tail(&pd->child)) != NULL) {
        tt_param_destroy(TT_CONTAINER(node, tt_param_t, node));
    }
}

tt_result_t __tidmap_add(IN tt_ptrhmap_t *m, IN tt_param_t *p)
{
    if (!TT_OK(tt_ptrhmap_add(m, (tt_u8_t *)&p->tid, sizeof(tt_s32_t), p))) {
        return TT_FAIL;
    }

    if (p->type == TT_PARAM_DIR) {
        tt_param_dir_t *pd = TT_PARAM_CAST(p, tt_param_dir_t);
        tt_param_t *c;

        for (c = tt_param_dir_head(pd); c != NULL; c = tt_param_dir_next(c)) {
            if (!TT_OK(__tidmap_add(m, c))) {
                // remove p and its offspring
                __tidmap_remove(m, p);
                return TT_FAIL;
            }
        }
    }

    return TT_SUCCESS;
}

void __tidmap_remove(IN tt_ptrhmap_t *m, IN tt_param_t *p)
{
    tt_ptrhmap_remove_key(m, (tt_u8_t *)&p->tid, sizeof(tt_s32_t));

    if (p->type == TT_PARAM_DIR) {
        tt_param_dir_t *pd = TT_PARAM_CAST(p, tt_param_dir_t);
        tt_param_t *c;

        for (c = tt_param_dir_head(pd); c != NULL; c = tt_param_dir_next(c)) {
            __tidmap_remove(m, c);
        }
    }
}
