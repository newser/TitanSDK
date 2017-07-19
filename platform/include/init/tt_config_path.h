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
@file tt_config_path.h
@brief path of config node

path of config node
*/

#ifndef __TT_CONFIG_PATH__
#define __TT_CONFIG_PATH__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <init/tt_config_object.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_buf_s;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// start from root if path begins with '/', otherwise use current
extern tt_cfgobj_t *tt_cfgpath_p2n(IN tt_cfgobj_t *root,
                                   IN tt_cfgobj_t *current,
                                   IN const tt_char_t *path,
                                   IN tt_u32_t len);

tt_inline tt_cfgobj_t *tt_cfgpath_p2n_cstr(IN tt_cfgobj_t *root,
                                           IN tt_cfgobj_t *current,
                                           IN const tt_char_t *path)
{
    return tt_cfgpath_p2n(root, current, path, tt_strlen(path));
}

// print path from root to current:
//  - if root == current, path would be emtpy
//  - else if root is a real ancestor of current, print a relative path
//  - otherwise print an absolute path of current
extern tt_result_t tt_cfgpath_n2p(IN OPT tt_cfgobj_t *root,
                                  IN tt_cfgobj_t *current,
                                  OUT struct tt_buf_s *path);

// - TT_CFGPCP_NONE, none match, @ref completed includes candidates
// - TT_CFGPCP_PARTIAL, can be partially completed
// - TT_CFGPCP_FULL, fully completed
extern tt_result_t tt_cfgpath_complete(IN tt_cfgobj_t *root,
                                       IN tt_cfgobj_t *current,
                                       IN const tt_char_t *path,
                                       IN tt_u32_t path_len,
                                       OUT tt_u32_t *status,
                                       OUT struct tt_buf_s *output);
#define TT_CFGPCP_NONE 0
#define TT_CFGPCP_PARTIAL 1
#define TT_CFGPCP_FULL 2
#define TT_CFGPCP_FULL_MORE 3

#endif /* __TT_CONFIG_PATH__ */
