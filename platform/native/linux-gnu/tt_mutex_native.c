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

#include <tt_mutex_native.h>

#include <os/tt_mutex.h>

#include <tt_sys_error.h>

#include <pthread.h>

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
    int ret = 0;
    pthread_mutexattr_t mutexattr;

    if (pthread_mutexattr_init(&mutexattr) != 0) {
        TT_ERROR_NTV("fail to init mutex attr");
        return TT_FAIL;
    }

    if (attr->config_recursive && attr->recursive &&
        (pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE) != 0)) {
        TT_ERROR_NTV("fail to set mutex attr recursive");

        pthread_mutexattr_destroy(&mutexattr);
        return TT_FAIL;
    }

    if (pthread_mutex_init(&sys_mutex->mutex, &mutexattr) != 0) {
        TT_ERROR_NTV("fail to create system mutex");

        pthread_mutexattr_destroy(&mutexattr);
        return TT_FAIL;
    }

    pthread_mutexattr_destroy(&mutexattr);
    return TT_SUCCESS;
}

void tt_mutex_destroy_ntv(IN tt_mutex_ntv_t *sys_mutex)
{
    if (pthread_mutex_destroy(&sys_mutex->mutex) != 0) {
        TT_ERROR_NTV("fail to destroy system mutex");
    }
}
