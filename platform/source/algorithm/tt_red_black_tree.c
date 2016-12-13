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

#include <algorithm/tt_red_black_tree.h>

#include <misc/tt_assert.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define TT_RBTREE_NODE_RED 0
#define TT_RBTREE_NODE_BLACK 1

#define TT_RBTREE_NODE_NIL(p_node) (p_node == (&p_node->tree->tt_nil))

#ifdef TT_RBTREE_SANITY_CHECK
#define TT_ASSERT_RBTREE TT_ASSERT
#define __RBTREE_EXPENSIVE_CHECK(tree)                                         \
    TT_ASSERT_ALWAYS(TT_OK(__rbtree_expensive_check((tree)->root, NULL)))
#else
#define TT_ASSERT_RBTREE(...)
#define __RBTREE_EXPENSIVE_CHECK(tree)
#endif

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __rbtree_left_rotate(IN tt_rbnode_t *node);
static void __rbtree_right_rotate(IN tt_rbnode_t *node);

static void __rbtree_insert_fixup(IN tt_rbnode_t *node);
static void __rbtree_remove_fixup(IN tt_rbnode_t *node);

tt_result_t __rbtree_expensive_check(IN tt_rbnode_t *node,
                                     IN OUT tt_u32_t *black_node_num);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_rbtree_init(IN tt_rbtree_t *tree,
                    IN tt_cmp_t cmp,
                    IN tt_cmpkey_t cmpkey)
{
    tt_rbnode_t *tt_nil = NULL;

    TT_ASSERT(tree != NULL);
    TT_ASSERT(cmp != NULL);
    TT_ASSERT(cmpkey != NULL);

    tt_nil = &tree->tt_nil;

    tree->root = tt_nil;
    tree->node_num = 0;

    tree->cmp = cmp;
    tree->cmpkey = cmpkey;

    // init root
    tt_rbnode_init(tt_nil);
    tt_nil->color = TT_RBTREE_NODE_BLACK;
    tt_nil->parent = tt_nil;
    tt_nil->left = tt_nil;
    tt_nil->right = tt_nil;
    tt_nil->tree = tree;
}

void tt_rbnode_init(IN tt_rbnode_t *node)
{
    TT_ASSERT(node != NULL);

    node->color = TT_RBTREE_NODE_RED;

    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    node->tree = NULL;
}

tt_rbnode_t *tt_rbtree_find(IN tt_rbnode_t *begin_node,
                            IN tt_rbnode_t *key_node)
{
    tt_rbnode_t *cur_node = NULL;
    tt_cmp_t cmp = NULL;

    TT_ASSERT(begin_node != NULL);
    TT_ASSERT(key_node != NULL);
    TT_ASSERT(begin_node->tree != NULL);
    TT_ASSERT(begin_node->tree->cmp != NULL);

    cmp = begin_node->tree->cmp;

    cur_node = begin_node;
    while (!TT_RBTREE_NODE_NIL(cur_node)) {
        tt_s32_t comp_ret = 0;

        comp_ret = cmp(key_node, cur_node);
        if (comp_ret < 0) {
            cur_node = cur_node->left;
        } else if (comp_ret == 0) {
            return cur_node;
        } else {
            cur_node = cur_node->right;
        }
    }

    return NULL;
}

tt_rbnode_t *tt_rbtree_find_k(IN tt_rbnode_t *begin_node,
                              IN const tt_u8_t *key,
                              IN tt_u32_t key_len)
{
    tt_rbnode_t *cur_node = NULL;
    tt_cmpkey_t cmpkey = NULL;

    TT_ASSERT(begin_node != NULL);
    TT_ASSERT(key != NULL);
    TT_ASSERT(key_len != 0);
    TT_ASSERT(begin_node->tree != NULL);
    TT_ASSERT(begin_node->tree->cmpkey != NULL);

    cmpkey = begin_node->tree->cmpkey;

    cur_node = begin_node;
    while (!TT_RBTREE_NODE_NIL(cur_node)) {
        tt_s32_t comp_ret = 0;

        comp_ret = cmpkey(cur_node, key, key_len);
        if (comp_ret > 0) {
            cur_node = cur_node->left;
        } else if (comp_ret == 0) {
            return cur_node;
        } else {
            cur_node = cur_node->right;
        }
    }

    return NULL;
}

