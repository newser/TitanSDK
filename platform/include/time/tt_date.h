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

/**
@file tt_date.h
@brief date

this file specifies date interfaces
*/

#ifndef __TT_DATE__
#define __TT_DATE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <misc/tt_assert.h>
#include <misc/tt_util.h>
#include <time/tt_date_def.h>

#include <tt_cstd_api.h>
#include <tt_date_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_date_s
{
    tt_u16_t year; // start from 1583
    tt_u8_t mday; // [1, 31]
    tt_u8_t hour; // [0, 23]
    tt_u8_t minute; // [0, 59]
    tt_u8_t second; // [0, 60]
    tt_month_t month : 8;
    tt_tmzone_t tz : 8;
} tt_date_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

tt_export tt_tmzone_t tt_g_local_tmzone;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
 @fn void tt_date_component_register()
 register date system
 */
tt_export void tt_date_component_register();

tt_export void tt_date_init(IN tt_date_t *date, IN tt_tmzone_t tz);

tt_inline void tt_date_copy(IN tt_date_t *dst, IN tt_date_t *src)
{
    tt_memcpy(dst, src, sizeof(tt_date_t));
}

tt_export tt_bool_t tt_date_valid(IN tt_date_t *date);

tt_export tt_s32_t tt_date_cmp(IN tt_date_t *a, IN tt_date_t *b);

tt_export tt_s32_t tt_date_cmp_date(IN tt_date_t *a, IN tt_date_t *b);

tt_export tt_s32_t tt_date_cmp_time(IN tt_date_t *a, IN tt_date_t *b);

tt_export tt_s32_t tt_date_cmp_v(IN tt_date_t *date,
                                 IN tt_u32_t year,
                                 IN tt_month_t month,
                                 IN tt_u32_t mday,
                                 IN tt_u32_t hour,
                                 IN tt_month_t minute,
                                 IN tt_u32_t second);

tt_export tt_s32_t tt_date_cmp_vdate(IN tt_date_t *date,
                                     IN tt_u32_t year,
                                     IN tt_month_t month,
                                     IN tt_u32_t mday);

tt_export tt_s32_t tt_date_cmp_vtime(IN tt_date_t *date,
                                     IN tt_u32_t hour,
                                     IN tt_month_t minute,
                                     IN tt_u32_t second);

tt_export tt_result_t tt_date_change_tmzone(IN tt_date_t *date,
                                            IN tt_tmzone_t tz);

tt_export tt_s32_t tt_date_diff_day(IN tt_date_t *a, IN tt_date_t *b);

tt_export tt_s64_t tt_date_diff_second(IN tt_date_t *a, IN tt_date_t *b);

// ========================================
// get/set
// ========================================

tt_export tt_result_t tt_date_set_year(IN tt_date_t *date, IN tt_u16_t year);

tt_export tt_u32_t tt_date_get_year(IN tt_date_t *date);

tt_export tt_bool_t tt_date_is_leapyear(IN tt_date_t *date);

tt_inline void tt_date_set_month(IN tt_date_t *date, IN tt_month_t month)
{
    TT_ASSERT(TT_MONTH_VALID(month));
    date->month = month;
}

tt_inline tt_month_t tt_date_get_month(IN tt_date_t *date)
{
    return date->month;
}

tt_export tt_result_t tt_date_set_monthday(IN tt_date_t *date,
                                           IN tt_u32_t mday);

tt_export tt_u32_t tt_date_get_monthday(IN tt_date_t *date);

tt_export tt_weekday_t tt_date_get_weekday(IN tt_date_t *date);

tt_export tt_result_t tt_date_set_yearday(IN tt_date_t *date, IN tt_u32_t yday);

tt_export tt_u32_t tt_date_get_yearday(IN tt_date_t *date);

tt_inline void tt_date_set_hour(IN tt_date_t *date, IN tt_u32_t hour)
{
    TT_ASSERT(hour <= 23);
    date->hour = (tt_u8_t)hour;
}

tt_inline tt_u32_t tt_date_get_hour(IN tt_date_t *date)
{
    return date->hour;
}

// 0: 12am, 12: 12pm
tt_inline tt_u32_t tt_date_get_hour_ampm(IN tt_date_t *date)
{
    if (date->hour <= 12) {
        return date->hour;
    } else {
        return date->hour - 12;
    }
}

// false for 12:00
tt_inline tt_bool_t tt_date_is_am(IN tt_date_t *date)
{
    return TT_BOOL(date->hour < 12);
}

tt_inline tt_bool_t tt_date_is_pm(IN tt_date_t *date)
{
    return !tt_date_is_am(date);
}

