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
@file tt_init_config.h
@brief platform config init

this file defines platform config init
*/

#ifndef __TT_CONFIG_INIT__
#define __TT_CONFIG_INIT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <init/tt_config_directory.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_cfgobj_t *tt_g_config_root;

tt_export tt_cfgobj_t *tt_g_config_platform;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_config_component_register();

tt_inline tt_result_t tt_config_add2root(IN tt_cfgobj_t *co)
{
    return tt_cfgdir_add(TT_CFGOBJ_CAST(tt_g_config_root, tt_cfgdir_t), co);
}

tt_inline tt_result_t tt_config_add2plat(IN tt_cfgobj_t *co)
{
    return tt_cfgdir_add(TT_CFGOBJ_CAST(tt_g_config_platform, tt_cfgdir_t), co);
}

#endif /* __TT_CONFIG_INIT__ */
