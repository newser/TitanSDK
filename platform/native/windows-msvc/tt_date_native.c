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

static char *strptime(const char *buf, const char *fmt, struct tm *tm);

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
    return tt_offsec2tmzone(mktime(&gtm) - mktime(&ltm));
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

tt_u32_t tt_date_render_ntv(IN tt_date_t *date,
                            IN const tt_char_t *format,
                            IN tt_char_t *buf,
                            IN tt_u32_t len)
{
    struct tm tm;
    tt_u32_t n;

    if (len == 0) {
        return 0;
    }

    __date2tm(date, &tm);
    n = (tt_u32_t)strftime(buf, len, format, &tm);
    if ((n >= len) || (n == 0)) {
        *buf = 0;
        n = 0;
    }
    return n;
}

tt_u32_t tt_date_parse_ntv(IN tt_date_t *date,
                           IN const tt_char_t *format,
                           IN const tt_char_t *buf)
{
    struct tm tm;
    const tt_char_t *p;

    p = strptime(buf, format, &tm);
    if (p != NULL) {
        __tm2date(&tm, date);
        TT_ASSERT(p >= buf);
        return (tt_u32_t)(p - buf);
    } else {
        TT_ERROR("fail to parse date: %s", buf);
        return 0;
    }
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

    // other members are not used
    tm->tm_wday = 0;
    tm->tm_yday = 0;
}

void __tm2date(IN struct tm *tm, OUT tt_date_t *date)
{
    date->year = (tt_u32_t)tm->tm_year + 1900;
    date->month = (tt_month_t)tm->tm_mon;
    date->mday = (tt_u8_t)tm->tm_mday;
    date->hour = (tt_u8_t)tm->tm_hour;
    date->minute = (tt_u8_t)tm->tm_min;
    date->second = (tt_u8_t)tm->tm_sec;
    // date->dst = TT_BOOL(tm->tm_isdst);
    // time zone is not changed
}

/*	$Id$	*/
/*	$NetBSD: strptime.c,v 1.18 1999/04/29 02:58:30 tv Exp $	*/

