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

#include <time/tt_date_format.h>

#include <algorithm/tt_string_common.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef tt_result_t (*__sym_render_t)(IN tt_date_t *d, OUT tt_string_t *s);

typedef const tt_char_t *(*__sym_parse_t)(IN const tt_char_t *start,
                                          IN const tt_char_t *end,
                                          OUT tt_date_t *d);

typedef struct
{
    tt_char_t c;
    __sym_render_t render;
    __sym_parse_t parse;
} __sym_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static tt_result_t __Y_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __y_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __C_render(IN tt_date_t *d, OUT tt_string_t *s);

static const tt_char_t *__Y_parse(IN const tt_char_t *start,
                                  IN const tt_char_t *end,
                                  OUT tt_date_t *d);

static tt_result_t __B_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __b_render(IN tt_date_t *d, OUT tt_string_t *s);

static const tt_char_t *__B_parse(IN const tt_char_t *start,
                                  IN const tt_char_t *end,
                                  OUT tt_date_t *d);

static tt_result_t __N_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __n_render(IN tt_date_t *d, OUT tt_string_t *s);

static const tt_char_t *__N_parse(IN const tt_char_t *start,
                                  IN const tt_char_t *end,
                                  OUT tt_date_t *d);

static tt_result_t __D_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __d_render(IN tt_date_t *d, OUT tt_string_t *s);

static const tt_char_t *__D_parse(IN const tt_char_t *start,
                                  IN const tt_char_t *end,
                                  OUT tt_date_t *d);

static tt_result_t __H_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __h_render(IN tt_date_t *d, OUT tt_string_t *s);

static const tt_char_t *__H_parse(IN const tt_char_t *start,
                                  IN const tt_char_t *end,
                                  OUT tt_date_t *d);

static tt_result_t __M_render(IN tt_date_t *d, OUT tt_string_t *s);

static const tt_char_t *__M_parse(IN const tt_char_t *start,
                                  IN const tt_char_t *end,
                                  OUT tt_date_t *d);

static tt_result_t __S_render(IN tt_date_t *d, OUT tt_string_t *s);

static const tt_char_t *__S_parse(IN const tt_char_t *start,
                                  IN const tt_char_t *end,
                                  OUT tt_date_t *d);

static tt_result_t __W_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __w_render(IN tt_date_t *d, OUT tt_string_t *s);

static const tt_char_t *__W_parse(IN const tt_char_t *start,
                                  IN const tt_char_t *end,
                                  OUT tt_date_t *d);

static tt_result_t __A_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __a_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __Z_render(IN tt_date_t *d, OUT tt_string_t *s);

static tt_result_t __z_render(IN tt_date_t *d, OUT tt_string_t *s);

static const tt_char_t *__Z_parse(IN const tt_char_t *start,
                                  IN const tt_char_t *end,
                                  OUT tt_date_t *d);

