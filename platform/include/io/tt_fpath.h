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

#include <algorithm/ptr/tt_ptr_queue.h>
#include <algorithm/tt_string_common.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_FPATH_SEP_WINDOWS '\\'
#define TT_FPATH_SEP_UNIX '/'

#if TT_ENV_OS_IS_WINDOWS
#define TT_FPATH_SEP TT_FPATH_SEP_WINDOWS
#else
#define TT_FPATH_SEP TT_FPATH_SEP_UNIX
#endif

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    const tt_char_t *basename;
    const tt_char_t *extension;
    tt_ptrq_t dir;
    tt_string_t path;
    tt_char_t root[6];
    tt_char_t sep;
    tt_bool_t modified : 1;
} tt_fpath_t;

typedef struct
{
    tt_ptrq_iter_t dir_iter;
    tt_bool_t file_accessed : 1;
} tt_fpath_iter_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export void tt_fpath_init(IN tt_fpath_t *fp, IN tt_char_t separator);

tt_export tt_result_t tt_fpath_create(IN tt_fpath_t *fp,
                                      IN const tt_char_t *path,
                                      IN tt_u32_t path_len,
                                      IN tt_char_t separator);

tt_inline tt_result_t tt_fpath_create_cstr(IN tt_fpath_t *fp,
                                           IN const tt_char_t *path,
                                           IN tt_char_t separator)
{
    return tt_fpath_create(fp, path, tt_strlen(path), separator);
}

tt_export void tt_fpath_destroy(IN tt_fpath_t *fp);

tt_export void tt_fpath_clear(IN tt_fpath_t *fp);

tt_export tt_result_t tt_fpath_set(IN tt_fpath_t *fp,
                                   const tt_char_t *path,
                                   IN tt_u32_t path_len);

tt_inline tt_result_t tt_fpath_set_cstr(IN tt_fpath_t *fp,
                                        const tt_char_t *path)
{
    return tt_fpath_set(fp, path, tt_strlen(path));
}

tt_export tt_result_t tt_fpath_copy(IN tt_fpath_t *dst, IN tt_fpath_t *src);

tt_export const tt_char_t *tt_fpath_cstr(IN tt_fpath_t *fp);

tt_export tt_string_t *tt_fpath_string(IN tt_fpath_t *fp);

tt_export tt_u32_t tt_fpath_len(IN tt_fpath_t *fp);

tt_export tt_s32_t tt_fpath_cmp(IN tt_fpath_t *fp, IN const tt_char_t *other);

tt_export tt_bool_t tt_fpath_startwith(IN tt_fpath_t *fp,
                                       IN const tt_char_t *other);

tt_export tt_bool_t tt_fpath_endwith(IN tt_fpath_t *fp,
                                     IN const tt_char_t *other);

tt_inline tt_bool_t tt_fpath_is_file(IN tt_fpath_t *fp)
{
    return (((fp->basename != NULL) && (fp->basename[0] != 0)) ||
            ((fp->extension != NULL) && (fp->extension[0] != 0)));
}

tt_export const tt_char_t *tt_fpath_get_filename(IN tt_fpath_t *fp);

tt_export tt_result_t tt_fpath_set_filename(IN tt_fpath_t *fp,
                                            IN const tt_char_t *filename);

tt_inline const tt_char_t *tt_fpath_get_basename(IN tt_fpath_t *fp)
{
    return TT_COND(fp->basename != NULL, fp->basename, "");
}

tt_export tt_result_t tt_fpath_set_basename(IN tt_fpath_t *fp,
                                            IN const tt_char_t *basename);

tt_inline const tt_char_t *tt_fpath_get_extension(IN tt_fpath_t *fp)
{
    return TT_COND(fp->extension != NULL, fp->extension, "");
}

tt_export tt_result_t tt_fpath_set_extension(IN tt_fpath_t *fp,
                                             IN const tt_char_t *extension);

