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

#include <time/tt_date.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
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

tt_tmzone_t tt_g_local_tmzone;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __date_component_init(IN tt_component_t *comp,
                                         IN tt_profile_t *profile);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_date_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __date_component_init,
    };

    // init component
    tt_component_init(&comp, TT_COMPONENT_DATE, "Date", NULL, &itf);

    // register component
    tt_component_register(&comp);
}

void tt_date_init(IN tt_date_t *date, IN tt_tmzone_t tz)
{
    TT_ASSERT(date != NULL);
    TT_ASSERT(TT_TMZONE_VALID(tz));

    tt_memset(date, 0, sizeof(tt_date_t));
    date->tz = tz;
}

tt_result_t tt_date_render(IN tt_date_t *date,
                           IN const tt_char_t *format,
                           IN tt_char_t *buf,
                           IN tt_u32_t len)
{
    TT_ASSERT(date != NULL);
    TT_ASSERT(format != NULL);
    TT_ASSERT(buf != NULL);

    return tt_date_render_ntv(date, format, buf, len);
}

tt_result_t tt_date_parse(IN tt_date_t *date,
                          IN const tt_char_t *format,
                          IN tt_char_t *buf,
                          IN tt_u32_t len)
{
    tt_char_t tmp[128], *s;
    tt_result_t result;

    TT_ASSERT(date != NULL);
    TT_ASSERT(format != NULL);
    TT_ASSERT(buf != NULL);

    if (len < sizeof(tmp)) {
        s = tmp;
    } else {
        s = tt_malloc(len + 1);
        if (s == NULL) {
            TT_ERROR("no mem to parse date");
            return TT_FAIL;
        }
    }
    tt_memcpy(s, buf, len);
    s[len] = 0;

    result = tt_date_parse_ntv(date, format, s);
    if (s != tmp) {
        tt_free(s);
    }
    return result;
}

tt_result_t __date_component_init(IN tt_component_t *comp,
                                  IN tt_profile_t *profile)
{
    if (!TT_OK(tt_date_component_init_ntv(profile))) {
        return TT_FAIL;
    }

    tt_g_local_tmzone = tt_local_tmzone_ntv();

    return TT_SUCCESS;
}
