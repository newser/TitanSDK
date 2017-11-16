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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_dll_native.h>

#include <misc/tt_assert.h>
#include <os/tt_dll.h>

#include <dlfcn.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_dll_create_ntv(IN tt_dll_ntv_t *dll,
                              IN const tt_char_t *path,
                              IN OPT tt_dll_attr_t *attr)
{
    void *handle;
    int mode = 0;

    TT_ASSERT(dll != NULL);
    TT_ASSERT(path != NULL);

    // may set mode according to attr;
    mode |= RTLD_LAZY;
    mode |= RTLD_GLOBAL;

    handle = dlopen(path, mode);
    if (handle == NULL) {
        TT_ERROR("dlopen failed: %s", dlerror());
        return TT_FAIL;
    }

    dll->handle = handle;
    return TT_SUCCESS;
}

void tt_dll_destroy_ntv(IN tt_dll_ntv_t *dll)
{
    TT_ASSERT(dll != NULL);

    dlclose(dll->handle);
}

void *tt_dll_symbol_ntv(IN tt_dll_ntv_t *dll, IN const tt_char_t *symbol_name)
{
    void *sym;

    TT_ASSERT(dll != NULL);
    TT_ASSERT(symbol_name != NULL);

    sym = dlsym(dll->handle, symbol_name);
    if (sym == NULL) {
        TT_ERROR("dlsym failed: %s", dlerror());
        return NULL;
    }

    return sym;
}
