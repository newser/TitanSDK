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
@file tt_log_init.h
@brief log init

this file declare APIs for init log
*/

#ifndef __TT_LOG_INIT__
#define __TT_LOG_INIT__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <log/tt_log_manager.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_logmgr_t tt_g_logmgr;

tt_export tt_bool_t tt_g_logmgr_ok;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_log_component_register()
register ts log component
*/
tt_export void tt_log_component_register();

tt_export void tt_logmgr_component_register();

tt_export void tt_logmgr_config_component_register();

tt_export void tt_logmgr_layout_default(IN tt_logmgr_t *lmgr);

tt_export tt_result_t tt_logmgr_io_default(IN tt_logmgr_t *lmgr);

#endif /* __TT_LOG_INIT__ */
