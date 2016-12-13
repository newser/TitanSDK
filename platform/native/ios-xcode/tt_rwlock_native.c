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

#include <tt_rwlock_native.h>

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

tt_result_t tt_rwlock_create_ntv(IN tt_rwlock_ntv_t *sys_rwlock,
                                 IN struct tt_rwlock_attr_s *attr)
{
    if (pthread_rwlock_init(&sys_rwlock->rwlock, NULL) != 0) {
        TT_ERROR_NTV("fail to create system rwlock");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_rwlock_destroy_ntv(IN tt_rwlock_ntv_t *sys_rwlock)
{
    if (pthread_rwlock_destroy(&sys_rwlock->rwlock) != 0) {
        TT_ERROR_NTV("fail to destroy system rwlock");
    }
}