static __sym_t __sym[] = {
    {
        'Y', __Y_render, __Y_parse,
    },
    {
        'y', __y_render, __Y_parse,
    },
    {
        'C', __C_render, __Y_parse,
    },
    {
        'B', __B_render, __B_parse,
    },
    {
        'b', __b_render, __B_parse,
    },
    {
        'N', __N_render, __N_parse,
    },
    {
        'n', __n_render, __N_parse,
    },
    {
        'D', __D_render, __D_parse,
    },
    {
        'd', __d_render, __D_parse,
    },
    {
        'H', __H_render, __H_parse,
    },
    {
        'h', __h_render, __H_parse,
    },
    {
        'M', __M_render, __M_parse,
    },
    {
        'S', __S_render, __S_parse,
    },
    {
        'W', __W_render, __W_parse,
    },
    {
        'w', __w_render, __W_parse,
    },
    {
        'A', __A_render,
    },
    {
        'a', __a_render,
    },
    {
        'Z', __Z_render, __Z_parse,
    },
    {
        'z', __z_render, __Z_parse,
    },
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static __sym_t *__find_sym(IN tt_char_t c);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_u32_t tt_date_render(IN tt_date_t *date,
                        IN const tt_char_t *format,
                        IN tt_char_t *buf,
                        IN tt_u32_t len)
{
    tt_string_t s;
    const tt_char_t *p, *end, *prev;
    tt_u32_t n;

    TT_ASSERT(date != NULL);
    TT_ASSERT(format != NULL);
    TT_ASSERT(buf != NULL);

    if (len == 0) {
        return 0;
    }

    tt_string_init(&s, NULL);
    p = format;
    end = p + tt_strlen(p);
    n = 0;

    prev = p;
    while (p < end) {
        if (*p != '%') {
            p += 1;
        } else if ((p + 2) > end) {
            TT_ASSERT((p + 1) == end);
            p += 1;
            break;
        } else if (*(p + 1) == '%') {
            p += 2;
        } else {
            __sym_t *sym;
            const tt_char_t *str;

            TT_DO_G(done,
                    tt_string_append_sub(&s, prev, 0, (tt_u32_t)(p - prev)));

            sym = __find_sym(*(p + 1));
            if (sym == NULL) {
                TT_ERROR("invalid symbol: %c", *(p + 1));
                goto done;
            }
            p += 2;
            prev = p;

            TT_DO_G(done, sym->render(date, &s));
        }
    }
    if (prev < p) {
        TT_DO_G(done, tt_string_append_sub(&s, prev, 0, (tt_u32_t)(p - prev)));
    }

    n = tt_string_len(&s);
    if (n >= len) {
        n = len - 1;
    }
    tt_memcpy(buf, tt_string_cstr(&s), n);
    buf[n] = 0;

done:
    tt_string_destroy(&s);
    return n;
}

tt_result_t tt_date_parse(IN tt_date_t *date,
                          IN const tt_char_t *format,
                          IN const tt_char_t *buf,
                          IN OUT tt_u32_t *len)
{
    tt_date_t d;
    const tt_char_t *fp, *fend, *bp, *bend;

    TT_ASSERT(date != NULL);
    TT_ASSERT(format != NULL);
    TT_ASSERT(buf != NULL);

    if (len == 0) {
        return TT_SUCCESS;
    }

    tt_date_init(&d, tt_g_local_tmzone);

    fp = format;
    fend = format + (tt_u32_t)tt_strlen(format);
    bp = buf;
    bend = bp + *len;
    do {
        while ((fp < fend) && tt_isspace(*fp)) {
            ++fp;
        }
        if (fp >= fend) {
            break;
        }
        while ((bp < bend) && tt_isspace(*bp)) {
            ++bp;
        }
        if (bp >= bend) {
            TT_ERROR("%c has no matching part", *fp);
            return TT_FAIL;
        }

        if (*fp == '%') {
            if ((fp + 1) < fend) {
                if (*(fp + 1) == '%') {
                    if (((bp + 1) < bend) && (*bp == '%') &&
                        (*(bp + 1) == '%')) {
                        fp += 2;
                        bp += 2;
                    } else {
                        TT_ERROR("%%%% has no matching part");
                        return TT_FAIL;
                    }
                } else {
                    __sym_t *sym = __find_sym(*(fp + 1));
                    if (sym == NULL) {
                        TT_ERROR("invalid symbol: %%%c", *(fp + 1));
                        return TT_FAIL;
                    }

                    bp = sym->parse(bp, bend, &d);
                    if (bp == NULL) {
                        return TT_FAIL;
                    }

                    fp += 2;
                }
            } else if (*bp == '%') {
                ++bp;
                break;
            } else {
                TT_ERROR("%% != %c", *bp);
                return TT_FAIL;
            }
        } else if (*fp == *bp) {
            ++fp;
            ++bp;
        } else {
            TT_ERROR("%c != %c", *fp, *bp);
            return TT_FAIL;
        }
    } while ((fp < fend) && (bp < bend));

    if (!tt_date_valid(&d)) {
        TT_ERROR("invalid date parsed");
        return TT_FAIL;
    }

    tt_date_copy(date, &d);
    *len = (tt_u32_t)(bp - buf);
    return TT_SUCCESS;
}

__sym_t *__find_sym(IN tt_char_t c)
{
    tt_u32_t i;
    for (i = 0; i < sizeof(__sym) / sizeof(__sym[0]); ++i) {
        __sym_t *sym = &__sym[i];
        if (sym->c == c) {
            return sym;
        }
    }
    return NULL;
}

tt_result_t __Y_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t y = tt_date_get_year(d) % 100;
    tt_char_t c[3] = {0};

    c[0] = y / 10 + '0';
    c[1] = y % 10 + '0';
    return tt_string_append(s, c);
}

