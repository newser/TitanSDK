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
@file tt_dll_native.h
@brief dynamic link library portlayer

this file defines dynamic link library portlayer APIs
*/

#ifndef __TT_DLL_NATIVE__
#define __TT_DLL_NATIVE__

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

struct tt_dll_attr_s;

typedef struct
{
    void *handle;
} tt_dll_ntv_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

extern tt_result_t tt_dll_create_ntv(IN tt_dll_ntv_t *dll,
                                     IN const tt_char_t *path,
                                     IN OPT struct tt_dll_attr_s *attr);

extern void tt_dll_destroy_ntv(IN tt_dll_ntv_t *dll);

extern void *tt_dll_symbol_ntv(IN tt_dll_ntv_t *dll,
                               IN const tt_char_t *symbol_name);

#endif /* __TT_DLL_NATIVE__ */
