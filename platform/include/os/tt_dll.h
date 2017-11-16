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
@file tt_dll.h
@brief dynamic link lib

this file defines dynamic link lib APIs
*/

#ifndef __TT_DLL__
#define __TT_DLL__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_dll_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef tt_dll_ntv_t tt_dll_t;

typedef struct tt_dll_attr_s
{
    tt_u32_t reserved;
} tt_dll_attr_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline tt_result_t tt_dll_create(IN tt_dll_t *dll,
                                    IN const tt_char_t *path,
                                    IN OPT tt_dll_attr_t *attr)
{
    return tt_dll_create_ntv(dll, path, attr);
}

tt_inline void tt_dll_destroy(IN tt_dll_t *dll)
{
    tt_dll_destroy_ntv(dll);
}

tt_inline void tt_dll_attr_default(IN tt_dll_attr_t *attr)
{
    attr->reserved = 0;
}

tt_inline void *tt_dll_symbol(IN tt_dll_t *dll, IN const tt_char_t *symbol_name)
{
    return tt_dll_symbol_ntv(dll, symbol_name);
}

#endif /* __TT_DLL__ */