tt_rbnode_t *tt_rbtree_find_upper(IN tt_rbnode_t *begin_node,
                                  IN tt_rbnode_t *key_node)
{
    tt_rbnode_t *cur_node = NULL;
    tt_rbnode_t *cur_larger = NULL;
    tt_cmp_t cmp = NULL;

    TT_ASSERT(begin_node != NULL);
    TT_ASSERT(key_node != NULL);
    TT_ASSERT(begin_node->tree != NULL);
    TT_ASSERT(begin_node->tree->cmp != NULL);

    cmp = begin_node->tree->cmp;

    cur_node = begin_node;
    while (!TT_RBTREE_NODE_NIL(cur_node)) {
        tt_s32_t comp_ret = cmp(key_node, cur_node);
        if (comp_ret < 0) {
            cur_larger = cur_node;

            cur_node = cur_node->left;
        } else if (comp_ret == 0) {
            return cur_node;
        } else {
            cur_node = cur_node->right;
        }
    }

    return cur_larger;
}

tt_rbnode_t *tt_rbtree_find_upper_k(IN tt_rbnode_t *begin_node,
                                    IN const tt_u8_t *key,
                                    IN tt_u32_t key_len)
{
    tt_rbnode_t *cur_node = NULL;
    tt_rbnode_t *cur_larger = NULL;
    tt_cmpkey_t cmpkey = NULL;

    TT_ASSERT(begin_node != NULL);
    TT_ASSERT(key != NULL);
    TT_ASSERT(key_len != 0);
    TT_ASSERT(begin_node->tree != NULL);
    TT_ASSERT(begin_node->tree->cmpkey != NULL);

    cmpkey = begin_node->tree->cmpkey;

    cur_node = begin_node;
    while (!TT_RBTREE_NODE_NIL(cur_node)) {
        tt_s32_t comp_ret = cmpkey(cur_node, key, key_len);
        if (comp_ret > 0) {
            cur_larger = cur_node;

            cur_node = cur_node->left;
        } else if (comp_ret == 0) {
            return cur_node;
        } else {
            cur_node = cur_node->right;
        }
    }

    return cur_larger;
}

tt_rbnode_t *tt_rbtree_find_lower(IN tt_rbnode_t *begin_node,
                                  IN tt_rbnode_t *key_node)
{
    tt_rbnode_t *cur_node = NULL;
    tt_rbnode_t *cur_less = NULL;
    tt_cmp_t cmp = NULL;

    TT_ASSERT(begin_node != NULL);
    TT_ASSERT(key_node != NULL);
    TT_ASSERT(begin_node->tree != NULL);
    TT_ASSERT(begin_node->tree->cmp != NULL);

    cmp = begin_node->tree->cmp;

    cur_node = begin_node;
    while (!TT_RBTREE_NODE_NIL(cur_node)) {
        tt_s32_t comp_ret = cmp(key_node, cur_node);
        if (comp_ret < 0) {
            cur_node = cur_node->left;
        } else if (comp_ret == 0) {
            return cur_node;
        } else {
            cur_less = cur_node;

            cur_node = cur_node->right;
        }
    }

    return cur_less;
}

tt_rbnode_t *tt_rbtree_find_lower_k(IN tt_rbnode_t *begin_node,
                                    IN const tt_u8_t *key,
                                    IN tt_u32_t key_len)
{
    tt_rbnode_t *cur_node = NULL;
    tt_rbnode_t *cur_less = NULL;
    tt_cmpkey_t cmpkey = NULL;

    TT_ASSERT(begin_node != NULL);
    TT_ASSERT(key != NULL);
    TT_ASSERT(key_len != 0);
    TT_ASSERT(begin_node->tree != NULL);
    TT_ASSERT(begin_node->tree->cmpkey != NULL);

    cmpkey = begin_node->tree->cmpkey;

    cur_node = begin_node;
    while (!TT_RBTREE_NODE_NIL(cur_node)) {
        tt_s32_t comp_ret = cmpkey(cur_node, key, key_len);
        if (comp_ret > 0) {
            cur_node = cur_node->left;
        } else if (comp_ret == 0) {
            return cur_node;
        } else {
            cur_less = cur_node;

            cur_node = cur_node->right;
        }
    }

    return cur_less;
}