tt_inline void tt_date_set_minute(IN tt_date_t *date, IN tt_u32_t minute)
{
    TT_ASSERT(minute <= 59);
    date->minute = (tt_u8_t)minute;
}

tt_inline tt_u32_t tt_date_get_minute(IN tt_date_t *date)
{
    return date->minute;
}

tt_inline void tt_date_set_second(IN tt_date_t *date, IN tt_u32_t second)
{
    TT_ASSERT(second <= 59);
    date->second = (tt_u8_t)second;
}

tt_inline tt_u32_t tt_date_get_second(IN tt_date_t *date)
{
    return date->second;
}

tt_export tt_result_t tt_date_set_date(IN tt_date_t *date,
                                       IN tt_u32_t year,
                                       IN tt_month_t month,
                                       IN tt_u32_t mday);

tt_inline void tt_date_set_time(IN tt_date_t *date,
                                IN tt_u32_t hour,
                                IN tt_u32_t minute,
                                IN tt_u32_t second)
{
    tt_date_set_hour(date, hour);
    tt_date_set_minute(date, minute);
    tt_date_set_second(date, second);
}

tt_inline tt_result_t tt_date_set(IN tt_date_t *date,
                                  IN tt_u32_t year,
                                  IN tt_month_t month,
                                  IN tt_u32_t mday,
                                  IN tt_u32_t hour,
                                  IN tt_u32_t minute,
                                  IN tt_u32_t second)
{
    if (TT_OK(tt_date_set_date(date, year, month, mday))) {
        tt_date_set_time(date, hour, minute, second);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_inline tt_tmzone_t tt_date_get_tmzone(IN tt_date_t *date)
{
    return date->tz;
}

tt_inline void tt_date_set_tmzone(IN tt_date_t *date, IN tt_tmzone_t tz)
{
    date->tz = tz;
}

tt_inline void tt_date_now(OUT tt_date_t *date)
{
    tt_date_now_ntv(date);
}

tt_export tt_u32_t tt_date_get_week(IN tt_date_t *date,
                                    IN tt_weekday_t first_day);

// ========================================
// increase/decrease
// ========================================

tt_export tt_result_t tt_date_inc_day(IN tt_date_t *date, IN tt_u32_t day);

tt_export tt_result_t tt_date_dec_day(IN tt_date_t *date, IN tt_u32_t day);

tt_export tt_result_t tt_date_inc_hour(IN tt_date_t *date, IN tt_u32_t hour);

tt_export tt_result_t tt_date_dec_hour(IN tt_date_t *date, IN tt_u32_t hour);

tt_export tt_result_t tt_date_inc_minute(IN tt_date_t *date,
                                         IN tt_u32_t minute);

tt_export tt_result_t tt_date_dec_minute(IN tt_date_t *date,
                                         IN tt_u32_t minute);

tt_export tt_result_t tt_date_inc_second(IN tt_date_t *date,
                                         IN tt_u32_t second);

tt_export tt_result_t tt_date_dec_second(IN tt_date_t *date,
                                         IN tt_u32_t second);

// ========================================
// misc
// ========================================

tt_export tt_result_t tt_date_to_julian(IN tt_date_t *date,
                                        OUT tt_double_t *julian);

tt_export tt_result_t tt_date_from_julian(IN tt_date_t *date,
                                          IN tt_double_t julian,
                                          IN tt_tmzone_t tz);

tt_inline void tt_date_epoch(IN tt_date_t *date)
{
    tt_date_set(date, 1970, TT_JANUARY, 1, 0, 0, 0);
    tt_date_set_tmzone(date, TT_UTC_00_00);
}

tt_inline tt_s32_t tt_date_diff_epoch_day(IN tt_date_t *date)
{
    tt_date_t d;
    tt_date_init(&d, TT_UTC_00_00);
    tt_date_epoch(&d);
    return tt_date_diff_day(date, &d);
}

tt_inline tt_s64_t tt_date_diff_epoch_second(IN tt_date_t *date)
{
    tt_date_t d;
    tt_date_init(&d, TT_UTC_00_00);
    tt_date_epoch(&d);
    return tt_date_diff_second(date, &d);
}

tt_inline tt_s32_t tt_date_diff_now_day(IN tt_date_t *date)
{
    tt_date_t d;
    tt_date_now(&d);
    return tt_date_diff_day(date, &d);
}

tt_inline tt_s64_t tt_date_diff_now_second(IN tt_date_t *date)
{
    tt_date_t d;
    tt_date_now(&d);
    return tt_date_diff_second(date, &d);
}

#endif /* __TT_DATE__ */
