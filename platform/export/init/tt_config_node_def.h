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
@brief config node definition

this file includes config node definition
*/

#ifndef __TT_CONFIG_NODE_DEF__
#define __TT_CONFIG_NODE_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_basic_type.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_cfgnode_s;
struct tt_buf_s;

typedef enum {
    TT_CFGGRP_MODE_L,
    TT_CFGGRP_MODE_ARL,

    TT_CFGGRP_MODE_NUM
} tt_cfggrp_mode_t;
#define TT_CFGGRP_MODE_VALID(m) ((m) < TT_CFGGRP_MODE_NUM)

typedef enum {
    TT_CFGVAL_MODE_G,
    TT_CFGVAL_MODE_GS,

    TT_CFGVAL_MODE_NUM
} tt_cfgval_mode_t;
#define TT_CFGVAL_MODE_VALID(m) ((m) < TT_CFGVAL_MODE_NUM)

// interface
typedef void (*tt_cfgnode_on_destroy_t)(IN struct tt_cfgnode_s *cnode,
                                        IN tt_bool_t committed);

typedef tt_result_t (*tt_cfgnode_add_child_t)(IN struct tt_cfgnode_s *cnode,
                                              IN tt_blob_t *name,
                                              IN tt_blob_t *val);

typedef tt_result_t (*tt_cfgnode_rm_child_t)(IN struct tt_cfgnode_s *cnode,
                                             IN tt_blob_t *name);

typedef tt_result_t (*tt_cfgnode_ls_t)(IN struct tt_cfgnode_s *cnode,
                                       IN OPT const tt_char_t *seperator,
                                       OUT struct tt_buf_s *output);

typedef tt_result_t (*tt_cfgnode_describe_t)(IN struct tt_cfgnode_s *cnode,
                                             IN tt_u32_t name_len,
                                             OUT struct tt_buf_s *output);

typedef tt_result_t (*tt_cfgnode_get_t)(IN struct tt_cfgnode_s *cnode,
                                        OUT struct tt_buf_s *output);

typedef tt_result_t (*tt_cfgnode_set_t)(IN struct tt_cfgnode_s *cnode,
                                        IN tt_blob_t *val);

typedef tt_result_t (*tt_cfgnode_check_t)(IN struct tt_cfgnode_s *cnode,
                                          IN tt_blob_t *val);

// return TT_END when need reboot
typedef tt_result_t (*tt_cfgnode_commit_t)(IN struct tt_cfgnode_s *cnode);

typedef struct tt_cfgnode_itf_s
{
    tt_cfgnode_on_destroy_t on_destroy;

    tt_cfgnode_add_child_t add_child;
    tt_cfgnode_rm_child_t rm_child;
    tt_cfgnode_ls_t ls;
    tt_cfgnode_get_t get;
    tt_cfgnode_set_t set;
    tt_cfgnode_check_t check;
    tt_cfgnode_commit_t commit;
} tt_cfgnode_itf_t;

typedef struct tt_cfgnode_perm_s
{
    tt_bool_t can_add : 1;
    tt_bool_t can_rm : 1;
    tt_bool_t can_ls : 1;
    tt_bool_t can_get : 1;
    tt_bool_t can_set : 1;
    tt_bool_t can_check : 1;
} tt_cfgnode_perm_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_CONFIG_NODE_DEF__ */
