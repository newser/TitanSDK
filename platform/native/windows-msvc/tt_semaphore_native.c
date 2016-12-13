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

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt_semaphore_native.h>

#include <misc/tt_assert.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __INFINITE_SEM_NUM (0x7FFFFFFF)

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

tt_result_t tt_sem_create_ntv(IN tt_sem_ntv_t *sys_sem,
                              IN tt_u32_t init_count,
                              IN struct tt_sem_attr_s *attr)
{
    HANDLE win_sem;

    tt_memset(sys_sem, 0, sizeof(tt_sem_ntv_t));

    win_sem = CreateSemaphore(NULL, init_count, __INFINITE_SEM_NUM, NULL);
    if (win_sem != NULL) {
        sys_sem->sem_handle = win_sem;
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to create semaphore");
        return TT_FAIL;
    }
}

void tt_sem_destroy_ntv(IN tt_sem_ntv_t *sys_sem)
{
    TT_ASSERT(sys_sem != NULL);
    TT_ASSERT(sys_sem->sem_handle != NULL);

    if (!CloseHandle((sys_sem)->sem_handle)) {
        TT_ERROR_NTV("fail to destroy semaphore");
    }
}
