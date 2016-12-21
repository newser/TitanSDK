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
@file tt_config_node.h
@brief config node

this file defines config node
*/

#ifndef __TT_CONFIG_NODE__
#define __TT_CONFIG_NODE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_list.h>
#include <init/tt_config_node_def.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_CFGNODE_CAST(cnode, type)                                           \
    TT_PTR_INC(type, cnode, sizeof(tt_cfgnode_t))
#define TT_CFGNODE_OF(p) TT_PTR_DEC(tt_cfgnode_t, p, sizeof(tt_cfgnode_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_cfgnode_s;
struct tt_buf_s;

typedef enum {
    TT_CFGNODE_TYPE_U32,
    TT_CFGNODE_TYPE_S32,
    TT_CFGNODE_TYPE_GROUP,
    TT_CFGNODE_TYPE_STRING,
    TT_CFGNODE_TYPE_BOOL,

    TT_CFGNODE_TYPE_NUM
} tt_cfgnode_type_t;
#define TT_CFGNODE_TYPE_VALID(t) ((t) < TT_CFGNODE_TYPE_NUM)

typedef struct
{
    const tt_char_t *display_name;
    const tt_char_t *brief;
    const tt_char_t *detail;

    tt_bool_t need_reboot;
} tt_cfgnode_attr_t;

typedef struct tt_cfgnode_s
{
    tt_cfgnode_type_t type;
    void *opaque;
    tt_cfgnode_itf_t *itf;
    tt_lnode_t node;

    const tt_char_t *name;
    const tt_char_t *display_name;
    const tt_char_t *brief;
    const tt_char_t *detail;

    tt_bool_t removing : 1;
    tt_bool_t modified : 1;
    tt_bool_t need_reboot : 1;
} tt_cfgnode_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_cfgnode_t *tt_cfgnode_create(IN tt_u32_t len,
                                       IN tt_cfgnode_type_t type,
                                       IN const tt_char_t *name,
                                       IN OPT tt_cfgnode_itf_t *itf,
                                       IN OPT void *opaque,
                                       IN OPT tt_cfgnode_attr_t *attr);

extern void tt_cfgnode_destroy(IN tt_cfgnode_t *cnode, IN tt_bool_t committed);

extern void tt_cfgnode_attr_default(IN tt_cfgnode_attr_t *attr);

tt_inline const tt_char_t *tt_cfgnode_name(IN tt_cfgnode_t *cnode)
{
    TT_ASSERT(cnode->name != NULL);
    return cnode->name;
}

tt_inline const tt_char_t *tt_cfgnode_display_name(IN tt_cfgnode_t *cnode)
{
    return TT_COND(cnode->display_name != NULL, cnode->display_name, "");
}

tt_inline const tt_char_t *tt_cfgnode_brief(IN tt_cfgnode_t *cnode)
{
    return TT_COND(cnode->brief != NULL, cnode->brief, "");
}

tt_inline const tt_char_t *tt_cfgnode_detail(IN tt_cfgnode_t *cnode)
{
    return TT_COND(cnode->detail != NULL, cnode->detail, "");
}

// - if name_len is less than node name length, then at most name_len of
//   name would be outputted followed by a space
// - if name_len is larger, spaces are filled
extern tt_result_t tt_cfgnode_describe(IN tt_cfgnode_t *cnode,
                                       IN tt_u32_t max_name_len,
                                       OUT struct tt_buf_s *output);

// ========================================
// node operations
// ========================================

// add a non-group child
extern tt_result_t tt_cfgnode_add(IN tt_cfgnode_t *cnode,
                                  IN tt_blob_t *name,
                                  IN tt_blob_t *val);

// remove a non-group child
extern tt_result_t tt_cfgnode_rm(IN tt_cfgnode_t *cnode, IN tt_blob_t *name);

extern tt_result_t tt_cfgnode_ls(IN tt_cfgnode_t *cnode,
                                 IN OPT const tt_char_t *newline,
                                 OUT struct tt_buf_s *output);

extern tt_result_t tt_cfgnode_get(IN tt_cfgnode_t *cnode,
                                  OUT struct tt_buf_s *output);

extern tt_result_t tt_cfgnode_set(IN tt_cfgnode_t *cnode, IN tt_blob_t *val);

extern tt_result_t tt_cfgnode_check(IN tt_cfgnode_t *cnode, IN tt_blob_t *val);

extern tt_result_t tt_cfgnode_status(IN tt_cfgnode_t *cnode,
                                     OUT struct tt_buf_s *output);

extern tt_result_t tt_cfgnode_commit(IN tt_cfgnode_t *cnode);

// restore from modification
extern void tt_cfgnode_restore(IN tt_cfgnode_t *cnode);

#endif /* __TT_CONFIG_NODE__ */
