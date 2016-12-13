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
@file tt_config_bool.h
@brief config option of bool type

this file defines config option of bool type
*/

#ifndef __TT_CONFIG_BOOL__
#define __TT_CONFIG_BOOL__

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

struct tt_cfgbool_s;

// return true if need reboot
typedef tt_bool_t (*tt_cfgbool_on_set_t)(IN struct tt_cfgnode_s *cnode,
                                         IN tt_bool_t new_val);

typedef struct tt_cfgbool_cb_s
{
    tt_cfgnode_on_destroy_t on_destroy;
    tt_cfgbool_on_set_t on_set;
} tt_cfgbool_cb_t;

typedef struct
{
    tt_cfgnode_attr_t cnode_attr;

    tt_cfgval_mode_t mode;
} tt_cfgbool_attr_t;

typedef struct tt_cfgbool_s
{
    tt_bool_t *val_ptr;
    tt_bool_t new_val;

    tt_cfgbool_cb_t *cb;
} tt_cfgbool_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_cfgnode_t *tt_cfgbool_create(IN const tt_char_t *name,
                                       IN OPT tt_cfgnode_itf_t *itf,
                                       IN OPT void *opaque,
                                       IN tt_bool_t *val_ptr,
                                       IN OPT tt_cfgbool_cb_t *cb,
                                       IN OPT tt_cfgbool_attr_t *attr);

extern void tt_cfgbool_attr_default(IN tt_cfgbool_attr_t *attr);

extern tt_result_t tt_cfgbool_ls(IN tt_cfgnode_t *cnode,
                                 IN const tt_char_t *seperator,
                                 OUT struct tt_buf_s *output);

extern tt_result_t tt_cfgbool_get(IN tt_cfgnode_t *cnode,
                                  OUT struct tt_buf_s *output);

extern tt_result_t tt_cfgbool_set(IN tt_cfgnode_t *cnode, IN tt_blob_t *val);

extern tt_result_t tt_cfgbool_check(IN tt_cfgnode_t *cnode, IN tt_blob_t *val);

extern tt_result_t tt_cfgbool_commit(IN tt_cfgnode_t *cnode);

#endif /* __TT_CONFIG_BOOL__ */