tt_result_t __y_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t y = tt_date_get_year(d) % 100, y1;
    tt_char_t c[3] = {0};

    y1 = y / 10;
    if (y1 != 0) {
        c[0] = y1 + '0';
        c[1] = y % 10 + '0';
    } else {
        c[0] = y % 10 + '0';
    }
    return tt_string_append(s, c);
}

tt_result_t __C_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t y = tt_date_get_year(d);
    tt_char_t c[10] = {0};

    snprintf(c, sizeof(c) - 1, "%d", y);
    return tt_string_append(s, c);
}

const tt_char_t *__Y_parse(IN const tt_char_t *start,
                           IN const tt_char_t *end,
                           OUT tt_date_t *d)
{
    tt_u32_t y = 0;

    if ((start + 4) < end) {
        end = start + 4;
    }

    while ((start < end) && tt_isdigit(*start)) {
        y *= 10;
        y += (*start - '0');
        ++start;
    }
    TT_ASSERT(y <= 9999);

    TT_DO_R(NULL, tt_date_set_year(d, y));
    return start;
}

tt_result_t __B_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    return tt_string_append(s,
                            (tt_char_t *)tt_g_month_name[tt_date_get_month(d)]
                                .addr);
}

tt_result_t __b_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    return tt_string_append(s,
                            (tt_char_t *)
                                tt_g_month_name_abbr[tt_date_get_month(d)]
                                    .addr);
}

const tt_char_t *__B_parse(IN const tt_char_t *start,
                           IN const tt_char_t *end,
                           OUT tt_date_t *d)
{
    tt_u32_t len = (tt_u32_t)(end - start), i;

    for (i = 0; i < sizeof(tt_g_month_name) / sizeof(tt_g_month_name[0]); ++i) {
        tt_blob_t *name = &tt_g_month_name[i];
        if ((len >= name->len) &&
            (tt_strnicmp(start, (tt_char_t *)name->addr, name->len) == 0)) {
            tt_date_set_month(d, i);
            return start + name->len;
        }
    }

    for (i = 0;
         i < sizeof(tt_g_month_name_abbr) / sizeof(tt_g_month_name_abbr[0]);
         ++i) {
        tt_blob_t *name = &tt_g_month_name_abbr[i];
        if ((len >= name->len) &&
            (tt_strnicmp(start, (tt_char_t *)name->addr, name->len) == 0)) {
            tt_date_set_month(d, i);
            return start + name->len;
        }
    }

    TT_ERROR("not found month");
    return NULL;
}

tt_result_t __N_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t m = tt_date_get_month(d) + 1;
    tt_char_t c[3] = {0};

    c[0] = m / 10 + '0';
    c[1] = m % 10 + '0';
    return tt_string_append(s, c);
}

tt_result_t __n_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t m = tt_date_get_month(d) + 1, m1;
    tt_char_t c[3] = {0};

    m1 = m / 10;
    if (m1 != 0) {
        c[0] = m1 + '0';
        c[1] = m % 10 + '0';
    } else {
        c[0] = ' ';
        c[1] = m % 10 + '0';
    }
    return tt_string_append(s, c);
}

