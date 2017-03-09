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

#include <tt_spinlock_native_cs.h>

#include <os/tt_spinlock.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __SPIN_LIMIT 4000

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

tt_result_t tt_spinlock_create_ntv_cs(IN tt_spinlock_ntv_cs_t *lock,
                                      IN struct tt_spinlock_attr_s *attr)
{
    if (InitializeCriticalSectionAndSpinCount(&lock->cs, __SPIN_LIMIT)) {
        return TT_SUCCESS;
    } else {
        TT_ERROR_NTV("fail to create system spinlock");
        return TT_FAIL;
    }
}

void tt_spinlock_destroy_ntv_cs(IN tt_spinlock_ntv_cs_t *lock)
{
    DeleteCriticalSection(&lock->cs);
}
