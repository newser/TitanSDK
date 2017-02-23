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

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#ifdef TT_RBTREE_SANITY_CHECK
#define TT_ASSERT_RB TT_ASSERT
#define __RBT_EXPENSIVE_CHECK(tree)                                            \
    TT_ASSERT_ALWAYS(TT_OK(__rbt_expensive_check(tree, (tree)->root, NULL)))
#else
#define TT_ASSERT_RB(...)
#define __RBT_EXPENSIVE_CHECK(tree)
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

static void __rbt_left_rotate(IN tt_rbtree_t *tree, IN tt_rbnode_t *node);

static void __rbt_right_rotate(IN tt_rbtree_t *tree, IN tt_rbnode_t *node);

static void __rbt_add_fixup(IN tt_rbtree_t *tree, IN tt_rbnode_t *node);

static void __rbt_remove_fixup(IN tt_rbtree_t *tree, IN tt_rbnode_t *node);

static tt_s32_t __rbn_cmpkey(IN void *p, IN tt_u8_t *key, IN tt_u32_t key_len);

tt_result_t __rbt_expensive_check(IN tt_rbtree_t *tree,
                                  IN tt_rbnode_t *node,
                                  OUT tt_u32_t *bh);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_inline tt_rbnode_t *__rbt_min(IN tt_rbtree_t *tree, IN tt_rbnode_t *from)
{
    TT_ASSERT(from != &tree->tnil);
    while (from->left != &tree->tnil) {
        from = from->left;
    }
    return from;
}

tt_inline tt_rbnode_t *__rbt_max(IN tt_rbtree_t *tree, IN tt_rbnode_t *from)
{
    TT_ASSERT(from != &tree->tnil);
    while (from->right != &tree->tnil) {
        from = from->right;
    }
    return from;
}

