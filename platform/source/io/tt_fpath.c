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
#include <os/tt_process.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __SEP(path) TT_COND(tt_strchr((path), '\\') != NULL, '\\', '/')

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

static tt_result_t __fpath_parse(IN const tt_char_t *path,
                                 IN tt_char_t sep,
                                 OUT tt_fpath_t *fp);

static tt_result_t __fpath_parse_move(IN tt_fpath_t *fp,
                                      IN const tt_char_t *path);

static void __fpath_render(IN tt_fpath_t *fp);

static tt_result_t __dir_push(IN tt_fpath_t *fp, IN tt_char_t *name);

static tt_result_t __dir_push_n(IN tt_fpath_t *fp,
                                IN tt_char_t *name,
                                IN tt_u32_t len);

static tt_char_t *__dir_pop(IN tt_fpath_t *fp);

static void __dir_pop_free(IN tt_fpath_t *fp);

static void __dir_clear(IN tt_fpath_t *fp);

static tt_result_t __dir_copy(IN tt_fpath_t *dst, IN tt_fpath_t *src);

static tt_result_t __dir_move(IN tt_fpath_t *dst, IN tt_fpath_t *src);

static void __dir_destroy(IN tt_fpath_t *fp);

static tt_result_t __fname_parse(IN tt_fpath_t *fp,
                                 IN const tt_char_t *filename);

static void __fname_clear(IN tt_fpath_t *fp);

static tt_result_t __fname_copy(IN tt_fpath_t *dst, IN tt_fpath_t *src);

static void __fname_move(IN tt_fpath_t *dst, IN tt_fpath_t *src);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_fpath_init(IN tt_fpath_t *fp, IN tt_fpath_style_t style)
{
    fp->basename = NULL;
    fp->extension = NULL;
    tt_ptrq_init(&fp->dir, NULL);
    tt_string_init(&fp->path, NULL);
    fp->root[0] = 0;
    fp->modified = TT_FALSE;

    TT_ASSERT(TT_FPATH_STYLE_VALID(style));
    if (style == TT_FPATH_UNIX) {
        fp->sep = '/';
    } else if (style == TT_FPATH_WINDOWS) {
        fp->sep = '\\';
    } else {
#if TT_ENV_OS_IS_WINDOWS
        fp->sep = '\\';
#else
        fp->sep = '/';
#endif
    }
}

tt_result_t tt_fpath_create(IN tt_fpath_t *fp,
                            const tt_char_t *path,
                            IN tt_fpath_style_t style)
{
    tt_fpath_init(fp, style);

    // seperator is inferred by path conntent
    if (!TT_OK(__fpath_parse(path, __SEP(path), fp))) {
        // must destroy fpath if parsing fail
        tt_fpath_destroy(fp);
        return TT_FAIL;
    }

    fp->modified = TT_TRUE;
    return TT_SUCCESS;
}

void tt_fpath_destroy(IN tt_fpath_t *fp)
{
    __fname_clear(fp);
    __dir_destroy(fp);
    tt_string_destroy(&fp->path);
}

void tt_fpath_clear(IN tt_fpath_t *fp)
{
    __fname_clear(fp);
    __dir_clear(fp);
    tt_string_clear(&fp->path);
    fp->root[0] = 0;
    // keep fp->sep;
    fp->modified = TT_FALSE;
}

tt_result_t tt_fpath_set(IN tt_fpath_t *fp, const tt_char_t *path)
{
    tt_fpath_clear(fp);

    if (!TT_OK(__fpath_parse(path, __SEP(path), fp))) {
        // caller should destroy fp
        return TT_FAIL;
    }

    fp->modified = TT_TRUE;
    return TT_SUCCESS;
}

tt_result_t tt_fpath_copy(IN tt_fpath_t *dst, IN tt_fpath_t *src)
{
    if (!TT_OK(__fname_copy(dst, src))) {
        return TT_FAIL;
    }

    __dir_copy(dst, src);

    tt_memcpy(dst->root, src->root, sizeof(dst->root));
    TT_ASSERT(dst->sep = src->sep);
    dst->modified = TT_TRUE;

    return TT_SUCCESS;
}

const tt_char_t *tt_fpath_cstr(IN tt_fpath_t *fp)
{
    __fpath_render(fp);
    return tt_string_cstr(&fp->path);
}

