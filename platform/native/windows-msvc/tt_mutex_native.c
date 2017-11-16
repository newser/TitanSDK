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

#include <tt_mutex_native.h>

#include <misc/tt_assert.h>
#include <os/tt_mutex.h>

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

tt_result_t tt_mutex_create_ntv(IN tt_mutex_ntv_t *sys_mutex,
                                IN tt_mutex_attr_t *attr)
{
    HANDLE h_mutex = CreateMutex(NULL, FALSE, NULL);
    if (h_mutex != NULL) {
        sys_mutex->h_mutex = h_mutex;
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to create system mutex");
        return TT_FAIL;
    }
}

void tt_mutex_destroy_ntv(IN tt_mutex_ntv_t *sys_mutex)
{
    if (!CloseHandle(sys_mutex->h_mutex)) {
        TT_ERROR_NTV("fail to destroy system mutex");
    }
}