tt_rbnode_t *tt_rbtree_min(IN tt_rbnode_t *begin_node)
{
    tt_rbnode_t *last_node = NULL;
    tt_rbnode_t *cur_node = NULL;

    TT_ASSERT(begin_node != NULL);
    TT_ASSERT(begin_node->tree != NULL);

    cur_node = begin_node;
    while (!TT_RBTREE_NODE_NIL(cur_node)) {
        last_node = cur_node;
        cur_node = cur_node->left;
    }

    return last_node;
}

tt_rbnode_t *tt_rbtree_max(IN tt_rbnode_t *begin_node)
{
    tt_rbnode_t *last_node = NULL;
    tt_rbnode_t *cur_node = NULL;

    TT_ASSERT(begin_node != NULL);
    TT_ASSERT(begin_node->tree != NULL);

    cur_node = begin_node;
    while (!TT_RBTREE_NODE_NIL(cur_node)) {
        last_node = cur_node;
        cur_node = cur_node->right;
    }

    return last_node;
}

tt_rbnode_t *tt_rbtree_next(IN tt_rbnode_t *node)
{
    tt_rbnode_t *cur_node = NULL;
    tt_rbnode_t *parent_node = NULL;

    TT_ASSERT(node != NULL);
    TT_ASSERT(node->tree != NULL);

    if (!TT_RBTREE_NODE_NIL(node->right)) {
        return tt_rbtree_min(node->right);
    }

    cur_node = node;
    parent_node = node->parent;
    while (!TT_RBTREE_NODE_NIL(parent_node) &&
           (cur_node == parent_node->right)) {
        cur_node = parent_node;
        parent_node = parent_node->parent;
    }

    if (!TT_RBTREE_NODE_NIL(parent_node)) {
        return parent_node;
    } else {
        return NULL;
    }
}

tt_rbnode_t *tt_rbtree_prev(IN tt_rbnode_t *node)
{
    tt_rbnode_t *cur_node = NULL;
    tt_rbnode_t *parent_node = NULL;

    TT_ASSERT(node != NULL);
    TT_ASSERT(node->tree != NULL);

    if (!TT_RBTREE_NODE_NIL(node->left)) {
        return tt_rbtree_max(node->left);
    }

    cur_node = node;
    parent_node = node->parent;
    while (!TT_RBTREE_NODE_NIL(parent_node) &&
           (cur_node == parent_node->left)) {
        cur_node = parent_node;
        parent_node = parent_node->parent;
    }

    if (!TT_RBTREE_NODE_NIL(parent_node)) {
        return parent_node;
    } else {
        return NULL;
    }
}

void tt_rbtree_add(IN tt_rbtree_t *tree, IN tt_rbnode_t *node)
{
    tt_rbnode_t *node_x = NULL;
    tt_rbnode_t *node_y = NULL;

    TT_ASSERT(tree != NULL);
    TT_ASSERT(tree->cmp != NULL);
    TT_ASSERT(node != NULL);
    TT_ASSERT(node->tree == NULL);

    node_y = &tree->tt_nil;
    node_x = tree->root;

    // find a place to insert, the node will be inserted
    // as node_y's child
    while (!TT_RBTREE_NODE_NIL(node_x)) {
        node_y = node_x;
        if (tree->cmp(node, node_x) < 0) {
            node_x = node_x->left;
        } else {
            node_x = node_x->right;
        }
    }

    // insert node
    node->parent = node_y;
    if (TT_RBTREE_NODE_NIL(node_y)) {
        tree->root = node;
    } else {
        if (tree->cmp(node, node_y) < 0) {
            node_y->left = node;
        } else {
            node_y->right = node;
        }
    }

    // set other field of node
    node->left = &tree->tt_nil;
    node->right = &tree->tt_nil;
    node->color = TT_RBTREE_NODE_RED;
    node->tree = tree;

    // fixup rb tree;
    __rbtree_insert_fixup(node);

    // update tree's data
    tree->node_num++;

    __RBTREE_EXPENSIVE_CHECK(tree);
}

