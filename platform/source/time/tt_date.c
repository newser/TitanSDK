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

#include <time/tt_date.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <memory/tt_memory_alloc.h>

#include <tt_cstd_api.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __GC TT_GREGORIAN_BEGIN

#define __Y_IN(o_year) ((o_year)-__GC)
#define __Y_OUT(i_year) ((i_year) + __GC)

#define __MDAY_IN(mday) ((mday)-1)
#define __MDAY_OUT(mday) ((mday) + 1)

#define __daynum_of(year)                                                      \
    TT_COND(__leap_year((year)), __s_daynum_of_leap, __s_daynum_of)

#define __daynum_before(year)                                                  \
    TT_COND(__leap_year((year)), __s_daynum_before_leap, __s_daynum_before)

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

static tt_u32_t __s_daynum_of[TT_MONTH_NUM] =
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static tt_u32_t __s_daynum_before[TT_MONTH_NUM] =
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

static tt_u32_t __s_daynum_of_leap[TT_MONTH_NUM] =
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static tt_u32_t __s_daynum_before_leap[TT_MONTH_NUM] =
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};

static tt_u32_t __s_min_cjdn;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static tt_result_t __date_component_init(IN tt_component_t *comp,
                                         IN tt_profile_t *profile);

static tt_bool_t __date_valid(IN tt_u16_t year,
                              IN tt_month_t month,
                              IN tt_u8_t day);

static tt_bool_t __leap_year(IN tt_u16_t year);

tt_u32_t __ymd2cjdn(IN tt_u16_t year, IN tt_month_t month, IN tt_u8_t day);

void __cjdn2ymd(IN tt_u32_t cjdn,
                OUT tt_u16_t *year,
                OUT tt_month_t *month,
                OUT tt_u8_t *day);

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
    // it's initially 32768 BC, Jan. 1st

    date->tz = tz;
}

tt_bool_t tt_date_valid(IN tt_u32_t year, IN tt_month_t month, IN tt_u32_t mday)
{
    return __date_valid(__Y_IN(year), month, __MDAY_IN(mday));
}

tt_s32_t tt_date_cmp(IN tt_date_t *a, IN tt_date_t *b)
{
#define __dcmp(field)                                                          \
    do {                                                                       \
        if (a->field > b->field) {                                             \
            return 1;                                                          \
        } else if (a->field < b->field) {                                      \
            return -1;                                                         \
        }                                                                      \
    } while (0)

    __dcmp(year);
    __dcmp(month);
    __dcmp(mday);
    __dcmp(hour);
    __dcmp(minute);
    __dcmp(second);

#undef __dcmp

    return 0;
}

tt_result_t tt_date_change_tmzone(IN tt_date_t *date, IN tt_tmzone_t tz)
{
    tt_s32_t org_s, new_s;

    TT_ASSERT(TT_TMZONE_VALID(tz));

    org_s = tt_tmzone2offsec(date->tz);
    new_s = tt_tmzone2offsec(tz);
    if (new_s >= org_s) {
        TT_DO(tt_date_inc_second(date, (tt_u32_t)(new_s - org_s)));
    } else {
        TT_DO(tt_date_dec_second(date, (tt_u32_t)(org_s - new_s)));
    }
    date->tz = tz;
    return TT_SUCCESS;
}

tt_s32_t tt_date_diff_day(IN tt_date_t *a, IN tt_date_t *b)
{
    tt_date_t utc_a, utc_b;
    tt_u32_t a_cjdn, b_cjdn;

    tt_date_copy(&utc_a, a);
    tt_date_change_tmzone(&utc_a, TT_UTC_00_00);
    a_cjdn = __ymd2cjdn(utc_a.year, utc_a.month, utc_a.mday);

    tt_date_copy(&utc_b, b);
    tt_date_change_tmzone(&utc_b, TT_UTC_00_00);
    b_cjdn = __ymd2cjdn(utc_b.year, utc_b.month, utc_b.mday);

    return (tt_s32_t)(a_cjdn - b_cjdn);
}

tt_s64_t tt_date_diff_second(IN tt_date_t *a, IN tt_date_t *b)
{
    tt_date_t utc_a, utc_b;
    tt_u32_t a_cjdn, b_cjdn;
    tt_s64_t sec;

    tt_date_copy(&utc_a, a);
    tt_date_change_tmzone(&utc_a, TT_UTC_00_00);
    a_cjdn = __ymd2cjdn(utc_a.year, utc_a.month, utc_a.mday);

    tt_date_copy(&utc_b, b);
    tt_date_change_tmzone(&utc_b, TT_UTC_00_00);
    b_cjdn = __ymd2cjdn(utc_b.year, utc_b.month, utc_b.mday);

    sec = (tt_s64_t)(a_cjdn - b_cjdn) * 86400;
    sec += (tt_s64_t)(utc_a.hour - utc_b.hour) * 3600;
    sec += (tt_s64_t)(utc_a.minute - utc_b.minute) * 60;
    sec += (tt_s64_t)(utc_a.second - utc_b.second);
    return sec;
}

