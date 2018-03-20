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
@file tt_io_network_init_native.h
@brief init io network native

init io network portlayer
*/

#ifndef __TT_IO_NETWORK_INIT_NATIVE__
#define __TT_IO_NETWORK_INIT_NATIVE__

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

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn tt_result_t tt_network_io_component_init_ntv()
init network portlayer

@param [in] init_param reserved

@return
- TT_SUCCESS if collecting succeeds
- TT_FAIL otherwise
*/
tt_inline tt_result_t tt_network_io_component_init_ntv()
{
    return TT_SUCCESS;
}

tt_inline void tt_network_io_component_exit_ntv()
{
}

#endif /* __TT_IO_NETWORK_INIT_NATIVE__ */
