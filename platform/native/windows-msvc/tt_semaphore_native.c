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

tt_result_t tt_sem_create_ntv(IN tt_sem_ntv_t *sys_sem,
                              IN tt_u32_t count,
                              IN struct tt_sem_attr_s *attr)
{
    HANDLE h_sem =
        CreateSemaphore(NULL, TT_MIN(count, 0x7FFFFFFF), 0x7FFFFFFF, NULL);
    if (h_sem != NULL) {
        sys_sem->h_sem = h_sem;
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to create semaphore");
        return TT_FAIL;
    }
}

void tt_sem_destroy_ntv(IN tt_sem_ntv_t *sys_sem)
{
    if (!CloseHandle((sys_sem)->h_sem)) {
        TT_ERROR_NTV("fail to destroy semaphore");
    }
}