tt_inline tt_bool_t tt_fpath_is_dir(IN tt_fpath_t *fp)
{
    return !tt_fpath_is_file(fp);
}

tt_export tt_result_t tt_fpath_to_dir(IN tt_fpath_t *fp);

tt_export tt_result_t tt_fpath_get_parent(IN tt_fpath_t *fp,
                                          OUT tt_fpath_t *parent);

tt_export tt_result_t tt_fpath_to_parent(IN tt_fpath_t *fp);

tt_inline const tt_char_t *tt_fpath_get_root(IN tt_fpath_t *fp)
{
    return fp->root;
}

tt_export void tt_fpath_to_root(IN tt_fpath_t *fp);

tt_export tt_result_t tt_fpath_get_sibling(IN tt_fpath_t *fp,
                                           IN const tt_char_t *sibling,
                                           OUT tt_fpath_t *sibling_fp);

tt_export tt_result_t tt_fpath_to_sibling(IN tt_fpath_t *fp,
                                          IN const tt_char_t *sibling);

tt_export tt_result_t tt_fpath_get_child(IN tt_fpath_t *fp,
                                         IN const tt_char_t *child,
                                         OUT tt_fpath_t *child_fp);

tt_export tt_result_t tt_fpath_to_child(IN tt_fpath_t *fp,
                                        IN const tt_char_t *child);

tt_inline tt_bool_t tt_fpath_is_absolute(IN tt_fpath_t *fp)
{
    return TT_BOOL(fp->root[0] != 0);
}

tt_export tt_result_t tt_fpath_get_absolute(IN tt_fpath_t *fp,
                                            OUT tt_fpath_t *abs);

// fp must have same separator as returned path by tt_current_path()
tt_export tt_result_t tt_fpath_to_absolute(IN tt_fpath_t *fp);

tt_inline tt_bool_t tt_fpath_is_relative(IN tt_fpath_t *fp)
{
    return !tt_fpath_is_absolute(fp);
}

tt_inline tt_bool_t tt_fpath_is_empty(IN tt_fpath_t *fp)
{
    return TT_BOOL(tt_fpath_cstr(fp)[0] == 0);
}

tt_inline tt_u32_t tt_fpath_count(IN tt_fpath_t *fp)
{
    return tt_ptrq_count(&fp->dir) + TT_COND(tt_fpath_is_file(fp), 1, 0);
}

tt_inline tt_bool_t tt_fpath_empty(IN tt_fpath_t *fp)
{
    return TT_BOOL(tt_fpath_count(fp) == 0);
}

tt_export const tt_char_t *tt_fpath_get_name(IN tt_fpath_t *fp,
                                             IN tt_u32_t idx);

tt_export tt_result_t tt_fpath_set_name(IN tt_fpath_t *fp,
                                        IN tt_u32_t idx,
                                        IN const tt_char_t *name);

tt_inline void tt_fpath_iter(IN tt_fpath_t *fp, OUT tt_fpath_iter_t *iter)
{
    tt_ptrq_iter(&fp->dir, &iter->dir_iter);
    iter->file_accessed = TT_FALSE;
}

tt_export const tt_char_t *tt_fpath_iter_next(IN OUT tt_fpath_iter_t *iter);

tt_export tt_result_t tt_fpath_get_sub(IN tt_fpath_t *fp,
                                       IN tt_u32_t from,
                                       IN tt_u32_t num,
                                       OUT tt_fpath_t *sub);

tt_export tt_result_t tt_fpath_normalize(IN tt_fpath_t *fp);

tt_export tt_result_t tt_fpath_resolve(IN tt_fpath_t *fp,
                                       IN tt_fpath_t *other,
                                       OUT tt_fpath_t *resolved);

tt_export tt_result_t tt_fpath_relativize(IN tt_fpath_t *fp,
                                          IN tt_fpath_t *other,
                                          OUT tt_fpath_t *relative);

#endif // __TT_FPATH__
