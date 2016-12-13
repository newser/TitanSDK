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

/**
@file tt_config_group.h
@brief config option of group type

this file defines config option of group type
*/

#ifndef __TT_CONFIG_GROUP__
#define __TT_CONFIG_GROUP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <init/tt_config_node.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_cfggrp_s;

// callback
typedef tt_cfgnode_t *(*tt_cfggrp_create_child_t)(IN struct tt_cfggrp_s *cgrp,
                                                  IN tt_blob_t *name,
                                                  IN tt_blob_t *val);

typedef tt_bool_t (*tt_cfggrp_on_add_child_t)(IN struct tt_cfgnode_s *cnode,
                                              IN struct tt_cfgnode_s *child);

typedef tt_bool_t (*tt_cfggrp_on_rm_child_t)(IN struct tt_cfgnode_s *cnode,
                                             IN struct tt_cfgnode_s *child);

typedef struct tt_cfggrp_cb_s
{
    tt_cfggrp_create_child_t create_child;
    tt_cfggrp_on_add_child_t on_add_child;
    tt_cfggrp_on_rm_child_t on_rm_child;
} tt_cfggrp_cb_t;

typedef struct
{
    tt_cfgnode_attr_t cnode_attr;

    tt_cfggrp_mode_t mode;
} tt_cfggrp_attr_t;

typedef struct tt_cfggrp_s
{
    tt_list_t child;
    tt_list_t new_child;
    tt_u32_t child_name_len;

    tt_cfggrp_cb_t *cb;
} tt_cfggrp_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_cfgnode_t *tt_cfggrp_create(IN const tt_char_t *name,
                                      IN OPT tt_cfgnode_itf_t *itf,
                                      IN OPT void *opaque,
                                      IN OPT tt_cfggrp_cb_t *cb,
                                      IN OPT tt_cfggrp_attr_t *attr);

extern void tt_cfggrp_attr_default(IN tt_cfggrp_attr_t *attr);

extern tt_result_t tt_cfggrp_add(IN tt_cfgnode_t *cnode,
                                 IN tt_cfgnode_t *child);

tt_inline void tt_cfggrp_remove(IN tt_cfgnode_t *cnode, IN tt_cfgnode_t *child)
{
    tt_cfggrp_t *cgrp;

    TT_ASSERT(cnode->type == TT_CFGNODE_TYPE_GROUP);
    cgrp = TT_CFGNODE_CAST(cnode, tt_cfggrp_t);

    TT_ASSERT(child->node.lst == &cgrp->child);
    tt_list_remove(&child->node);
}

extern tt_cfgnode_t *tt_cfggrp_find(IN tt_cfgnode_t *cnode,
                                    IN const tt_char_t *name);

extern tt_cfgnode_t *tt_cfggrp_find_blob(IN tt_cfgnode_t *cnode,
                                         IN tt_blob_t *name);

// ========================================
// interface
// ========================================

extern tt_result_t tt_cfggrp_add_child(IN tt_cfgnode_t *cnode,
                                       IN tt_blob_t *name,
                                       IN tt_blob_t *val);

extern tt_result_t tt_cfggrp_rm_child(IN tt_cfgnode_t *cnode,
                                      IN tt_blob_t *name);

extern tt_result_t tt_cfggrp_ls(IN tt_cfgnode_t *cnode,
                                IN const tt_char_t *seperator,
                                OUT struct tt_buf_s *output);

extern tt_result_t tt_cfggrp_commit(IN tt_cfgnode_t *cnode);

#endif /* __TT_CONFIG_GROUP__ */
