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

static void __date2tm(IN tt_date_t *date, OUT struct tm *tm);

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
    struct tm tm;

    time(&t);
    localtime_r(&t, &tm);
    return tt_offsec2tmzone(tm.tm_gmtoff);
}

void tt_date_now_ntv(OUT tt_date_t *date)
{
    time_t t;
    struct tm tm;

    time(&t);
    localtime_r(&t, &tm);

    __tm2date(&tm, date);
    date->tz = tt_g_local_tmzone;
}

tt_result_t tt_date_render_ntv(IN tt_date_t *date,
                               IN const tt_char_t *format,
                               IN tt_char_t *buf,
                               IN tt_u32_t len)
{
    struct tm tm;
    tt_u32_t n;

    __date2tm(date, &tm);
    n = strftime(buf, len, format, &tm);
    if ((n == 0) || (n >= len)) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_date_parse_ntv(IN tt_date_t *date,
                              IN const tt_char_t *format,
                              IN const tt_char_t *buf)
{
    struct tm tm;

    if (strptime(buf, format, &tm) == NULL) {
        TT_ERROR("fail to parse date: %s", buf);
        return TT_FAIL;
    }

    __tm2date(&tm, date);
    return TT_SUCCESS;
}

void __date2tm(IN tt_date_t *date, OUT struct tm *tm)
{
    tm->tm_year = (int)date->year - 1900;
    tm->tm_mon = (int)date->month;
    tm->tm_mday = (int)date->mday;
    tm->tm_hour = (int)date->hour;
    tm->tm_min = (int)date->minute;
    tm->tm_sec = (int)date->second;
    tm->tm_isdst = (int)date->dst;
}

void __tm2date(IN struct tm *tm, OUT tt_date_t *date)
{
    date->year = (tt_u32_t)tm->tm_year + 1900;
    date->month = (tt_month_t)tm->tm_mon;
    date->mday = (tt_u8_t)tm->tm_mday;
    date->hour = (tt_u8_t)tm->tm_hour;
    date->minute = (tt_u8_t)tm->tm_min;
    date->second = (tt_u8_t)tm->tm_sec;
    date->dst = TT_BOOL(tm->tm_isdst);
}
