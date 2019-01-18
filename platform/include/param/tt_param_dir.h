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
@file tt_param_dir.h
@brief parameter directory

this file defines parameter directory
*/

#ifndef __TT_PARAM_DIR__
#define __TT_PARAM_DIR__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <param/tt_param.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_ptrhmap_s;
struct tt_ptrhmap_attr_s;

typedef struct tt_param_dir_s
{
    struct tt_ptrhmap_s *tidmap;
    tt_list_t child;
    tt_u32_t child_name_len;
    tt_bool_t tidmap_updated : 1;
} tt_param_dir_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_param_t *tt_param_dir_create(IN const tt_char_t *name,
                                          IN OPT tt_param_attr_t *attr);

tt_export tt_result_t tt_param_dir_add(IN tt_param_dir_t *pd,
                                       IN tt_param_t *child);

tt_export void tt_param_dir_remove(IN tt_param_dir_t *pd, IN tt_param_t *child);

tt_export tt_param_t *tt_param_dir_find(IN tt_param_dir_t *pd,
                                        IN const tt_char_t *name,
                                        IN tt_u32_t name_len);

tt_export tt_param_t *tt_param_dir_find_tid(IN tt_param_dir_t *pd,
                                            IN tt_s32_t tid);

tt_inline tt_param_t *tt_param_dir_head(IN tt_param_dir_t *pd)
{
    tt_lnode_t *ln = tt_list_head(&pd->child);
    return TT_COND(ln != NULL, TT_CONTAINER(ln, tt_param_t, node), NULL);
}

tt_inline tt_param_t *tt_param_dir_next(IN tt_param_t *param)
{
    tt_lnode_t *ln = param->node.next;
    return TT_COND(ln != NULL, TT_CONTAINER(ln, tt_param_t, node), NULL);
}

/*
 - tidmap only includes CURRENT param information, once dir is modified, either
   added or removed children, the tidmap may have invalid entries, i,e, entries
   referencing children which have been freed. so caller should call this
   function to rebuild timap once dir is changed.
 - if dir is modified but tidmap is not rebuilt, ALL APIs including
   @tt_param_dir_find_tid are still safe to use, the dir can be safely cleared
   or destroyed
 */
tt_export tt_result_t
tt_param_dir_build_tidmap(IN tt_param_dir_t *pd,
                          IN tt_u32_t slot_num,
                          IN OPT struct tt_ptrhmap_attr_s *attr);

#endif /* __TT_PARAM_DIR__ */
