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
 @file tt_config_s32.h
 @brief config option of s32 type

 this file defines config option of s32 type
 */

#ifndef __TT_CONFIG_S32__
#define __TT_CONFIG_S32__

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

struct tt_cfgs32_s;

typedef tt_bool_t (*tt_cfgs32_on_set_t)(IN struct tt_cfgnode_s *cs32,
                                        IN tt_s32_t new_val);

typedef struct tt_cfgs32_cb_s
{
    tt_cfgnode_on_destroy_t on_destroy;
    tt_cfgs32_on_set_t on_set;
} tt_cfgs32_cb_t;

typedef struct
{
    tt_cfgnode_attr_t cnode_attr;

    tt_s32_t mode;
#define TT_CFGS32_MODE_G 0
#define TT_CFGS32_MODE_GS 1
} tt_cfgs32_attr_t;

typedef struct tt_cfgs32_s
{
    tt_s32_t *val_ptr;
    tt_s32_t new_val;

    tt_cfgs32_cb_t *cb;
} tt_cfgs32_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_cfgnode_t *tt_cfgs32_create(IN const tt_char_t *name,
                                      IN OPT tt_cfgnode_itf_t *itf,
                                      IN OPT void *opaque,
                                      IN tt_s32_t *val_ptr,
                                      IN OPT tt_cfgs32_cb_t *cb,
                                      IN OPT tt_cfgs32_attr_t *attr);

extern void tt_cfgs32_attr_default(IN tt_cfgs32_attr_t *attr);

extern tt_result_t tt_cfgs32_ls(IN tt_cfgnode_t *cnode,
                                IN const tt_char_t *seperator,
                                OUT struct tt_buf_s *output);

extern tt_result_t tt_cfgs32_get(IN tt_cfgnode_t *cnode,
                                 OUT struct tt_buf_s *output);

extern tt_result_t tt_cfgs32_set(IN tt_cfgnode_t *cnode, IN tt_blob_t *val);

extern tt_result_t tt_cfgs32_check(IN tt_cfgnode_t *cnode, IN tt_blob_t *val);

extern tt_result_t tt_cfgs32_commit(IN tt_cfgnode_t *cnode);

#endif /* __TT_CONFIG_S32__ */