tt_inline void __rbn_transplant(IN tt_rbtree_t *tree,
                                IN tt_rbnode_t *u,
                                IN tt_rbnode_t *v)
{
    if (u->parent == &tree->tnil) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

void tt_rbnode_init(IN tt_rbnode_t *node)
{
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    node->key = NULL;
    node->key_len = 0;

    node->black = TT_FALSE;
}

void tt_rbtree_init(IN tt_rbtree_t *tree, IN OPT tt_rbtree_attr_t *attr)
{
    tt_rbtree_attr_t __attr;

    if (attr == NULL) {
        tt_rbtree_attr_default(&__attr);
        attr = &__attr;
    }

    tree->root = &tree->tnil;
    tree->cmpkey = attr->cmpkey;

    tt_rbnode_init(&tree->tnil);
    tree->tnil.black = TT_TRUE;

    tree->count = 0;
}

void tt_rbtree_attr_default(IN tt_rbtree_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    attr->cmpkey = __rbn_cmpkey;
}

tt_rbnode_t *tt_rbtree_next(IN tt_rbtree_t *tree, IN tt_rbnode_t *node)
{
    tt_rbnode_t *parent;

    if (node->right != &tree->tnil) {
        return __rbt_min(tree, node->right);
    }

    parent = node->parent;
    while ((parent != &tree->tnil) && (node == parent->right)) {
        node = parent;
        parent = parent->parent;
    }
    return TT_COND(parent != &tree->tnil, parent, NULL);
}

tt_rbnode_t *tt_rbtree_prev(IN tt_rbtree_t *tree, IN tt_rbnode_t *node)
{
    tt_rbnode_t *parent;

    if (node->left != &tree->tnil) {
        return __rbt_max(tree, node->left);
    }

    parent = node->parent;
    while ((parent != &tree->tnil) && (node == parent->left)) {
        node = parent;
        parent = parent->parent;
    }
    return TT_COND(parent != &tree->tnil, parent, NULL);
}

tt_rbnode_t *tt_rbtree_find(IN tt_rbtree_t *tree,
                            IN tt_u8_t *key,
                            IN tt_u32_t key_len)
{
    tt_rbnode_t *node = tree->root;
    while (node != &tree->tnil) {
        tt_s32_t r = tree->cmpkey(node, key, key_len);
        if (r > 0) {
            node = node->left;
        } else if (r == 0) {
            return node;
        } else {
            node = node->right;
        }
    }
    return NULL;
}

tt_rbnode_t *tt_rbtree_find_gteq(IN tt_rbtree_t *tree,
                                 IN tt_u8_t *key,
                                 IN tt_u32_t key_len)
{
    tt_rbnode_t *node = tree->root;
    tt_rbnode_t *gt = NULL;
    while (node != &tree->tnil) {
        tt_s32_t r = tree->cmpkey(node, key, key_len);
        if (r > 0) {
            gt = node;
            node = node->left;
        } else if (r == 0) {
            return node;
        } else {
            node = node->right;
        }
    }
    return gt;
}

tt_rbnode_t *tt_rbtree_find_lteq(IN tt_rbtree_t *tree,
                                 IN tt_u8_t *key,
                                 IN tt_u32_t key_len)
{
    tt_rbnode_t *node = tree->root;
    tt_rbnode_t *lt = NULL;
    while (node != &tree->tnil) {
        tt_s32_t r = tree->cmpkey(node, key, key_len);
        if (r > 0) {
            node = node->left;
        } else if (r == 0) {
            return node;
        } else {
            lt = node;
            node = node->right;
        }
    }
    return lt;
}

tt_rbnode_t *tt_rbtree_min(IN tt_rbtree_t *tree)
{
    return TT_COND(tree->root != &tree->tnil,
                   __rbt_min(tree, tree->root),
                   NULL);
}

tt_rbnode_t *tt_rbtree_max(IN tt_rbtree_t *tree)
{
    return TT_COND(tree->root != &tree->tnil,
                   __rbt_max(tree, tree->root),
                   NULL);
}

void tt_rbtree_add(IN tt_rbtree_t *tree,
                   IN tt_u8_t *key,
                   IN tt_u32_t key_len,
                   IN tt_rbnode_t *z)
{
    tt_rbnode_t *x = tree->root;
    tt_rbnode_t *y = &tree->tnil;
    tt_bool_t left;

    TT_ASSERT(z->parent == NULL);

    // find place to insert, node will be inserted as y's child
    while (x != &tree->tnil) {
        y = x;
        if (tree->cmpkey(x, key, key_len) > 0) {
            x = x->left;
            left = TT_TRUE;
        } else {
            x = x->right;
            left = TT_FALSE;
        }
    }

    // insert
    z->parent = y;
    if (y == &tree->tnil) {
        tree->root = z;
    } else {
        if (left) {
            y->left = z;
        } else {
            y->right = z;
        }
    }

    z->left = &tree->tnil;
    z->right = &tree->tnil;
    z->key = key;
    z->key_len = key_len;
    z->black = TT_FALSE;

    // fixup
    __rbt_add_fixup(tree, z);

    ++tree->count;
    __RBT_EXPENSIVE_CHECK(tree);
}

void tt_rbtree_remove(IN tt_rbtree_t *tree, IN tt_rbnode_t *z)
{
    tt_bool_t black;
    tt_rbnode_t *x;

    if (z->parent == NULL) {
        return;
    }

    if (z->left == &tree->tnil) {
        black = z->black;
        x = z->right;
        __rbn_transplant(tree, z, z->right);
    } else if (z->right == &tree->tnil) {
        black = z->black;
        x = z->left;
        __rbn_transplant(tree, z, z->left);
    } else {
        tt_rbnode_t *y = __rbt_min(tree, z->right);
        black = y->black;

        x = y->right;
        if (y->parent == z) {
            // x may be nil
            x->parent = y;
        } else {
            __rbn_transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        __rbn_transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->black = z->black;
    }

    if (black) {
        __rbt_remove_fixup(tree, x);
    }

    tt_rbnode_init(z);

    --tree->count;
    __RBT_EXPENSIVE_CHECK(tree);
}

void __rbt_left_rotate(IN tt_rbtree_t *tree, IN tt_rbnode_t *x)
{
    tt_rbnode_t *y = x->right;

#if 1
    TT_ASSERT(y != &tree->tnil);
#else
    if (y == &tree->tnil) {
        return;
    }
#endif

    //     \                    |
    //     [x] <-- node         |
    //     /  \                 |
    //   [a]  [y]               |
    //        /  \              |
    //      [b]  [c]            |

    // [x] <==> [b]
    x->right = y->left;
    if (y->left != &tree->tnil) {
        y->left->parent = x;
    }

    // [y] <==> [x]'parent
    y->parent = x->parent;
    if (x->parent == &tree->tnil) {
        tree->root = y;
    } else {
        if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
    }

    // [y] <==> [x]
    y->left = x;
    x->parent = y;

    //     \         |
    //     [y]       |
    //     /  \      |
    //   [x]  [c]    |
    //   /  \        |
    // [a]  [b]      |
}

void __rbt_right_rotate(IN tt_rbtree_t *tree, IN tt_rbnode_t *x)
{
    tt_rbnode_t *y = x->left;

#if 1
    TT_ASSERT(y != &tree->tnil);
#else
    if (y == &tree->tnil) {
        return;
    }
#endif

    //     \              |
    //     [x] <-- node   |
    //     /  \           |
    //   [y]  [c]         |
    //   /  \             |
    // [a]  [b]           |

    // [x] <==> [b]
    x->left = y->right;
    if (y->right != &tree->tnil) {
        y->right->parent = x;
    }

    // [y] <==> [x]'parent
    y->parent = x->parent;
    if (x->parent == &tree->tnil) {
        tree->root = y;
    } else {
        if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
    }

    // [y] <==> [x]
    y->right = x;
    x->parent = y;

    //     \           |
    //     [y]         |
    //     /  \        |
    //   [a]  [x]      |
    //        /  \     |
    //      [b]  [c]   |
}

void __rbt_add_fixup(IN tt_rbtree_t *tree, IN tt_rbnode_t *z)
{
    tt_rbnode_t *y;

    while (!z->parent->black) {
        if (z->parent == z->parent->parent->left) {
            y = z->parent->parent->right;
            if (!y->black) {
                z->parent->black = TT_TRUE;
                y->black = TT_TRUE;
                z->parent->parent->black = TT_FALSE;

                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    __rbt_left_rotate(tree, z);
                }

                z->parent->black = TT_TRUE;
                z->parent->parent->black = TT_FALSE;
                __rbt_right_rotate(tree, z->parent->parent);
            }
        } else {
            y = z->parent->parent->left;
            if (!y->black) {
                z->parent->black = TT_TRUE;
                y->black = TT_TRUE;
                z->parent->parent->black = TT_FALSE;

                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    __rbt_right_rotate(tree, z);
                }

                z->parent->black = TT_TRUE;
                z->parent->parent->black = TT_FALSE;
                __rbt_left_rotate(tree, z->parent->parent);
            }
        }
    }

    tree->root->black = TT_TRUE;
}

