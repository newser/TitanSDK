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
@file tt_fpath.h
@brief file system path

this file defines file system path
*/

#ifndef __TT_FPATH__
#define __TT_FPATH__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_string_common.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_string_t s;
} tt_fpath_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_inline void tt_fpath_init(IN tt_fpath_t *fp)
{
    tt_string_init(&fp->s, NULL);
}

tt_export tt_result_t tt_fpath_create(IN tt_fpath_t *fp, const tt_char_t *path);

tt_inline void tt_fpath_destroy(IN tt_fpath_t *fp)
{
    tt_string_destroy(&fp->s);
}

tt_inline tt_result_t tt_fpath_copy(IN tt_fpath_t *dst, IN tt_fpath_t *src)
{
    return tt_string_copy(&dst->s, &src->s);
}

tt_inline const tt_char_t *tt_fpath_cstr(IN tt_fpath_t *fp)
{
    return tt_string_cstr(&fp->s);
}

tt_inline tt_string_t *tt_fpath_string(IN tt_fpath_t *fp)
{
    return &fp->s;
}

tt_inline tt_bool_t tt_fpath_empty(IN tt_fpath_t *fp)
{
    return tt_string_empty(&fp->s);
}

tt_inline tt_s32_t tt_fpath_cmp(IN tt_fpath_t *fp, IN const tt_char_t *other)
{
    return tt_string_cmp(&fp->s, other);
}

tt_inline tt_bool_t tt_fpath_startwith(IN tt_fpath_t *fp,
                                       IN const tt_char_t *other)
{
    return tt_string_startwith(&fp->s, other);
}

tt_inline tt_bool_t tt_fpath_endwith(IN tt_fpath_t *fp,
                                     IN const tt_char_t *other)
{
    return tt_string_endwith(&fp->s, other);
}

tt_export tt_bool_t tt_fpath_is_file(IN tt_fpath_t *fp);

tt_export const tt_char_t *tt_fpath_filename(IN tt_fpath_t *fp);

tt_export tt_result_t tt_fpath_set_filename(IN tt_fpath_t *fp,
                                            IN const tt_char_t *filename);

tt_export tt_result_t tt_fpath_set_basename(IN tt_fpath_t *fp,
                                            IN const tt_char_t *basename);

tt_export tt_result_t tt_fpath_set_extension(IN tt_fpath_t *fp,
                                             IN const tt_char_t *extension);

tt_export tt_bool_t tt_fpath_is_dir(IN tt_fpath_t *fp);

tt_export tt_result_t tt_fpath_dir(IN tt_fpath_t *fp, OUT tt_fpath_t *dir);

tt_export tt_result_t tt_fpath_parent(IN tt_fpath_t *fp,
                                      OUT tt_fpath_t *parent);

tt_export tt_result_t tt_fpath_to_parent(IN tt_fpath_t *fp);

tt_export tt_result_t tt_fpath_root(IN tt_fpath_t *fp, OUT tt_fpath_t *root);

tt_export tt_result_t tt_fpath_to_root(IN tt_fpath_t *fp);

tt_export tt_result_t tt_fpath_sibling(IN tt_fpath_t *fp,
                                       IN const tt_char_t *sibling,
                                       OUT tt_fpath_t *sibling_fp);

tt_export tt_result_t tt_fpath_to_sibling(IN tt_fpath_t *fp,
                                          IN const tt_char_t *sibling);

tt_export tt_result_t tt_fpath_child(IN tt_fpath_t *fp,
                                     IN const tt_char_t *child,
                                     OUT tt_fpath_t *child_fp);

tt_export tt_result_t tt_fpath_to_child(IN tt_fpath_t *fp,
                                        IN const tt_char_t *child);

tt_export tt_bool_t tt_fpath_is_absolute(IN tt_fpath_t *fp);

tt_export tt_result_t tt_fpath_absolute(IN tt_fpath_t *fp, OUT tt_fpath_t *abs);

tt_export tt_result_t tt_fpath_to_absolute(IN tt_fpath_t *fp);

tt_inline tt_bool_t tt_fpath_is_relative(IN tt_fpath_t *fp)
{
    return !tt_fpath_is_absolute(fp);
}

tt_export tt_result_t tt_fpath_relative(IN tt_fpath_t *fp,
                                        IN const tt_char_t *other,
                                        OUT tt_fpath_t *rel);

tt_export tt_result_t tt_fpath_to_relative(IN tt_fpath_t *fp,
                                           IN const tt_char_t *other);

#endif // __TT_FPATH__
