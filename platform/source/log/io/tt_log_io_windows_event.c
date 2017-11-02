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

#include <log/io/tt_log_io_windows_event.h>

#include <log/io/tt_log_io.h>
#include <memory/tt_memory_alloc.h>

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

static void __lio_winev_destroy(IN tt_logio_t *lio);

static void __lio_winev_output(IN tt_logio_t *lio,
                               IN const tt_char_t *data,
                               IN tt_u32_t data_len);

static tt_logio_itf_t tt_s_logio_winev_itf = {
    TT_LOGIO_WINDOWS_EVENT,

    NULL,
    __lio_winev_destroy,
    __lio_winev_output,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_logio_t *tt_logio_winev_create(IN const tt_char_t *source,
                                  IN tt_winev_type_t type,
                                  IN tt_u32_t category,
                                  IN tt_u32_t ev_id,
                                  IN OPT tt_logio_winev_attr_t *attr)
{
    tt_logio_winev_attr_t __attr;
    tt_logio_t *lio;
    tt_logio_winev_t *lio_winev;

    if (attr == NULL) {
        tt_logio_winev_attr_default(&__attr);
        attr = &__attr;
    }

    lio = tt_logio_create(sizeof(tt_logio_winev_t), &tt_s_logio_winev_itf);
    if (lio == NULL) {
        return NULL;
    }

    lio_winev = TT_LOGIO_CAST(lio, tt_logio_winev_t);

#ifdef TT_HAVE_WINDOWS_EVENT_LOG
    lio_winev->source = RegisterEventSourceA(attr->server, source);
    if (lio_winev->source == NULL) {
        TT_ERROR_NTV("fail to reg ev source: %s", source);
        tt_free(lio);
        return NULL;
    }
#endif

    lio_winev->type = type;
    lio_winev->category = category;
    lio_winev->ev_id = ev_id;

    return lio;
}

void tt_logio_winev_attr_default(IN tt_logio_winev_attr_t *attr)
{
    attr->server = NULL;
}

void __lio_winev_destroy(IN tt_logio_t *lio)
{
    tt_logio_winev_t *lio_winev = TT_LOGIO_CAST(lio, tt_logio_winev_t);

#ifdef TT_HAVE_WINDOWS_EVENT_LOG
    if (!DeregisterEventSource(lio_winev->source)) {
        TT_ERROR_NTV("fail to dereg ev source");
    }
#endif
}

void __lio_winev_output(IN tt_logio_t *lio,
                        IN const tt_char_t *data,
                        IN tt_u32_t data_len)
{
    tt_logio_winev_t *lio_winev = TT_LOGIO_CAST(lio, tt_logio_winev_t);

#ifdef TT_HAVE_WINDOWS_EVENT_LOG
    if (!ReportEventA(lio_winev->source,
                      lio_winev->type,
                      lio_winev->category,
                      lio_winev->ev_id,
                      NULL,
                      1,
                      0,
                      &data,
                      NULL)) {
        TT_ERROR_NTV("fail to report event: %s", data);
    }
#endif
}