const tt_char_t *__N_parse(IN const tt_char_t *start,
                           IN const tt_char_t *end,
                           OUT tt_date_t *d)
{
    tt_u32_t m = 0;

    if ((start + 2) < end) {
        end = start + 2;
    }

    while ((start < end) && tt_isdigit(*start)) {
        m *= 10;
        m += (*start - '0');
        ++start;
    }

    if (m <= 12) {
        tt_date_set_month(d, m - 1);
        return start;
    } else {
        TT_ERROR("invalid month: %d", m);
        return NULL;
    }
}

tt_result_t __D_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t day = tt_date_get_monthday(d);
    tt_char_t c[3] = {0};

    c[0] = day / 10 + '0';
    c[1] = day % 10 + '0';
    return tt_string_append(s, c);
}

tt_result_t __d_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t day = tt_date_get_monthday(d), d1;
    tt_char_t c[3] = {0};

    d1 = day / 10;
    if (d1 != 0) {
        c[0] = d1 + '0';
        c[1] = day % 10 + '0';
    } else {
        c[0] = ' ';
        c[1] = day % 10 + '0';
    }
    return tt_string_append(s, c);
}

const tt_char_t *__D_parse(IN const tt_char_t *start,
                           IN const tt_char_t *end,
                           OUT tt_date_t *d)
{
    tt_u32_t day = 0;

    if ((start + 2) < end) {
        end = start + 2;
    }

    while ((start < end) && tt_isdigit(*start)) {
        day *= 10;
        day += (*start - '0');
        ++start;
    }

    TT_DO_R(NULL, tt_date_set_monthday(d, day));
    return start;
}

tt_result_t __H_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t h = tt_date_get_hour(d);
    tt_char_t c[3] = {0};

    c[0] = h / 10 + '0';
    c[1] = h % 10 + '0';
    return tt_string_append(s, c);
}

tt_result_t __h_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t h = tt_date_get_hour(d) % 12;
    tt_char_t c[3] = {0};

    c[0] = h / 10 + '0';
    c[1] = h % 10 + '0';
    return tt_string_append(s, c);
}

const tt_char_t *__H_parse(IN const tt_char_t *start,
                           IN const tt_char_t *end,
                           OUT tt_date_t *d)
{
    tt_u32_t h = 0;

    if ((start + 2) < end) {
        end = start + 2;
    }

    while ((start < end) && tt_isdigit(*start)) {
        h *= 10;
        h += (*start - '0');
        ++start;
    }

    if (h <= 23) {
        tt_date_set_hour(d, h);
        return start;
    } else {
        TT_ERROR("invalid hour: %d", h);
        return NULL;
    }
}

tt_result_t __M_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t m = tt_date_get_minute(d);
    tt_char_t c[3] = {0};

    c[0] = m / 10 + '0';
    c[1] = m % 10 + '0';
    return tt_string_append(s, c);
}

const tt_char_t *__M_parse(IN const tt_char_t *start,
                           IN const tt_char_t *end,
                           OUT tt_date_t *d)
{
    tt_u32_t m = 0;

    if ((start + 2) < end) {
        end = start + 2;
    }

    while ((start < end) && tt_isdigit(*start)) {
        m *= 10;
        m += (*start - '0');
        ++start;
    }

    if (m <= 59) {
        tt_date_set_minute(d, m);
        return start;
    } else {
        TT_ERROR("invalid minute: %d", m);
        return NULL;
    }
}

tt_result_t __S_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t sec = tt_date_get_second(d);
    tt_char_t c[3] = {0};

    c[0] = sec / 10 + '0';
    c[1] = sec % 10 + '0';
    return tt_string_append(s, c);
}