tt_string_t *tt_fpath_string(IN tt_fpath_t *fp)
{
    __fpath_render(fp);
    return &fp->path;
}

tt_u32_t tt_fpath_len(IN tt_fpath_t *fp)
{
    __fpath_render(fp);
    return tt_string_len(&fp->path);
}

tt_s32_t tt_fpath_cmp(IN tt_fpath_t *fp, IN const tt_char_t *other)
{
    __fpath_render(fp);
    return tt_string_cmp(&fp->path, other);
}

tt_bool_t tt_fpath_startwith(IN tt_fpath_t *fp, IN const tt_char_t *other)
{
    __fpath_render(fp);
    return tt_string_startwith(&fp->path, other);
}

tt_bool_t tt_fpath_endwith(IN tt_fpath_t *fp, IN const tt_char_t *other)
{
    __fpath_render(fp);
    return tt_string_endwith(&fp->path, other);
}

const tt_char_t *tt_fpath_get_filename(IN tt_fpath_t *fp)
{
    tt_u32_t pos;

    __fpath_render(fp);

    pos = tt_string_rfind_c(&fp->path, fp->sep);
    if (pos != TT_POS_NULL) {
        return tt_string_subcstr(&fp->path, pos + 1, NULL);
    } else {
        return tt_string_cstr(&fp->path);
    }
}

tt_result_t tt_fpath_set_filename(IN tt_fpath_t *fp,
                                  IN const tt_char_t *filename)
{
    __fname_clear(fp);
    TT_DO(__fname_parse(fp, filename));

    fp->modified = TT_TRUE;
    return TT_SUCCESS;
}