void tt_rbtree_remove(IN tt_rbnode_t *node)
{
    tt_rbnode_t *node_x = NULL;
    tt_rbnode_t *node_y = NULL;
    tt_rbnode_t *node_z = NULL;
    tt_rbtree_t *tree = NULL;
    tt_u8_t deleted_color = 0;

    TT_ASSERT(node != NULL);

    if (node->tree == NULL) {
        return;
    }

    node_z = node;
    tree = node->tree;

    // find the node which is really to be deleted
    if (TT_RBTREE_NODE_NIL(node_z->left) || TT_RBTREE_NODE_NIL(node_z->right)) {
        node_y = node_z;
    } else {
        // node_z would not be NULL for it has at lease
        // a right child
        node_y = tt_rbtree_next(node_z);
    }

    if (!TT_RBTREE_NODE_NIL(node_y->left)) {
        node_x = node_y->left;
    } else {
        node_x = node_y->right;
    }

    // delink node_y
    node_x->parent = node_y->parent;
    if (TT_RBTREE_NODE_NIL(node_y->parent)) {
        tree->root = node_x;
    } else {
        if (node_y == node_y->parent->left) {
            node_y->parent->left = node_x;
        } else {
            node_y->parent->right = node_x;
        }
    }

    deleted_color = node_y->color;

    if (node_y != node_z) {
        tt_memcpy(node_y, node_z, sizeof(tt_rbnode_t));

        if (TT_RBTREE_NODE_NIL(node_z->parent)) {
            tree->root = node_y;
        } else {
            if (node_z == node_z->parent->left) {
                node_z->parent->left = node_y;
            } else {
                node_z->parent->right = node_y;
            }
        }

        if (!TT_RBTREE_NODE_NIL(node_z->left)) {
            node_z->left->parent = node_y;
        }

        if (!TT_RBTREE_NODE_NIL(node_z->right)) {
            node_z->right->parent = node_y;
        }

        // special handle, for node_x may be NIL
        // if (TT_RBTREE_NODE_NIL(node_x))
        //{
        //    node_x->parent = node_y;
        //}

        // if node_x is tt_nil, then its parent won't be changed to node_y
        // by above code
        if (node_x->parent == node_z) {
            node_x->parent = node_y;
        }
    }

    if (deleted_color == TT_RBTREE_NODE_BLACK) {
        __rbtree_remove_fixup(node_x);
    }

    tree->node_num--;

    // clear node_z's data
    node->tree = NULL;
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;

    __RBTREE_EXPENSIVE_CHECK(tree);
}

void __rbtree_left_rotate(IN tt_rbnode_t *node)
{
    tt_rbnode_t *node_x = NULL;
    tt_rbnode_t *node_y = NULL;
    tt_rbtree_t *tree = NULL;

    TT_ASSERT(node != NULL);
    TT_ASSERT(node->tree != NULL);

    if (TT_RBTREE_NODE_NIL(node->right)) {
        return;
    }

    //     \                    |
    //     [x] <-- node         |
    //     /  \                 |
    //   [a]  [y]               |
    //        /  \              |
    //      [b]  [c]            |
    node_x = node;
    node_y = node->right;
    tree = node->tree;

    // [x] <==> [b]
    node_x->right = node_y->left;
    if (!TT_RBTREE_NODE_NIL(node_y->left)) {
        node_y->left->parent = node_x;
    }

    // [y] <==> [x]'parent
    node_y->parent = node_x->parent;
    if (TT_RBTREE_NODE_NIL(node_x->parent)) {
        // update tree's root
        tree->root = node_y;
    } else {
        if (node_x == node_x->parent->left) {
            node_x->parent->left = node_y;
        } else {
            node_x->parent->right = node_y;
        }
    }

    // [y] <==> [x]
    node_y->left = node_x;
    node_x->parent = node_y;

    //     \         |
    //     [y]       |
    //     /  \      |
    //   [x]  [c]    |
    //   /  \        |
    // [a]  [b]      |
}