// ========================================
// get/set
// ========================================

tt_result_t tt_date_set_year(IN tt_date_t *date, IN tt_u16_t year)
{
    tt_u16_t y;

    if (year < __GC) {
        TT_ERROR("Gregorian calendar starts from %d", __GC);
        return TT_FAIL;
    }
    y = __Y_IN(year);

    if (!__date_valid(y, date->month, date->mday)) {
        return TT_FAIL;
    }

    date->year = y;
    return TT_SUCCESS;
}

tt_u32_t tt_date_get_year(IN tt_date_t *date)
{
    return __Y_OUT(date->year);
}

tt_bool_t tt_date_is_leapyear(IN tt_date_t *date)
{
    return __leap_year(date->year);
}

tt_result_t tt_date_set_monthday(IN tt_date_t *date, IN tt_u32_t mday)
{
    tt_u8_t md;

    if ((mday < 1) || (mday > 31)) {
        TT_ERROR("invalid month day: %d", mday);
        return TT_FAIL;
    }
    md = (tt_u8_t)__MDAY_IN(mday);

    if (!__date_valid(date->year, date->month, md)) {
        return TT_FAIL;
    }

    date->mday = md;
    return TT_SUCCESS;
}

tt_u32_t tt_date_get_monthday(IN tt_date_t *date)
{
    return __MDAY_OUT(date->mday);
}

tt_weekday_t tt_date_get_weekday(IN tt_date_t *date)
{
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    tt_u32_t y, m, d;

    y = __Y_OUT(date->year);
    m = date->month;
    d = __MDAY_OUT(date->mday);
    y -= TT_COND(m < 3, 1, 0);
    return (y + y / 4 - y / 100 + y / 400 + t[m] + d) % 7;
}

tt_result_t tt_date_set_yearday(IN tt_date_t *date, IN tt_u32_t yday)
{
    tt_bool_t leap;
    tt_u32_t *dn_of;
    tt_month_t m;

    if (yday > 365) {
        TT_ERROR("invalid year day: %d", yday);
        return TT_FAIL;
    }

    leap = __leap_year(date->year);
    if (!leap && (yday == 365)) {
        TT_ERROR("leap year has only 365 days", yday);
        return TT_FAIL;
    }

    dn_of = __daynum_of(date->year);
    m = TT_JANUARY;
    while (m <= TT_DECEMBER) {
        if (yday < dn_of[m]) {
            date->month = m;
            date->mday = yday;
            return TT_SUCCESS;
        }

        yday -= dn_of[m];
        ++m;
    }
    TT_ASSERT(0);
    return TT_FAIL;
}

tt_u32_t tt_date_get_yearday(IN tt_date_t *date)
{
    return __daynum_before(date->year)[date->month] + date->mday;
}

tt_result_t tt_date_set_date(IN tt_date_t *date,
                             IN tt_u32_t year,
                             IN tt_month_t month,
                             IN tt_u32_t mday)
{
    tt_u32_t y, md;

    if (year < __GC) {
        TT_ERROR("Gregorian calendar starts from %d", __GC);
        return TT_FAIL;
    }
    y = __Y_IN(year);

    TT_ASSERT(TT_MONTH_VALID(month));

    if ((mday < 1) || (mday > 31)) {
        TT_ERROR("invalid month day: %d", mday);
        return TT_FAIL;
    }
    md = __MDAY_IN(mday);

    if (!__date_valid(y, month, md)) {
        return TT_FAIL;
    }

    date->year = y;
    date->month = month;
    date->mday = md;
    return TT_SUCCESS;
}

tt_u32_t tt_date_get_week(IN tt_date_t *date, IN tt_weekday_t first_day)
{
    tt_date_t d;
    tt_weekday_t wd;
    tt_u32_t w, n;

    tt_date_copy(&d, date);
    tt_date_set_month(&d, TT_JANUARY);
    tt_date_set_monthday(&d, 1);
    wd = tt_date_get_weekday(&d);
    if (wd < first_day) {
        n = first_day - wd;
    } else if (wd == first_day) {
        n = 0;
    } else {
        n = TT_WEEKDAY_NUM + first_day - wd;
    }

    w = tt_date_get_yearday(date);
    if (w >= n) {
        w -= n;
        w /= 7;
        if (n != 0) {
            ++w;
        }
        return w;
    } else {
        return 0;
    }
}

