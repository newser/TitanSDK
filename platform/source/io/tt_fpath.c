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

#include <io/tt_fpath.h>

#include <algorithm/tt_algorithm_def.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#if TT_ENV_OS_IS_WINDOWS
#define __FSEP_C '\\'
#define __FSEP_S "\\"
#else
#define __FSEP_C '/'
#define __FSEP_S "/"
#endif

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

static void __fpath_normalize(IN tt_fpath_t *fp);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_fpath_create(IN tt_fpath_t *fp, const tt_char_t *path)
{
    TT_DO(tt_string_create(&fp->s, path, NULL));
    __fpath_normalize(fp);
    return TT_SUCCESS;
}

tt_bool_t tt_fpath_is_file(IN tt_fpath_t *fp)
{
    return !tt_fpath_empty(fp) && !tt_fpath_endwith(fp, __FSEP_S);
}

const tt_char_t *tt_fpath_filename(IN tt_fpath_t *fp)
{
    tt_u32_t pos = tt_string_rfind_c(&fp->s, __FSEP_C);
    if (pos != TT_POS_NULL) {
        return tt_string_subcstr(&fp->s, pos + 1, NULL);
    } else {
        return tt_string_cstr(&fp->s);
    }
}

tt_result_t tt_fpath_set_filename(IN tt_fpath_t *fp,
                                  IN const tt_char_t *filename)
{
    tt_u32_t pos = tt_string_rfind_c(&fp->s, __FSEP_C);
    if (pos != TT_POS_NULL) {
        return TT_SUCCESS; // tt_string_remove_range(&fp->s, pos, )(&fp->s,pos +
        // 1,NULL);
    } else {
        return tt_string_cstr(&fp->s);
    }
}

void __fpath_normalize(IN tt_fpath_t *fp)
{
#if TT_ENV_OS_IS_WINDOWS
    tt_string_replace_c(&fp->s, '/', '\\');
#else
    tt_string_replace_c(&fp->s, '\\', '/');
#endif
}
