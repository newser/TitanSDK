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
#include <time/tt_date_def.h>

#include <tt_date_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_date_s
{
    tt_u16_t year;
    tt_u8_t mday; // [1, 31]
    tt_u8_t hour; // [0, 23]
    tt_u8_t minute; // [0, 59]
    tt_u8_t second; // [0, 60]
    tt_month_t month : 8;
    tt_tmzone_t tz : 8;
    tt_bool_t dst : 1;
} tt_date_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

extern tt_tmzone_t tt_g_local_tmzone;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
 @fn void tt_date_component_register()
 register date system
 */
extern void tt_date_component_register();

extern void tt_date_init(IN tt_date_t *date, IN tt_tmzone_t tz);

tt_inline void tt_date_set_year(IN tt_date_t *date, IN tt_u32_t year)
{
    date->year = (tt_u16_t)year;
}

tt_inline tt_u32_t tt_date_get_year(IN tt_date_t *date)
{
    return date->year;
}

tt_inline void tt_date_set_month(IN tt_date_t *date, IN tt_month_t month)
{
    TT_ASSERT(TT_MONTH_VALID(month));
    date->month = month;
}

tt_inline tt_month_t tt_date_get_month(IN tt_date_t *date)
{
    return date->month;
}

tt_inline void tt_date_set_mday(IN tt_date_t *date, IN tt_u32_t mday)
{
    // [1, 31]
    TT_ASSERT((mday >= 1) && (mday <= 31));
    date->mday = (tt_u8_t)mday;
}

tt_inline tt_u32_t tt_date_get_mday(IN tt_date_t *date)
{
    return date->mday;
}

tt_inline void tt_date_set_hour(IN tt_date_t *date, IN tt_u32_t hour)
{
    TT_ASSERT(hour <= 23);
    date->hour = (tt_u8_t)hour;
}

tt_inline tt_u32_t tt_date_get_hour(IN tt_date_t *date)
{
    return date->hour;
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

tt_inline tt_tmzone_t tt_date_get_tmzone(IN tt_date_t *date)
{
    return date->tz;
}

tt_inline void tt_date_set_dst(IN tt_date_t *date, IN tt_bool_t dst)
{
    date->dst = dst;
}

tt_inline tt_bool_t tt_date_get_dst(IN tt_date_t *date)
{
    return date->dst;
}

tt_inline void tt_date_now(OUT tt_date_t *date)
{
    tt_date_now_ntv(date);
}

extern tt_u32_t tt_date_render(IN tt_date_t *date,
                               IN const tt_char_t *format,
                               IN tt_char_t *buf,
                               IN tt_u32_t len);

tt_inline tt_u32_t tt_date_render_now(IN const tt_char_t *format,
                                      IN tt_char_t *buf,
                                      IN tt_u32_t len)
{
    tt_date_t d;
    tt_date_now(&d);
    return tt_date_render_ntv(&d, format, buf, len);
}

extern tt_u32_t tt_date_parse(IN tt_date_t *date,
                              IN const tt_char_t *format,
                              IN tt_char_t *buf,
                              IN tt_u32_t len);

#endif /* __TT_DATE__ */