// ========================================
// increase/decrease
// ========================================

tt_result_t tt_date_inc_day(IN tt_date_t *date, IN tt_u32_t day)
{
    tt_u32_t cjdn;
    tt_month_t m;

    cjdn = __ymd2cjdn(date->year, date->month, date->mday);
    if ((cjdn + day) < day) {
        TT_ERROR("invalid day to increase");
        return TT_FAIL;
    }

    cjdn += day;
    __cjdn2ymd(cjdn, &date->year, &m, &date->mday);
    date->month = m;

    TT_ASSERT(__date_valid(date->year, date->month, date->mday));
    return TT_SUCCESS;
}

tt_result_t tt_date_dec_day(IN tt_date_t *date, IN tt_u32_t day)
{
    tt_u32_t cjdn;
    tt_month_t m;

    cjdn = __ymd2cjdn(date->year, date->month, date->mday);
    if (((__s_min_cjdn + day) < day) || ((__s_min_cjdn + day) > cjdn)) {
        TT_ERROR("invalid day to decrease");
        return TT_FAIL;
    }

    cjdn -= day;
    __cjdn2ymd(cjdn, &date->year, &m, &date->mday);
    date->month = m;

    TT_ASSERT(__date_valid(date->year, date->month, date->mday));
    return TT_SUCCESS;
}