/*-
 * Copyright (c) 1997, 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code was contributed to The NetBSD Foundation by Klaus Klein.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

//#include "lukemftp.h"

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E 0x01
#define ALT_O 0x02
#define LEGAL_ALT(x)                                                           \
    {                                                                          \
        if (alt_format & ~(x))                                                 \
            return (0);                                                        \
    }

#define TM_YEAR_BASE 1900

static int conv_num(const char **, int *, int, int);

static const char *day[7] = {"Sunday",
                             "Monday",
                             "Tuesday",
                             "Wednesday",
                             "Thursday",
                             "Friday",
                             "Saturday"};
static const char *abday[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char *mon[12] = {"January",
                              "February",
                              "March",
                              "April",
                              "May",
                              "June",
                              "July",
                              "August",
                              "September",
                              "October",
                              "November",
                              "December"};
static const char *abmon[12] = {"Jan",
                                "Feb",
                                "Mar",
                                "Apr",
                                "May",
                                "Jun",
                                "Jul",
                                "Aug",
                                "Sep",
                                "Oct",
                                "Nov",
                                "Dec"};
static const char *am_pm[2] = {"AM", "PM"};


char *strptime(const char *buf, const char *fmt, struct tm *tm)
{
    char c;
    const char *bp;
    size_t len = 0;
    int alt_format, i, split_year = 0;

    bp = buf;

    while ((c = *fmt) != '\0') {
        /* Clear `alternate' modifier prior to new conversion. */
        alt_format = 0;

        /* Eat up white-space. */
        if (isspace(c)) {
            while (isspace(*bp))
                bp++;

            fmt++;
            continue;
        }

        if ((c = *fmt++) != '%')
            goto literal;


    again:
        switch (c = *fmt++) {
            case '%': /* "%%" is converted to "%". */
            literal:
                if (c != *bp++)
                    return (0);
                break;

            /*
             * "Alternative" modifiers. Just set the appropriate flag
             * and start over again.
             */
            case 'E': /* "%E?" alternative conversion modifier. */
                LEGAL_ALT(0);
                alt_format |= ALT_E;
                goto again;

            case 'O': /* "%O?" alternative conversion modifier. */
                LEGAL_ALT(0);
                alt_format |= ALT_O;
                goto again;

            /*
             * "Complex" conversion rules, implemented through recursion.
             */
            case 'c': /* Date and time, using the locale's format. */
                LEGAL_ALT(ALT_E);
                if (!(bp = strptime(bp, "%x %X", tm)))
                    return (0);
                break;

            case 'D': /* The date as "%m/%d/%y". */
                LEGAL_ALT(0);
                if (!(bp = strptime(bp, "%m/%d/%y", tm)))
                    return (0);
                break;

            case 'R': /* The time as "%H:%M". */
                LEGAL_ALT(0);
                if (!(bp = strptime(bp, "%H:%M", tm)))
                    return (0);
                break;

            case 'r': /* The time in 12-hour clock representation. */
                LEGAL_ALT(0);
                if (!(bp = strptime(bp, "%I:%M:%S %p", tm)))
                    return (0);
                break;

            case 'T': /* The time as "%H:%M:%S". */
                LEGAL_ALT(0);
                if (!(bp = strptime(bp, "%H:%M:%S", tm)))
                    return (0);
                break;

            case 'X': /* The time, using the locale's format. */
                LEGAL_ALT(ALT_E);
                if (!(bp = strptime(bp, "%H:%M:%S", tm)))
                    return (0);
                break;

            case 'x': /* The date, using the locale's format. */
                LEGAL_ALT(ALT_E);
                if (!(bp = strptime(bp, "%m/%d/%y", tm)))
                    return (0);
                break;

            /*
             * "Elementary" conversion rules.
             */
            case 'A': /* The day of week, using the locale's form. */
            case 'a':
                LEGAL_ALT(0);
                for (i = 0; i < 7; i++) {
                    /* Full name. */
                    len = strlen(day[i]);
                    if (_strnicmp(day[i], bp, len) == 0)
                        break;

                    /* Abbreviated name. */
                    len = strlen(abday[i]);
                    if (_strnicmp(abday[i], bp, len) == 0)
                        break;
                }

                /* Nothing matched. */
                if (i == 7)
                    return (0);

                tm->tm_wday = i;
                bp += len;
                break;

            case 'B': /* The month, using the locale's form. */
            case 'b':
            case 'h':
                LEGAL_ALT(0);
                for (i = 0; i < 12; i++) {
                    /* Full name. */
                    len = strlen(mon[i]);
                    if (_strnicmp(mon[i], bp, len) == 0)
                        break;

                    /* Abbreviated name. */
                    len = strlen(abmon[i]);
                    if (_strnicmp(abmon[i], bp, len) == 0)
                        break;
                }

                /* Nothing matched. */
                if (i == 12)
                    return (0);

                tm->tm_mon = i;
                bp += len;
                break;

            case 'C': /* The century number. */
                LEGAL_ALT(ALT_E);
                if (!(conv_num(&bp, &i, 0, 99)))
                    return (0);

                if (split_year) {
                    tm->tm_year = (tm->tm_year % 100) + (i * 100);
                } else {
                    tm->tm_year = i * 100;
                    split_year = 1;
                }
                break;

            case 'd': /* The day of month. */
            case 'e':
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_mday, 1, 31)))
                    return (0);
                break;

            case 'k': /* The hour (24-hour clock representation). */
                LEGAL_ALT(0);
            /* FALLTHROUGH */
            case 'H':
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_hour, 0, 23)))
                    return (0);
                break;

            case 'l': /* The hour (12-hour clock representation). */
                LEGAL_ALT(0);
            /* FALLTHROUGH */
            case 'I':
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_hour, 1, 12)))
                    return (0);
                if (tm->tm_hour == 12)
                    tm->tm_hour = 0;
                break;

            case 'j': /* The day of year. */
                LEGAL_ALT(0);
                if (!(conv_num(&bp, &i, 1, 366)))
                    return (0);
                tm->tm_yday = i - 1;
                break;

            case 'M': /* The minute. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_min, 0, 59)))
                    return (0);
                break;

            case 'm': /* The month. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &i, 1, 12)))
                    return (0);
                tm->tm_mon = i - 1;
                break;

            case 'p': /* The locale's equivalent of AM/PM. */
                LEGAL_ALT(0);
                /* AM? */
                if (_stricmp(am_pm[0], bp) == 0) {
                    if (tm->tm_hour > 11)
                        return (0);

                    bp += strlen(am_pm[0]);
                    break;
                }
                /* PM? */
                else if (_stricmp(am_pm[1], bp) == 0) {
                    if (tm->tm_hour > 11)
                        return (0);

                    tm->tm_hour += 12;
                    bp += strlen(am_pm[1]);
                    break;
                }

                /* Nothing matched. */
                return (0);

            case 'S': /* The seconds. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_sec, 0, 61)))
                    return (0);
                break;

            case 'U': /* The week of year, beginning on sunday. */
            case 'W': /* The week of year, beginning on monday. */
                LEGAL_ALT(ALT_O);
                /*
                 * XXX This is bogus, as we can not assume any valid
                 * information present in the tm structure at this
                 * point to calculate a real value, so just check the
                 * range for now.
                 */
                if (!(conv_num(&bp, &i, 0, 53)))
                    return (0);
                break;

            case 'w': /* The day of week, beginning on sunday. */
                LEGAL_ALT(ALT_O);
                if (!(conv_num(&bp, &tm->tm_wday, 0, 6)))
                    return (0);
                break;

            case 'Y': /* The year. */
                LEGAL_ALT(ALT_E);
                if (!(conv_num(&bp, &i, 0, 9999)))
                    return (0);

                tm->tm_year = i - TM_YEAR_BASE;
                break;

            case 'y': /* The year within 100 years of the epoch. */
                LEGAL_ALT(ALT_E | ALT_O);
                if (!(conv_num(&bp, &i, 0, 99)))
                    return (0);

                if (split_year) {
                    tm->tm_year = ((tm->tm_year / 100) * 100) + i;
                    break;
                }
                split_year = 1;
                if (i <= 68)
                    tm->tm_year = i + 2000 - TM_YEAR_BASE;
                else
                    tm->tm_year = i + 1900 - TM_YEAR_BASE;
                break;

            /*
             * Miscellaneous conversions.
             */
            case 'n': /* Any kind of white-space. */
            case 't':
                LEGAL_ALT(0);
                while (isspace(*bp))
                    bp++;
                break;


            default: /* Unknown/unsupported conversion. */
                return (0);
        }
    }

    /* LINTED functional specification */
    return ((char *)bp);
}


static int conv_num(const char **buf, int *dest, int llim, int ulim)
{
    int result = 0;

    /* The limit also determines the number of valid digits. */
    int rulim = ulim;

    if (**buf < '0' || **buf > '9')
        return (0);

    do {
        result *= 10;
        result += *(*buf)++ - '0';
        rulim /= 10;
    } while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

    if (result < llim || result > ulim)
        return (0);

    *dest = result;
    return (1);
}