const tt_char_t *__S_parse(IN const tt_char_t *start,
                           IN const tt_char_t *end,
                           OUT tt_date_t *d)
{
    tt_u32_t s = 0;

    if ((start + 2) < end) {
        end = start + 2;
    }

    while ((start < end) && tt_isdigit(*start)) {
        s *= 10;
        s += (*start - '0');
        ++start;
    }

    if (s <= 59) {
        tt_date_set_second(d, s);
        return start;
    } else {
        TT_ERROR("invalid second: %d", s);
        return NULL;
    }
}

tt_result_t __W_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    return tt_string_append(s,
                            (tt_char_t *)
                                tt_g_weekday_name[tt_date_get_weekday(d)]
                                    .addr);
}

tt_result_t __w_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    return tt_string_append(s,
                            (tt_char_t *)
                                tt_g_weekday_name_abbr[tt_date_get_weekday(d)]
                                    .addr);
}

const tt_char_t *__W_parse(IN const tt_char_t *start,
                           IN const tt_char_t *end,
                           OUT tt_date_t *d)
{
    tt_u32_t len = (tt_u32_t)(end - start), i;

    for (i = 0; i < sizeof(tt_g_weekday_name) / sizeof(tt_g_weekday_name[0]);
         ++i) {
        tt_blob_t *name = &tt_g_weekday_name[i];
        if ((len >= name->len) &&
            (tt_strnicmp(start, (tt_char_t *)name->addr, name->len) == 0)) {
            return start + name->len;
        }
    }

    for (i = 0;
         i < sizeof(tt_g_weekday_name_abbr) / sizeof(tt_g_weekday_name_abbr[0]);
         ++i) {
        tt_blob_t *name = &tt_g_weekday_name_abbr[i];
        if ((len >= name->len) &&
            (tt_strnicmp(start, (tt_char_t *)name->addr, name->len) == 0)) {
            return start + name->len;
        }
    }

    TT_ERROR("not found weekday");
    return NULL;
}

tt_result_t __A_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t h = tt_date_get_hour(d);
    return tt_string_append(s, TT_COND(h < 12, "AM", "PM"));
}

tt_result_t __a_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    tt_u32_t h = tt_date_get_hour(d);
    return tt_string_append(s, TT_COND(h < 12, "am", "pm"));
}

tt_result_t __Z_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    return tt_string_append(s,
                            (tt_char_t *)
                                tt_g_tmzone_name_rfc1123[tt_date_get_tmzone(d)]
                                    .addr);
}

tt_result_t __z_render(IN tt_date_t *d, OUT tt_string_t *s)
{
    return tt_string_append(s,
                            (tt_char_t *)
                                tt_g_tmzone_name_iso8601[tt_date_get_tmzone(d)]
                                    .addr);
}

const tt_char_t *__Z_parse(IN const tt_char_t *start,
                           IN const tt_char_t *end,
                           OUT tt_date_t *d)
{
    tt_u32_t len = (tt_u32_t)(end - start), i;

    for (i = 0; i < sizeof(tt_g_tmzone_name_iso8601) /
                        sizeof(tt_g_tmzone_name_iso8601[0]);
         ++i) {
        tt_blob_t *name = &tt_g_tmzone_name_iso8601[i];
        if ((len >= name->len) &&
            (tt_strnicmp(start, (tt_char_t *)name->addr, name->len) == 0)) {
            tt_date_set_tmzone(d, i);
            return start + name->len;
        }
    }

    for (i = 0; i < sizeof(tt_g_tmzone_name_rfc1123) /
                        sizeof(tt_g_tmzone_name_rfc1123[0]);
         ++i) {
        tt_blob_t *name = &tt_g_tmzone_name_rfc1123[i];
        if ((len >= name->len) &&
            (tt_strnicmp(start, (tt_char_t *)name->addr, name->len) == 0)) {
            tt_date_set_tmzone(d, i);
            return start + name->len;
        }
    }

    TT_ERROR("not found time zone");
    return NULL;
}