void __rbtree_right_rotate(IN tt_rbnode_t *node)
{
    tt_rbnode_t *node_x = NULL;
    tt_rbnode_t *node_y = NULL;
    tt_rbtree_t *tree = NULL;

    TT_ASSERT(node != NULL);
    TT_ASSERT(node->tree != NULL);

    if (TT_RBTREE_NODE_NIL(node->left)) {
        return;
    }

    //     \              |
    //     [x] <-- node   |
    //     /  \           |
    //   [y]  [c]         |
    //   /  \             |
    // [a]  [b]           |
    node_x = node;
    node_y = node->left;
    tree = node->tree;

    // [x] <==> [b]
    node_x->left = node_y->right;
    if (!TT_RBTREE_NODE_NIL(node_y->right)) {
        node_y->right->parent = node_x;
    }

    // [y] <==> [x]'parent
    node_y->parent = node_x->parent;
    if (TT_RBTREE_NODE_NIL(node_x->parent)) {
        // update tree's root
        tree->root = node_y;
    } else {
        if (node_x == node_x->parent->left) {
            node_x->parent->left = node_y;
        } else {
            node_x->parent->right = node_y;
        }
    }

    // [y] <==> [x]
    node_y->right = node_x;
    node_x->parent = node_y;

    //     \           |
    //     [y]         |
    //     /  \        |
    //   [a]  [x]      |
    //        /  \     |
    //      [b]  [c]   |
}

void __rbtree_insert_fixup(IN tt_rbnode_t *node)
{
    tt_rbnode_t *node_y = NULL;
    tt_rbnode_t *node_z = NULL;
    tt_rbtree_t *tree = NULL;

    TT_ASSERT(node != NULL);
    TT_ASSERT(node->tree != NULL);

    node_z = node;
    tree = node->tree;
    while (node_z->parent->color == TT_RBTREE_NODE_RED) {
        if (node_z->parent == node_z->parent->parent->left) {
            node_y = node_z->parent->parent->right;
            if (node_y->color == TT_RBTREE_NODE_RED) {
                node_z->parent->color = TT_RBTREE_NODE_BLACK;
                node_y->color = TT_RBTREE_NODE_BLACK;
                node_z->parent->parent->color = TT_RBTREE_NODE_RED;

                node_z = node_z->parent->parent;
            } else {
                if (node_z == node_z->parent->right) {
                    node_z = node_z->parent;
                    __rbtree_left_rotate(node_z);
                }

                node_z->parent->color = TT_RBTREE_NODE_BLACK;
                node_z->parent->parent->color = TT_RBTREE_NODE_RED;
                __rbtree_right_rotate(node_z->parent->parent);
            }
        } else {
            node_y = node_z->parent->parent->left;
            if (node_y->color == TT_RBTREE_NODE_RED) {
                node_z->parent->color = TT_RBTREE_NODE_BLACK;
                node_y->color = TT_RBTREE_NODE_BLACK;
                node_z->parent->parent->color = TT_RBTREE_NODE_RED;

                node_z = node_z->parent->parent;
            } else {
                if (node_z == node_z->parent->left) {
                    node_z = node_z->parent;
                    __rbtree_right_rotate(node_z);
                }

                node_z->parent->color = TT_RBTREE_NODE_BLACK;
                node_z->parent->parent->color = TT_RBTREE_NODE_RED;
                __rbtree_left_rotate(node_z->parent->parent);
            }
        }
    }

    tree->root->color = TT_RBTREE_NODE_BLACK;
}