tt_result_t tt_date_inc_hour(IN tt_date_t *date, IN tt_u32_t hour)
{
    tt_u32_t day;

    day = hour / 24;
    hour %= 24;
    if ((date->hour + hour) >= 24) {
        ++day;
        hour -= 24;
    }

    if (TT_OK(tt_date_inc_day(date, day))) {
        date->hour += hour;
        TT_ASSERT(__date_valid(date->year, date->month, date->mday));
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_date_dec_hour(IN tt_date_t *date, IN tt_u32_t hour)
{
    tt_u32_t day;

    day = hour / 24;
    hour %= 24;
    if (date->hour < hour) {
        ++day;
        hour -= 24;
    }

    if (TT_OK(tt_date_dec_day(date, day))) {
        date->hour -= hour;
        TT_ASSERT(__date_valid(date->year, date->month, date->mday));
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_date_inc_minute(IN tt_date_t *date, IN tt_u32_t minute)
{
    tt_u32_t hour;

    hour = minute / 60;
    minute %= 60;
    if ((date->minute + minute) >= 60) {
        ++hour;
        minute -= 60;
    }

    if (TT_OK(tt_date_inc_hour(date, hour))) {
        date->minute += minute;
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_date_dec_minute(IN tt_date_t *date, IN tt_u32_t minute)
{
    tt_u32_t hour;

    hour = minute / 60;
    minute %= 60;
    if (date->minute < minute) {
        ++hour;
        minute -= 60;
    }

    if (TT_OK(tt_date_dec_hour(date, hour))) {
        date->minute -= minute;
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_date_inc_second(IN tt_date_t *date, IN tt_u32_t second)
{
    tt_u32_t minute;

    minute = second / 60;
    second %= 60;
    if ((date->second + second) >= 60) {
        ++minute;
        second -= 60;
    }

    if (TT_OK(tt_date_inc_minute(date, minute))) {
        date->second += second;
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_date_dec_second(IN tt_date_t *date, IN tt_u32_t second)
{
    tt_u32_t minute;

    minute = second / 60;
    second %= 60;
    if (date->second < second) {
        ++minute;
        second -= 60;
    }

    if (TT_OK(tt_date_dec_minute(date, minute))) {
        date->second -= second;
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_u32_t tt_date_render(IN tt_date_t *date,
                        IN const tt_char_t *format,
                        IN tt_char_t *buf,
                        IN tt_u32_t len)
{
    TT_ASSERT(date != NULL);
    TT_ASSERT(format != NULL);
    TT_ASSERT(buf != NULL);

    return tt_date_render_ntv(date, format, buf, len);
}

tt_u32_t tt_date_parse(IN tt_date_t *date,
                       IN const tt_char_t *format,
                       IN tt_char_t *buf,
                       IN tt_u32_t len)
{
    tt_char_t tmp[128], *s;
    tt_u32_t result;

    TT_ASSERT(date != NULL);
    TT_ASSERT(format != NULL);
    TT_ASSERT(buf != NULL);

    if (len < sizeof(tmp)) {
        s = tmp;
    } else {
        s = tt_malloc(len + 1);
        if (s == NULL) {
            TT_ERROR("no mem to parse date");
            return 0;
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

// ========================================
// conversion
// ========================================

tt_result_t tt_date_to_julian(IN tt_date_t *date, OUT tt_double_t *julian)
{
    tt_date_t d;
    tt_double_t jd;
    tt_u32_t cjdn;

    tt_date_copy(&d, date);
    if (!TT_OK(tt_date_change_tmzone(&d, TT_UTC_00_00))) {
        return -1;
    }

    cjdn = __ymd2cjdn(d.year, d.month, d.mday);

    jd = cjdn;
    jd += d.hour / 24.0 + d.minute / 1440.0 + d.second / 86400.0 - 0.5;
    *julian = jd;
    return TT_SUCCESS;
}

tt_result_t tt_date_from_julian(IN tt_date_t *date,
                                IN tt_double_t julian,
                                IN tt_tmzone_t tz)
{
    tt_double_t cjd;
    tt_u32_t cjdn, sec;
    tt_month_t m;

    cjd = julian + 0.5;
    cjdn = (tt_u32_t)cjd;
    cjd -= cjdn;

    __cjdn2ymd(cjdn, &date->year, &m, &date->mday);
    date->month = m;
    if (!__date_valid(date->year, date->month, date->mday)) {
        return TT_FAIL;
    }

    // time
    TT_ASSERT(cjd < 1);
    sec = (tt_u32_t)(cjd * 86400);

    date->hour = (tt_u8_t)(sec / 3600);
    TT_ASSERT(date->hour < 24);
    sec -= (date->hour * 3600);

    date->minute = (tt_u8_t)(sec / 60);
    TT_ASSERT(date->hour < 60);
    sec -= (date->minute * 60);

    TT_ASSERT(sec < 60);
    date->second = sec;
    ;

    date->tz = TT_UTC_00_00;
    return tt_date_change_tmzone(date, tz);
}

tt_result_t __date_component_init(IN tt_component_t *comp,
                                  IN tt_profile_t *profile)
{
    if (!TT_OK(tt_date_component_init_ntv(profile))) {
        return TT_FAIL;
    }

    tt_g_local_tmzone = tt_local_tmzone_ntv();

    __s_min_cjdn = __ymd2cjdn(__Y_IN(__GC), TT_JANUARY, 1);

    return TT_SUCCESS;
}

tt_bool_t __date_valid(IN tt_u16_t year, IN tt_month_t month, IN tt_u8_t day)
{
    if (!TT_MONTH_VALID(month)) {
        TT_ERROR("invalid month: %d", month);
        return TT_FALSE;
    }

    if (day >= __daynum_of(year)[month]) {
        TT_ERROR("invalid day[%d] of %d-%d has only", day, year, month + 1);
        return TT_FALSE;
    }

    return TT_TRUE;
}

tt_bool_t __leap_year(IN tt_u16_t year)
{
    tt_s32_t y = __Y_OUT(year);
    if (y < 0) {
        ++y;
    }
    return TT_BOOL((y % 4 == 0) && ((y % 100 != 0) || (y % 400 == 0)) &&
                   (y % 3200 != 0));
}

tt_u32_t __ymd2cjdn(IN tt_u16_t year, IN tt_month_t month, IN tt_u8_t day)
{
    tt_s32_t j, c0, x4, x3, x2, x1;

    year = __Y_OUT(year);
    ++month;
    day = __MDAY_OUT(day);

    // c0 = ((tt_s32_t)month - 3) / 12;
    c0 = TT_COND(month < 3, -1, 0);
    x4 = year + c0;
    x3 = x4 / 100;
    x2 = x4 % 100;
    x1 = month - 12 * c0 - 3;
    j = (146097 * x3 / 4) + (36525 * x2 / 100) + ((153 * x1 + 2) / 5) + day +
        1721119;

    TT_ASSERT(j >= 0);
    return (tt_u32_t)j;
}

void __cjdn2ymd(IN tt_u32_t cjdn,
                OUT tt_u16_t *year,
                OUT tt_month_t *month,
                OUT tt_u8_t *day)
{
    tt_u32_t x3, r3, x2, r2, x1, r1, d, c0, j, m;

    m = 4 * cjdn - 6884477;
    x3 = m / 146097;
    r3 = m % 146097;

    m = (r3 / 4) * 100 + 99;
    x2 = m / 36525;
    r2 = m % 36525;

    m = (r2 / 100) * 5 + 2;
    x1 = m / 153;
    r1 = m % 153;

    d = r1 / 5 + 1;
    c0 = (x1 + 2) / 12;
    j = 100 * x3 + x2 + c0;
    m = x1 - 12 * c0 + 3;

    *year = __Y_IN(j);
    TT_ASSERT(month > 0);
    *month = m - 1;
    *day = __MDAY_IN(d);
}
