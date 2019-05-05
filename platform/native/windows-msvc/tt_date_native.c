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

#include <tt_date_native.h>

#include <init/tt_profile.h>
#include <misc/tt_util.h>
#include <time/tt_date.h>

#include <tt_cstd_api.h>

#include <time.h>

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

static void __tm2date(IN struct tm *tm, OUT tt_date_t *date);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_date_component_init_ntv(IN tt_profile_t *profile)
{
    return TT_SUCCESS;
}

tt_tmzone_t tt_local_tmzone_ntv()
{
    time_t t;
    struct tm gtm, ltm;

    time(&t);
    gmtime_s(&gtm, &t);
    localtime_s(&ltm, &t);
    return tt_offsec2tmzone((tt_s32_t)(mktime(&ltm) - mktime(&gtm)));
}

void tt_date_now_ntv(OUT tt_date_t *date)
{
    time_t t;
    struct tm tm;

    time(&t);
    localtime_s(&tm, &t);

    __tm2date(&tm, date);
    date->tz = tt_g_local_tmzone;
}

void __tm2date(IN struct tm *tm, OUT tt_date_t *date)
{
    tt_date_set(date, tm->tm_year + 1900, tm->tm_mon, tm->tm_mday, tm->tm_hour,
                tm->tm_min, TT_COND(tm->tm_sec < 60, tm->tm_sec, 59));
}
