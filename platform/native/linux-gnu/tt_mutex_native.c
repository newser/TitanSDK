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

    ret = pthread_mutexattr_init(&mutexattr);
    if (ret != 0) {
        TT_ERROR("fail to init mutex attr: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    if (attr->config_recursive && attr->recursive) {
        ret = pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
        if (ret != 0) {
            TT_ERROR("fail to set mutex attr recursive: %d[%s]",
                     ret,
                     strerror(ret));
            pthread_mutexattr_destroy(&mutexattr);
            return TT_FAIL;
        }
    }

    ret = pthread_mutex_init(&sys_mutex->mutex, &mutexattr);
    if (ret != 0) {
        TT_ERROR("fail to create system mutex: %d[%s]", ret, strerror(ret));
        pthread_mutexattr_destroy(&mutexattr);
        return TT_FAIL;
    }

    pthread_mutexattr_destroy(&mutexattr);
    return TT_SUCCESS;
}

void tt_mutex_destroy_ntv(IN tt_mutex_ntv_t *sys_mutex)
{
    int ret = pthread_mutex_destroy(&sys_mutex->mutex);
    if (ret != 0) {
        TT_ERROR("fail to destroy system mutex: %d[%s]", ret, strerror(ret));
    }
}