void __rbt_remove_fixup(IN tt_rbtree_t *tree, IN tt_rbnode_t *x)
{
    tt_rbnode_t *w;

    while ((x != tree->root) && x->black) {
        if (x == x->parent->left) {
            w = x->parent->right;
            if (!w->black) {
                w->black = TT_TRUE;
                x->parent->black = TT_FALSE;
                __rbt_left_rotate(tree, x->parent);
                w = x->parent->right;
            }

            if (w->left->black && w->right->black) {
                w->black = TT_FALSE;
                x = x->parent;
            } else {
                if (w->right->black) {
                    w->left->black = TT_TRUE;
                    w->black = TT_FALSE;
                    __rbt_right_rotate(tree, w);
                    w = x->parent->right;
                }

                w->black = x->parent->black;
                x->parent->black = TT_TRUE;
                w->right->black = TT_TRUE;
                __rbt_left_rotate(tree, x->parent);

                x = tree->root;
            }
        } else {
            w = x->parent->left;
            if (!w->black) {
                w->black = TT_TRUE;
                x->parent->black = TT_FALSE;
                __rbt_right_rotate(tree, x->parent);
                w = x->parent->left;
            }

            if (w->left->black && w->right->black) {
                w->black = TT_FALSE;
                x = x->parent;
            } else {
                if (w->left->black) {
                    w->right->black = TT_TRUE;
                    w->black = TT_FALSE;
                    __rbt_left_rotate(tree, w);
                    w = x->parent->left;
                }

                w->black = x->parent->black;
                x->parent->black = TT_TRUE;
                w->left->black = TT_TRUE;
                __rbt_right_rotate(tree, x->parent);

                x = tree->root;
            }
        }
    }

    x->black = TT_TRUE;
}

tt_s32_t __rbn_cmpkey(IN void *p, IN tt_u8_t *key, IN tt_u32_t key_len)
{
    tt_rbnode_t *node = (tt_rbnode_t *)p;
    if (node->key_len < key_len) {
        return -1;
    } else if (node->key_len == key_len) {
        return tt_memcmp(node->key, key, key_len);
    } else {
        return 1;
    }
}

tt_result_t __rbt_expensive_check(IN tt_rbtree_t *tree,
                                  IN tt_rbnode_t *node,
                                  OUT tt_u32_t *bh)
{
    tt_u32_t left_bh = 0;
    tt_u32_t right_bh = 0;

    if (node == &tree->tnil) {
        if (!node->black) {
            TT_ERROR("nil must be black");
            return TT_FAIL;
        }

        TT_SAFE_ASSIGN(bh, 1);
        return TT_SUCCESS;
    }

    if (node == tree->root) {
        if (!node->black) {
            TT_ERROR("root must be black");
            return TT_FAIL;
        }
    }

    // red node's children must be both black
    if (!node->black) {
        if (!node->left->black) {
            TT_ERROR("red node's left son is not black");
            return TT_FAIL;
        }
        if (!node->right->black) {
            TT_ERROR("red node's right son is not black");
            return TT_FAIL;
        }
    }

    // left black node num should equal right num
    if (!TT_OK(__rbt_expensive_check(tree, node->left, &left_bh)) ||
        !TT_OK(__rbt_expensive_check(tree, node->right, &right_bh))) {
        return TT_FAIL;
    }
    if (left_bh != right_bh) {
        TT_ERROR("left black node[%d] != right black node[%d]",
                 left_bh,
                 right_bh);
        return TT_FAIL;
    }

    // all checking passed
    if (node->black) {
        TT_SAFE_ASSIGN(bh, left_bh + 1);
    } else {
        TT_SAFE_ASSIGN(bh, left_bh);
    }

    return TT_SUCCESS;
}

// this function is for debugging purpose
tt_u32_t __rbt_count(IN tt_rbtree_t *rbt, IN tt_rbnode_t *node)
{
    tt_u32_t n;

    if (node == &rbt->tnil) {
        return 0;
    }

    return __rbt_count(rbt, node->left) + __rbt_count(rbt, node->right) + 1;
}