tt_result_t tt_fpath_set_basename(IN tt_fpath_t *fp,
                                  IN const tt_char_t *basename)
{
    tt_char_t *b = tt_cstr_copy(basename);
    if (b != NULL) {
        if (fp->basename != NULL) {
            tt_free((void *)fp->basename);
        }
        fp->basename = b;

        fp->modified = TT_TRUE;
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_fpath_set_extension(IN tt_fpath_t *fp,
                                   IN const tt_char_t *extension)
{
    tt_char_t *e = tt_cstr_copy(extension);
    if (e != NULL) {
        if (fp->extension != NULL) {
            tt_free((void *)fp->extension);
        }
        fp->extension = e;

        fp->modified = TT_TRUE;
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_fpath_to_dir(IN tt_fpath_t *fp)
{
    tt_char_t *name;

    if ((fp->basename != NULL) && (fp->extension != NULL)) {
        tt_u32_t bn = tt_strlen(fp->basename);
        tt_u32_t en = tt_strlen(fp->extension);

        name = tt_malloc(bn + en + 2);
        if (name == NULL) {
            TT_ERROR("no mem for dir name");
            return TT_FAIL;
        }
        tt_memcpy(name, fp->basename, bn);
        name[bn] = '.';
        tt_memcpy(name + bn + 1, fp->extension, en);
        name[bn + 1 + en] = 0;
    } else if (fp->basename != NULL) {
        name = (tt_char_t *)fp->basename;
        fp->basename = NULL;
    } else if (fp->extension != NULL) {
        name = (tt_char_t *)fp->extension;
        fp->extension = NULL;
    } else {
        name = NULL;
    }

    if ((name == NULL) || TT_OK(tt_ptrq_push_tail(&fp->dir, name))) {
        return TT_SUCCESS;
    } else {
        tt_free(name);
        return TT_FAIL;
    }
}

tt_result_t tt_fpath_get_parent(IN tt_fpath_t *fp, OUT tt_fpath_t *parent)
{
    TT_DO(tt_fpath_copy(parent, fp));
    TT_DO(tt_fpath_to_parent(parent));

    return TT_SUCCESS;
}

tt_result_t tt_fpath_to_parent(IN tt_fpath_t *fp)
{
    if (tt_fpath_is_file(fp)) {
        // remove file name if it's file
        __fname_clear(fp);
    } else {
        tt_u32_t level = 1;
        tt_ptrq_t *q = &fp->dir;

        while ((level > 0) && !tt_ptrq_empty(q)) {
            tt_char_t *dir = (tt_char_t *)tt_ptrq_tail(q);
            if ((tt_strlen(dir) == 0) || (tt_strcmp(dir, ".") == 0)) {
            } else if (tt_strcmp(dir, "..") == 0) {
                ++level;
            } else {
                --level;
            }
            tt_ptrq_pop_tail(q);
            tt_free(dir);
        }

        if ((level > 0) && (tt_fpath_is_relative(fp))) {
            while (level-- > 0) {
                TT_DO(__dir_push(fp, ".."));
            }
        }
    }

    fp->modified = TT_TRUE;
    return TT_SUCCESS;
}

void tt_fpath_to_root(IN tt_fpath_t *fp)
{
    __fname_clear(fp);
    __dir_clear(fp);
    fp->modified = TT_TRUE;
}

tt_result_t tt_fpath_get_sibling(IN tt_fpath_t *fp,
                                 IN const tt_char_t *sibling,
                                 OUT tt_fpath_t *sibling_fp)
{
    // init sibling_fp
    if (tt_fpath_empty(sibling_fp)) {
        TT_DO(tt_fpath_copy(sibling_fp, fp));
    } else {
        TT_DO(tt_fpath_get_parent(fp, sibling_fp));
    }

    return __fpath_parse_move(sibling_fp, sibling);
}

tt_result_t tt_fpath_to_sibling(IN tt_fpath_t *fp, IN const tt_char_t *sibling)
{
    // move to parent if path is not empty
    if (!tt_fpath_empty(fp)) {
        tt_fpath_to_parent(fp);
    }

    return __fpath_parse_move(fp, sibling);
}

tt_result_t tt_fpath_get_child(IN tt_fpath_t *fp,
                               IN const tt_char_t *child,
                               OUT tt_fpath_t *child_fp)
{
    TT_DO(tt_fpath_copy(child_fp, fp));

    return tt_fpath_to_child(child_fp, child);
}

tt_result_t tt_fpath_to_child(IN tt_fpath_t *fp, IN const tt_char_t *child)
{
    // consider fp as an dir
    TT_DO(tt_fpath_to_dir(fp));

    // child of current dir: "./child"
    if (tt_fpath_empty(fp) && tt_fpath_is_relative(fp)) {
        TT_DO(__dir_push(fp, "."));
    }

    return __fpath_parse_move(fp, child);
}

tt_result_t tt_fpath_get_absolute(IN tt_fpath_t *fp, OUT tt_fpath_t *abs)
{
    tt_fpath_copy(abs, fp);

    return tt_fpath_to_absolute(abs);
}

tt_result_t tt_fpath_to_absolute(IN tt_fpath_t *fp)
{
    tt_char_t *dir;
    tt_fpath_t tmp;
    tt_result_t result;

    if (tt_fpath_is_absolute(fp)) {
        return TT_SUCCESS;
    }

    dir = tt_current_path(TT_TRUE);
    if (dir == NULL) {
        return TT_FAIL;
    }

    result = tt_fpath_create(&tmp, dir, TT_FPATH_AUTO);
    tt_free(dir);
    if (TT_OK(!result)) {
        return TT_FAIL;
    }

    // copy root
    tt_memcpy(fp->root, tmp.root, sizeof(fp->root));

    // prepend directories
    result = TT_FAIL;
    while ((dir = tt_ptrq_pop_tail(&tmp.dir)) != NULL) {
        if (!TT_OK(tt_ptrq_push_head(&fp->dir, dir))) {
            goto done;
        }
    }
    result = TT_SUCCESS;

done:
    tt_fpath_destroy(&tmp);
    return result;
}

const tt_char_t *tt_fpath_get_name(IN tt_fpath_t *fp, IN tt_u32_t idx)
{
    tt_u32_t n = tt_ptrq_count(&fp->dir);
    if (idx < n) {
        return tt_ptrq_get(&fp->dir, idx);
    } else if ((idx == n) && (tt_fpath_is_file(fp))) {
        return tt_fpath_get_filename(fp);
    } else {
        TT_ERROR("invalid fpath index: %d", idx);
        return NULL;
    }
}

tt_result_t tt_fpath_set_name(IN tt_fpath_t *fp,
                              IN tt_u32_t idx,
                              IN const tt_char_t *name)
{
    tt_u32_t n;

    if ((tt_strchr(name, '/') != NULL) || (tt_strchr(name, '\\') != NULL)) {
        TT_ERROR("fpath name can not include / or \\");
        return TT_FAIL;
    }

    n = tt_ptrq_count(&fp->dir);
    if (idx < n) {
        tt_ptr_t p, old_p;

        p = tt_cstr_copy(name);
        if (p != NULL) {
            old_p = tt_ptrq_set(&fp->dir, idx, p);
            TT_ASSERT(old_p != NULL);
            tt_free(old_p);
            return TT_SUCCESS;
        } else {
            TT_ERROR("fail to copy fpath name");
            return TT_FAIL;
        }
    } else if ((idx == n) && (tt_fpath_is_file(fp))) {
        return tt_fpath_set_filename(fp, name);
    } else {
        TT_ERROR("invalid fpath index: %d", idx);
        return NULL;
    }
}

tt_result_t __fpath_parse(IN const tt_char_t *path,
                          IN tt_char_t sep,
                          OUT tt_fpath_t *fp)
{
    tt_u32_t len = (tt_u32_t)tt_strlen(path);
    tt_char_t *pos, *prev;

    pos = (tt_char_t *)path;
    if (sep == '\\') {
        if ((len >= 3) && (path[1] == ':') && (path[2] == '\\')) {
            tt_memcpy(fp->root, path, 3);
            fp->root[3] = 0;
            pos = (tt_char_t *)&path[3];
        }
    } else {
        if ((len >= 1) && (path[0] == '/')) {
            fp->root[0] = '/';
            fp->root[1] = 0;
            pos = (tt_char_t *)&path[1];
        }
    }

    prev = pos;
    while ((pos = tt_strchr(pos, sep)) != NULL) {
        if (!TT_OK(__dir_push_n(fp, prev, (tt_u32_t)(pos - prev)))) {
            return TT_FAIL;
        }
        prev = ++pos;
    }
    if (prev < (path + len)) {
        tt_u32_t n = (tt_u32_t)(path + len - prev);
        if (((n == 1) && (prev[0] == '.')) ||
            ((n == 2) && (prev[0] == '.') && (prev[1] == '.'))) {
            // end with "." or ".."
            return __dir_push_n(fp, prev, n);
        } else {
            // end with a filename
            return __fname_parse(fp, prev);
        }
    } else {
        // end with a seperator
        TT_ASSERT(prev == (path + len));
        return TT_SUCCESS;
    }
}

tt_result_t __fpath_parse_move(IN tt_fpath_t *fp, IN const tt_char_t *path)
{
    tt_fpath_t tmp;

    TT_DO(tt_fpath_create(&tmp, path, TT_FPATH_AUTO));
    TT_DO_G(fail, __dir_move(fp, &tmp));
    __fname_move(fp, &tmp);
    tt_fpath_destroy(&tmp);

    fp->modified = TT_TRUE;
    return TT_SUCCESS;

fail:
    tt_fpath_destroy(&tmp);
    return TT_FAIL;
}

void __fpath_render(IN tt_fpath_t *fp)
{
    tt_string_t *s = &fp->path;
    tt_ptrq_iter_t iter;
    tt_char_t *dir;

    if (!fp->modified) {
        return;
    }

    tt_string_clear(s);

    // root
    TT_DO_G(fail, tt_string_append(s, fp->root));
    if (fp->sep == '\\') {
        tt_string_replace_c(s, '/', '\\');
    } else {
        tt_string_replace_c(s, '\\', '/');
    }

    // dir
    tt_ptrq_iter(&fp->dir, &iter);
    while ((dir = (tt_char_t *)tt_ptrq_iter_next(&iter)) != NULL) {
        TT_DO_G(fail, tt_string_append(s, dir));
        TT_DO_G(fail, tt_string_append_c(s, fp->sep));
    }

    // file
    if (fp->basename != NULL) {
        TT_DO_G(fail, tt_string_append(s, fp->basename));
    }
    if (fp->extension != NULL) {
        TT_DO_G(fail, tt_string_append_c(s, '.'));
        TT_DO_G(fail, tt_string_append(s, fp->extension));
    }

    fp->modified = TT_FALSE;
    return;

fail:
    TT_ERROR("fail to render path");
    tt_string_clear(&fp->path);
}

tt_result_t __dir_push(IN tt_fpath_t *fp, IN tt_char_t *name)
{
    tt_char_t *dir = tt_cstr_copy(name);
    if (dir != NULL) {
        return tt_ptrq_push_tail(&fp->dir, dir);
    } else {
        TT_ERROR("no mem for path dir");
        return TT_FAIL;
    }
}

tt_result_t __dir_push_n(IN tt_fpath_t *fp, IN tt_char_t *name, IN tt_u32_t len)
{
    tt_char_t *dir = tt_cstr_copy_n(name, len);
    if (dir != NULL) {
        return tt_ptrq_push_tail(&fp->dir, dir);
    } else {
        TT_ERROR("no mem for path dir");
        return TT_FAIL;
    }
}

tt_char_t *__dir_pop(IN tt_fpath_t *fp)
{
    return tt_ptrq_pop_head(&fp->dir);
}

void __dir_pop_free(IN tt_fpath_t *fp)
{
    tt_ptr_t p = tt_ptrq_pop_head(&fp->dir);
    if (p != NULL) {
        tt_free(p);
    }
}

void __dir_clear(IN tt_fpath_t *fp)
{
    tt_char_t *dir;
    while ((dir = (tt_char_t *)tt_ptrq_pop_head(&fp->dir)) != NULL) {
        tt_free(dir);
    }
}

tt_result_t __dir_copy(IN tt_fpath_t *dst, IN tt_fpath_t *src)
{
    tt_ptrq_iter_t iter;
    tt_char_t *dir;

    __dir_clear(dst);

    tt_ptrq_iter(&src->dir, &iter);
    while ((dir = (tt_char_t *)tt_ptrq_iter_next(&iter)) != NULL) {
        if (!TT_OK(__dir_push(dst, dir))) {
            return TT_FAIL;
        }
    }

    return TT_SUCCESS;
}

tt_result_t __dir_move(IN tt_fpath_t *dst, IN tt_fpath_t *src)
{
    tt_char_t *dir;

    while ((dir = (tt_char_t *)tt_ptrq_pop_head(&src->dir)) != NULL) {
        TT_DO(tt_ptrq_push_tail(&dst->dir, dir));
    }

    return TT_SUCCESS;
}

void __dir_destroy(IN tt_fpath_t *fp)
{
    __dir_clear(fp);
    tt_ptrq_destroy(&fp->dir);
}

tt_result_t __fname_parse(IN tt_fpath_t *fp, IN const tt_char_t *filename)
{
    tt_char_t *pos, *b = NULL, *e = NULL;

    pos = tt_strrchr(filename, '.');
    if (pos != NULL) {
        TT_DONN_G(fail,
                  b = tt_cstr_copy_n(filename, (tt_u32_t)(pos - filename)));

        ++pos;
        if (*pos != 0) {
            TT_DONN_G(fail, e = tt_cstr_copy(pos));
        }
    } else {
        TT_DONN_G(fail, b = tt_cstr_copy(filename));
    }

    fp->basename = b;
    fp->extension = e;
    return TT_SUCCESS;

fail:
    if (b != NULL) {
        tt_free(b);
    }

    if (e != NULL) {
        tt_free(e);
    }

    return TT_FAIL;
}

void __fname_clear(IN tt_fpath_t *fp)
{
    if (fp->basename != NULL) {
        tt_free((void *)fp->basename);
        fp->basename = NULL;
    }

    if (fp->extension != NULL) {
        tt_free((void *)fp->extension);
        fp->extension = NULL;
    }
}

tt_result_t __fname_copy(IN tt_fpath_t *dst, IN tt_fpath_t *src)
{
    tt_char_t *b = NULL, *e = NULL;

    if ((src->basename != NULL) &&
        ((b = tt_cstr_copy(src->basename)) == NULL)) {
        TT_ERROR("fail to copy basename");
        goto fail;
    }

    if ((src->extension != NULL) &&
        ((e = tt_cstr_copy(src->extension)) == NULL)) {
        TT_ERROR("fail to copy extension");
        goto fail;
    }

    if (dst->basename != NULL) {
        tt_free((void *)dst->basename);
    }
    dst->basename = b;

    if (dst->extension != NULL) {
        tt_free((void *)dst->extension);
    }
    dst->extension = e;

    return TT_SUCCESS;

fail:
    if (b != NULL) {
        tt_free(b);
    }

    if (e != NULL) {
        tt_free(e);
    }

    return TT_FAIL;
}

void __fname_move(IN tt_fpath_t *dst, IN tt_fpath_t *src)
{
    if (dst->basename != NULL) {
        tt_free((void *)dst->basename);
    }
    dst->basename = src->basename;
    src->basename = NULL;

    if (dst->extension != NULL) {
        tt_free((void *)dst->extension);
    }
    dst->extension = src->extension;
    src->extension = NULL;
}
