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

#include <log/tt_log_io.h>

#include <os/tt_spinlock.h>

#include <tt_cstd_api.h>

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

tt_logio_t *tt_logio_create(IN tt_u32_t size,
                            IN tt_logio_type_t type,
                            IN tt_logio_itf_t *itf)
{
    tt_logio_t *lio;

    lio = (tt_logio_t *)tt_malloc(sizeof(tt_logio_t) + size);
    if (lio == NULL) {
        return NULL;
    }

    lio->type = type;
    lio->lock = NULL;

    tt_memcpy(&lio->itf, itf, sizeof(tt_logio_itf_t));

    return lio;
}

void tt_logio_destroy(IN tt_logio_t *lio)
{
    lio->itf.destroy(lio);

    if (lio->lock != NULL) {
        tt_spinlock_destroy(lio->lock);
        tt_free(lio->lock);
    }

    tt_free(lio);
}

tt_u32_t tt_logio_output(IN tt_logio_t *lio,
                         IN tt_u8_t *data,
                         IN tt_u32_t data_len)
{
    tt_u32_t n;

    if ((data == NULL) || (data_len == 0)) {
        return 0;
    }

    if (lio->lock != NULL) {
        tt_spinlock_acquire(lio->lock);
    }

    n = lio->itf.output(lio, data, data_len);

    if (lio->lock != NULL) {
        tt_spinlock_release(lio->lock);
    }

    return n;
}

tt_result_t tt_logio_enable_lock(IN tt_logio_t *lio)
{
    tt_spinlock_t *lock;

    // this function should be called when platform initialization
    // is finished

    if (lio->lock != NULL) {
        return TT_SUCCESS;
    }

    lock = tt_malloc(sizeof(tt_spinlock_t));
    if (lock == NULL) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_spinlock_create(lock, NULL))) {
        tt_free(lock);
        return TT_FAIL;
    }

    lio->lock = lock;
    return TT_SUCCESS;
}
