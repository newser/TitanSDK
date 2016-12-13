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
                              IN tt_u32_t init_num,
                              IN struct tt_sem_attr_s *attr)
{
    pthread_condattr_t cond_attr;

    int ret = pthread_mutex_init(&sys_sem->mutex, NULL);
    if (ret != 0) {
        TT_ERROR("fail to create system mutex: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    ret = pthread_condattr_init(&cond_attr);
    if (ret != 0) {
        pthread_mutex_destroy(&sys_sem->mutex);

        TT_ERROR("fail to init system cond attr: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    ret = pthread_condattr_setclock(&cond_attr, CLOCK_MONOTONIC);
    if (ret != 0) {
        pthread_condattr_destroy(&cond_attr);
        pthread_mutex_destroy(&sys_sem->mutex);

        TT_ERROR("fail to set system cond clock: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    ret = pthread_cond_init(&sys_sem->cond, &cond_attr);
    pthread_condattr_destroy(&cond_attr);
    if (ret != 0) {
        pthread_mutex_destroy(&sys_sem->mutex);

        TT_ERROR("fail to create system cond: %d[%s]", ret, strerror(ret));
        return TT_FAIL;
    }

    sys_sem->avail_num = init_num;

    return TT_SUCCESS;
}

void tt_sem_destroy_ntv(IN tt_sem_ntv_t *sys_sem)
{
    int ret;

    ret = pthread_cond_destroy(&sys_sem->cond);
    if (ret != 0) {
        TT_ERROR("fail to destroy system cond: %d[%s]", ret, strerror(ret));
    }

    ret = pthread_mutex_destroy(&sys_sem->mutex);
    if (ret != 0) {
        TT_ERROR("fail to destroy system mutex: %d[%s]", ret, strerror(ret));
    }
}
