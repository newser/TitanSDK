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

#include <log/filter/tt_log_filter.h>

#include <algorithm/tt_buffer.h>
#include <log/io/tt_log_io.h>
#include <memory/tt_memory_alloc.h>

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

tt_logfltr_t *tt_logfltr_create(IN tt_u32_t size, IN tt_logfltr_itf_t *itf)
{
    tt_logfltr_t *lf;

    if ((itf == NULL) || (itf->input == NULL)) {
        return NULL;
    }

    lf = tt_malloc(sizeof(tt_logfltr_t) + size);
    if (lf == NULL) {
        return NULL;
    }

    lf->itf = itf;
    tt_ptrq_init(&lf->io_q, NULL);
    tt_atomic_s32_set(&lf->ref, 1);

    return lf;
}

void __logfltr_destroy(IN tt_logfltr_t *lf)
{
    tt_logio_t *lio;

    if (lf == NULL) {
        return;
    }

    if (lf->itf->destroy != NULL) {
        lf->itf->destroy(lf);
    }

    while ((lio = (tt_logio_t *)tt_ptrq_pop_head(&lf->io_q)) != NULL) {
        tt_logio_release(lio);
    }

    tt_free(lf);
}

tt_result_t tt_logfltr_append_io(IN tt_logfltr_t *lf,
                                 IN TO struct tt_logio_s *lio)
{
    if ((lf == NULL) || (lio == NULL)) {
        return TT_E_BADARG;
    }

    if (TT_OK(tt_ptrq_push_tail(&lf->io_q, lio))) {
        tt_logio_ref(lio);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_u32_t tt_logfltr_input(IN tt_logfltr_t *lf,
                          IN tt_log_entry_t *entry,
                          IN OUT tt_buf_t *buf)
{
    tt_u32_t io = lf->itf->input(lf, entry, buf);

    if (io & TT_LOGFLTR_SELF) {
        tt_ptrq_iter_t i;
        tt_logio_t *lio;

        tt_ptrq_iter(&lf->io_q, &i);
        while ((lio = (tt_logio_t *)tt_ptrq_iter_next(&i)) != NULL) {
            tt_logio_output(lio,
                            entry,
                            (tt_char_t *)TT_BUF_RPOS(buf),
                            TT_BUF_RLEN(buf));
        }
    }

    return io;
}
