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

/**
@file tt_red_black_tree.h
@brief red black tree data structure

this file defines red black tree data structure and related operations
*/

#ifndef __TT_RED_BLACK_TREE__
#define __TT_RED_BLACK_TREE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_compare.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

/**
@struct tt_rbnode_t
a node in a red black tree
*/
typedef struct tt_rbnode_s
{
    struct tt_rbnode_s *parent;
    struct tt_rbnode_s *left;
    struct tt_rbnode_s *right;

    tt_u8_t *key;
    tt_u32_t key_len;

    tt_bool_t black : 1;
} tt_rbnode_t;

typedef struct
{
    tt_cmpkey_t cmpkey;
} tt_rbtree_attr_t;

/**
@struct tt_rbtree_t
a red black tree
*/
typedef struct tt_rbtree_s
{
    tt_rbnode_t *root;
    tt_cmpkey_t cmpkey;
    tt_rbnode_t tnil;
    tt_u32_t count;
} tt_rbtree_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
 @fn void tt_rbnode_init(IN tt_rbnode_t *node)
 initialize a red black tree node

 @param [in] node red black tree node to be initialized

 @return
 void

 @note
 there is no memory operation in such function, it only
 initialize the struct passed in by param
 */
tt_export void tt_rbnode_init(IN tt_rbnode_t *node);

/**
@fn void tt_rbtree_init(IN tt_rbtree_t *tree,
                        IN tt_cmp_t comparer)
initialize a red black tree

@param [in] tree red black tree to be initialized
@param [in] comparer number of elements

@return
void

@note
there is no memory operation in such function, it only
initialize the struct passed in by param
*/
tt_export void tt_rbtree_init(IN tt_rbtree_t *tree,
                              IN OPT tt_rbtree_attr_t *attr);

tt_export void tt_rbtree_attr_default(IN tt_rbtree_attr_t *attr);

/**
 @def tt_rbtree_root(rbtree)
 get root of rbtree
 */
tt_inline tt_rbnode_t *tt_rbtree_root(IN tt_rbtree_t *tree)
{
    return tree->root != &tree->tnil ? tree->root : NULL;
}

tt_inline tt_u32_t tt_rbtree_count(IN tt_rbtree_t *tree)
{
    return tree->count;
}

tt_inline tt_bool_t tt_rbtree_empty(IN tt_rbtree_t *tree)
{
    return tree->count == 0 ? TT_TRUE : TT_FALSE;
}

/**
 @fn tt_rbnode_t* tt_rbtree_next(IN tt_rbnode_t *node)
 find successor node in rbtree

 @param [in] node the node of which the successor is searching

 @return
 - the successor node in rbtree
 - NULL if the node specified by param has no successor
 */
tt_export tt_rbnode_t *tt_rbtree_next(IN tt_rbtree_t *tree,
                                      IN tt_rbnode_t *node);

/**
 @fn tt_rbnode_t* tt_rbtree_prev(IN tt_rbnode_t *node)
 find predecessor node in rbtree

 @param [in] node the node of which the predecessor is searching

 @return
 - the predecessor node in rbtree
 - NULL if the node specified by param has no predecessor
 */
tt_export tt_rbnode_t *tt_rbtree_prev(IN tt_rbtree_t *tree,
                                      IN tt_rbnode_t *node);

/**
 @fn tt_rbnode_t* tt_rbtree_find(IN tt_rbnode_t *from,
 IN tt_u8_t *key,
 IN tt_u32_t key_len)
search a node in rbtree

@param [in] begin_node from which node the searching begins
@param [in] key_node the node which has key value

@return
- the matching node if found
- NULL if no matching node
*/
tt_export tt_rbnode_t *tt_rbtree_find(IN tt_rbtree_t *tree, IN tt_u8_t *key,
                                      IN tt_u32_t key_len);

/**
@fn tt_rbnode_t* tt_rbtree_find_gteq( \
                                            IN tt_rbnode_t *begin_node,
                                            IN tt_rbnode_t *key_node)
search a node in rbtree, which is minimum among all nodes with larger value
than key specified

@param [in] begin_node from which node the searching begins
@param [in] key_node the node which has key value

@return
- the minimum node in tree with larger value
- NULL if no larger node
*/
tt_export tt_rbnode_t *tt_rbtree_find_gteq(IN tt_rbtree_t *tree,
                                           IN tt_u8_t *key,
                                           IN tt_u32_t key_len);

/**
@fn tt_rbnode_t* tt_rbtree_find_lteq( \
                                            IN tt_rbnode_t *begin_node,
                                            IN tt_rbnode_t *key_node)
search a node in rbtree, which is maximum among all nodes with less value
than key specified

@param [in] begin_node from which node the searching begins
@param [in] key_node the node which has key value

@return
- the maximum node in tree with less value
- NULL if no less node
*/
tt_export tt_rbnode_t *tt_rbtree_find_lteq(IN tt_rbtree_t *tree,
                                           IN tt_u8_t *key,
                                           IN tt_u32_t key_len);

/**
@fn tt_rbnode_t* tt_rbtree_min(IN tt_rbnode_t *begin_node)
get minimum node in rbtree

@param [in] begin_node root node of the sub rbtree

@return
- the minimum node in rbtree
- NULL if the tree is empty
*/
tt_export tt_rbnode_t *tt_rbtree_min(IN tt_rbtree_t *tree);

/**
@fn tt_rbnode_t* tt_rbtree_max(IN tt_rbnode_t *begin_node)
get maximum node in rbtree

@param [in] begin_node root node of the sub rbtree

@return
- the maximum node in rbtree
- NULL if the tree is empty
*/
tt_export tt_rbnode_t *tt_rbtree_max(IN tt_rbtree_t *tree);

/**
@fn void tt_rbtree_add(IN tt_rbtree_t *tree,
                               IN tt_rbnode_t *node)
insert a node to rbtree

@param [in] tree the rbtree
@param [in] node the node to be inserted to rbtree

@return
void
*/
tt_export void tt_rbtree_add(IN tt_rbtree_t *tree, IN tt_u8_t *key,
                             IN tt_u32_t key_len, IN tt_rbnode_t *node);

/**
@fn void tt_rbtree_remove(IN tt_rbnode_t *node)
remove a node from rbtree

@param [in] node the node to be removed from rbtree

@return
void
*/
tt_export void tt_rbtree_remove(IN tt_rbtree_t *tree, IN tt_rbnode_t *node);

#endif /* __TT_RED_BLACK_TREE__ */