void __rbtree_remove_fixup(IN tt_rbnode_t *node)
{
    tt_rbnode_t *node_x = NULL;
    tt_rbnode_t *node_w = NULL;
    tt_rbtree_t *tree = NULL;

    TT_ASSERT(node != NULL);
    TT_ASSERT(node->tree != NULL);

    node_x = node;
    tree = node->tree;

    while ((node_x != tree->root) && (node_x->color == TT_RBTREE_NODE_BLACK)) {
        if (node_x == node_x->parent->left) {
            node_w = node_x->parent->right;
            if (node_w->color == TT_RBTREE_NODE_RED) {
                node_w->color = TT_RBTREE_NODE_BLACK;
                node_x->parent->color = TT_RBTREE_NODE_RED;
                __rbtree_left_rotate(node_x->parent);
                node_w = node_x->parent->right;
            }

            if ((node_w->left->color == TT_RBTREE_NODE_BLACK) &&
                (node_w->right->color == TT_RBTREE_NODE_BLACK)) {
                node_w->color = TT_RBTREE_NODE_RED;
                node_x = node_x->parent;
            } else {
                if (node_w->right->color == TT_RBTREE_NODE_BLACK) {
                    node_w->left->color = TT_RBTREE_NODE_BLACK;
                    node_w->color = TT_RBTREE_NODE_RED;
                    __rbtree_right_rotate(node_w);
                    node_w = node_x->parent->right;
                }

                node_w->color = node_x->parent->color;
                node_x->parent->color = TT_RBTREE_NODE_BLACK;
                node_w->right->color = TT_RBTREE_NODE_BLACK;
                __rbtree_left_rotate(node_x->parent);

                node_x = tree->root;
            }
        } else {
            node_w = node_x->parent->left;
            if (node_w->color == TT_RBTREE_NODE_RED) {
                node_w->color = TT_RBTREE_NODE_BLACK;
                node_x->parent->color = TT_RBTREE_NODE_RED;
                __rbtree_right_rotate(node_x->parent);
                node_w = node_x->parent->left;
            }

            if ((node_w->left->color == TT_RBTREE_NODE_BLACK) &&
                (node_w->right->color == TT_RBTREE_NODE_BLACK)) {
                node_w->color = TT_RBTREE_NODE_RED;
                node_x = node_x->parent;
            } else {
                if (node_w->left->color == TT_RBTREE_NODE_BLACK) {
                    node_w->right->color = TT_RBTREE_NODE_BLACK;
                    node_w->color = TT_RBTREE_NODE_RED;
                    __rbtree_left_rotate(node_w);
                    node_w = node_x->parent->left;
                }

                node_w->color = node_x->parent->color;
                node_x->parent->color = TT_RBTREE_NODE_BLACK;
                node_w->left->color = TT_RBTREE_NODE_BLACK;
                __rbtree_right_rotate(node_x->parent);

                node_x = tree->root;
            }
        }
    }

    node_x->color = TT_RBTREE_NODE_BLACK;
}

tt_result_t __rbtree_expensive_check(IN tt_rbnode_t *node,
                                     OUT tt_u32_t *black_node_num)
{
    tt_u32_t left_black_node_num = 0;
    tt_u32_t right_black_node_num = 0;

    if (TT_RBTREE_NODE_NIL(node)) {
        // for tt_nil, only check color
        if (node->color != TT_RBTREE_NODE_BLACK) {
            TT_ERROR("tt_nil must be black");
            return TT_FAIL;
        }

        if (black_node_num != NULL) {
            *black_node_num = 1;
        }
        return TT_SUCCESS;
    }

    // 1. either black or red
    if ((node->color != TT_RBTREE_NODE_BLACK) &&
        (node->color != TT_RBTREE_NODE_RED)) {
        TT_ERROR("unknown color: %d", node->color);
        return TT_FAIL;
    }

    // 2. root must be black
    if (node == node->tree->root) {
        if (node->color != TT_RBTREE_NODE_BLACK) {
            TT_ERROR("root must be black");
            return TT_FAIL;
        }
    }

// 3. leaf must be black
#if 0 // if tt_nil is black, that's enough
    if (TT_RBTREE_NODE_NIL(node->left) && TT_RBTREE_NODE_NIL(node->right))
    {
        if (node->color != TT_RBTREE_NODE_BLACK)
        {
            TT_ERROR("leaf must be black");
            return TT_FAIL;
        }
    }
#endif

    // 4. red node's sons must be both black
    if (node->color == TT_RBTREE_NODE_RED) {
        if (node->left->color != TT_RBTREE_NODE_BLACK) {
            TT_ERROR("red node's left son is not black");
            return TT_FAIL;
        }
        if (node->right->color != TT_RBTREE_NODE_BLACK) {
            TT_ERROR("red node's right son is not black");
            return TT_FAIL;
        }
    }

    // 5. left black node num should equal right num
    if (!TT_OK(__rbtree_expensive_check(node->left, &left_black_node_num)) ||
        !TT_OK(__rbtree_expensive_check(node->right, &right_black_node_num))) {
        return TT_FAIL;
    }
    if (left_black_node_num != right_black_node_num) {
        TT_ERROR("left black node[%d] != right black node[%d]",
                 left_black_node_num,
                 right_black_node_num);
        return TT_FAIL;
    }

    // all checking passed
    if (node->color == TT_RBTREE_NODE_BLACK) {
        left_black_node_num += 1;
    }
    if (black_node_num != NULL) {
        *black_node_num = left_black_node_num;
    }
    return TT_SUCCESS;
}
