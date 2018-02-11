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

#include <io/tt_file_system.h>

#include <algorithm/tt_buffer.h>
#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <io/tt_io_worker_group.h>
#include <misc/tt_assert.h>
#include <os/tt_atomic.h>

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

static tt_result_t __fs_component_init(IN tt_component_t *comp,
                                       IN tt_profile_t *profile);

static void __fs_component_exit(IN tt_component_t *comp);

static tt_char_t *__parent_dir(IN const tt_char_t *path);

static tt_atomic_s32_t __file_opened;

static tt_atomic_s32_t __dir_opened;

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_fs_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __fs_component_init, __fs_component_exit,
    };

    // init component
    tt_component_init(&comp,
                      TT_COMPONENT_FILE_SYSTEM,
                      "File System",
                      NULL,
                      &itf);

    // register component
    tt_component_register(&comp);
}

void tt_fs_status_dump(IN tt_u32_t flag)
{
    if (flag & TT_FS_STATUS_FILE) {
        tt_printf("%s[%d files] are opened\n",
                  TT_COND(flag & TT_FS_STATUS_PREFIX, "<<FS>> ", ""),
                  tt_atomic_s32_get(&__file_opened));
    }

    if (flag & TT_FS_STATUS_DIR) {
        tt_printf("%s[%d directories] are opened\n",
                  TT_COND(flag & TT_FS_STATUS_PREFIX, "<<FS>> ", ""),
                  tt_atomic_s32_get(&__dir_opened));
    }

    if (flag & TT_FS_STATUS_NATIVE) {
        tt_fs_status_dump_ntv(flag);
    }
}

void tt_file_attr_default(IN tt_file_attr_t *attr)
{
    TT_ASSERT(attr != NULL);
}

tt_result_t tt_fcreate(IN const tt_char_t *path, IN OPT tt_file_attr_t *attr)
{
    tt_file_attr_t __attr;
    tt_char_t *parent;

    TT_ASSERT(path != NULL);

    if (attr == NULL) {
        tt_file_attr_default(&__attr);
        attr = &__attr;
    }

    parent = __parent_dir(path);
    if (parent != NULL) {
        tt_result_t result = tt_dcreate(parent, NULL);
        tt_free(parent);
        if (TT_OK(result) || (result == TT_E_EXIST)) {
            return tt_fcreate_ntv(path, attr);
        } else {
            return result;
        }
    } else {
        return tt_fcreate_ntv(path, attr);
    }
}

tt_result_t tt_fcreate_temp(IN OUT tt_char_t *path, IN OPT tt_file_attr_t *attr)
{
    tt_char_t *x;

    TT_ASSERT(path != NULL);

    x = path + tt_strlen(path) - 1;
    while ((x >= path) && (*x == 'X')) {
        --x;
    }
    ++x;

    if (*x == 'X') {
        static tt_char_t t[62] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        tt_char_t *p;
        tt_u32_t ntry;
        tt_result_t result;

        p = x;
        ntry = 3;
        while (ntry-- != 0) {
            while (*p == 'X') {
                *p++ = t[tt_rand_u32() % sizeof(t)];
            }

            result = tt_fcreate(path, attr);
            if (TT_OK(result)) {
                return TT_SUCCESS;
            } else if (result != TT_E_EXIST) {
                break;
            }
        }
        return result;
    } else {
        return tt_fcreate(path, attr);
    }
}

tt_result_t tt_fremove(IN const tt_char_t *path)
{
    TT_ASSERT(path != NULL);

    return tt_fremove_ntv(path);
}

tt_result_t tt_fopen(IN tt_file_t *file,
                     IN const tt_char_t *path,
                     IN tt_u32_t flag,
                     IN OPT tt_file_attr_t *attr)
{
    tt_file_attr_t __attr;
    tt_char_t *parent;
    tt_result_t result;

    TT_ASSERT(file != NULL);
    TT_ASSERT(path != NULL);

    if (attr == NULL) {
        tt_file_attr_default(&__attr);
        attr = &__attr;
    }

    if ((flag & TT_FO_RLOCK) & (flag & TT_FO_WLOCK)) {
        flag &= ~TT_FO_RLOCK;
    }

    if ((flag & TT_FO_CREAT_DIR) && ((parent = __parent_dir(path)) != NULL)) {
        result = tt_dcreate(parent, NULL);
        tt_free(parent);
        if (TT_OK(result)) {
            flag |= TT_FO_CREAT;
            result = tt_fopen_ntv(&file->sys_file, path, flag, attr);
        }
    } else {
        result = tt_fopen_ntv(&file->sys_file, path, flag, attr);
    }

    if (TT_OK(result)) {
        tt_atomic_s32_inc(&__file_opened);
        return TT_SUCCESS;
    } else {
        return result;
    }
}

void tt_fclose(IN tt_file_t *file)
{
    tt_s32_t n;

    TT_ASSERT(file != NULL);

    tt_fclose_ntv(&file->sys_file);

    n = tt_atomic_s32_dec(&__file_opened);
    TT_ASSERT(n >= 0);
}

tt_u8_t *tt_fcontent(IN const tt_char_t *path, OUT OPT tt_u64_t *size)
{
    tt_file_t f;
    tt_u8_t *buf;
    tt_u64_t len;

    TT_ASSERT(path != NULL);

    if (!TT_OK(tt_fopen(&f, path, TT_FO_READ, NULL))) {
        return NULL;
    }

    if (!TT_OK(tt_fseek(&f, TT_FSEEK_END, 0, &len))) {
        tt_fclose(&f);
        return NULL;
    }

    buf = tt_malloc((size_t)len);
    if (buf == NULL) {
        TT_ERROR("no mem for file content");
        tt_fclose(&f);
        return NULL;
    }

    if (!TT_OK(tt_fseek(&f, TT_FSEEK_BEGIN, 0, NULL)) ||
        !TT_OK(tt_fread(&f, buf, (tt_u32_t)len, NULL))) {
        tt_free(buf);
        tt_fclose(&f);
        return NULL;
    }
    tt_fclose(&f);

    TT_SAFE_ASSIGN(size, len);
    return buf;
}

tt_result_t tt_fcontent_buf(IN const tt_char_t *path, OUT tt_buf_t *buf)
{
    tt_file_t f;
    tt_u64_t len;

    TT_ASSERT(path != NULL);
    TT_ASSERT(buf != NULL);

    if (!TT_OK(tt_fopen(&f, path, TT_FO_READ, NULL))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_fseek(&f, TT_FSEEK_END, 0, &len))) {
        tt_fclose(&f);
        return TT_FAIL;
    }

    if (!TT_OK(tt_buf_reserve(buf, (tt_u32_t)len))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_fseek(&f, TT_FSEEK_BEGIN, 0, NULL)) ||
        !TT_OK(tt_fread(&f, TT_BUF_WPOS(buf), (tt_u32_t)len, NULL))) {
        tt_fclose(&f);
        return TT_FAIL;
    }
    tt_buf_inc_wp(buf, (tt_u32_t)len);

    tt_fclose(&f);
    return TT_SUCCESS;
}

tt_result_t tt_fsize(IN const tt_char_t *path, OUT tt_u64_t *size)
{
    tt_file_t f;
    tt_u64_t len;

    TT_ASSERT(path != NULL);
    TT_ASSERT(size != NULL);

    if (!TT_OK(tt_fopen(&f, path, TT_FO_READ, NULL))) {
        return TT_FAIL;
    }

    if (!TT_OK(tt_fseek(&f, TT_FSEEK_END, 0, &len))) {
        tt_fclose(&f);
        return TT_FAIL;
    }

    tt_fclose(&f);
    *size = len;
    return TT_SUCCESS;
}

tt_result_t tt_fstat_path(IN const tt_char_t *path, OUT tt_fstat_t *fstat)
{
    tt_file_t f;
    tt_result_t result;

    TT_ASSERT(path != NULL);
    TT_ASSERT(fstat != NULL);

    result = tt_fopen(&f, path, TT_FO_READ, NULL);
    if (!TT_OK(result)) {
        return result;
    }

    result = tt_fstat(&f, fstat);
    if (!TT_OK(result)) {
        tt_fclose(&f);
        return result;
    }

    tt_fclose(&f);
    return TT_SUCCESS;
}

tt_result_t tt_futime(IN tt_file_t *file,
                      IN OPT tt_date_t *accessed,
                      IN OPT tt_date_t *modified)
{
    tt_date_t a, m;

    TT_ASSERT(file != NULL);

    if (accessed == NULL) {
        tt_date_now(&a);
        accessed = &a;
    }

    if (modified == NULL) {
        tt_date_now(&m);
        modified = &m;
    }

    return tt_futime_ntv(&file->sys_file, accessed, modified);
}

void tt_dir_attr_default(IN tt_dir_attr_t *attr)
{
    TT_ASSERT(attr != NULL);
}

tt_result_t tt_dcreate(IN const tt_char_t *path, IN tt_dir_attr_t *attr)
{
    tt_dir_attr_t __attr;
    tt_char_t sep;
    tt_u32_t len, pos;
    tt_char_t *p;
    const tt_char_t *prev, *s;
    tt_result_t result = TT_FAIL;

    TT_ASSERT(path != NULL);

    if (attr == NULL) {
        tt_dir_attr_default(&__attr);
        attr = &__attr;
    }

    if (tt_strchr(path, '/') != NULL) {
        sep = '/';
    } else if (tt_strchr(path, '\\') != NULL) {
        sep = '\\';
    } else {
        return tt_dcreate_ntv(path, attr);
    }

    len = (tt_u32_t)tt_strlen(path);
    p = tt_zalloc(len + 1);
    if (p == NULL) {
        return TT_E_NOMEM;
    }

    pos = 0;
    prev = path;
    s = path;
    while ((s = tt_strchr(s, sep)) != NULL) {
        tt_u32_t n;

        // unix absolute path: /a/b/...
        if (s == path) {
            s += 1;
            continue;
        }

        // windows absolute path: c:/...
        if ((s == (path + 2)) && (path[1] == ':')) {
            s += 3;
            continue;
        }

        n = (tt_u32_t)(s - prev);
        tt_memcpy(p + pos, prev, n);
        pos += n;
        if (!tt_fs_exist(p) && !TT_OK((result = tt_dcreate_ntv(p, attr)))) {
            goto done;
        }

        prev = s;
        ++s;
    }
    if (prev < (path + len - 1)) {
        tt_memcpy(p + pos, prev, path + len - prev);
        if (!tt_fs_exist(p) && !TT_OK((result = tt_dcreate_ntv(p, attr)))) {
            goto done;
        }
    }

    if (!TT_OK(result)) {
        // reaching here but result was not changed, means all dir exist
        result = TT_E_EXIST;
    }

done:
    tt_free(p);
    return result;
}

tt_result_t tt_dcreate_temp(IN OUT tt_char_t *path, IN OPT tt_dir_attr_t *attr)
{
    tt_char_t *x;

    TT_ASSERT(path != NULL);

    x = path + tt_strlen(path) - 1;
    while ((x >= path) && (*x == 'X')) {
        --x;
    }
    ++x;

    if (*x == 'X') {
        static tt_char_t t[62] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        tt_char_t *p;
        tt_u32_t ntry;
        tt_result_t result;

        p = x;
        ntry = 3;
        while (ntry-- != 0) {
            while (*p == 'X') {
                *p++ = t[tt_rand_u32() % sizeof(t)];
            }

            result = tt_dcreate(path, attr);
            if (TT_OK(result)) {
                return TT_SUCCESS;
            } else if (result != TT_E_EXIST) {
                break;
            }
        }
        return result;
    } else {
        return tt_dcreate(path, attr);
    }
}

tt_result_t tt_dremove(IN const tt_char_t *path)
{
    TT_ASSERT(path != NULL);

    return tt_dremove_ntv(path);
}

tt_result_t tt_dopen(IN tt_dir_t *dir,
                     IN const tt_char_t *path,
                     IN tt_dir_attr_t *attr)
{
    tt_dir_attr_t __attr;

    TT_ASSERT(dir != NULL);
    TT_ASSERT(path != NULL);

    if (attr == NULL) {
        tt_dir_attr_default(&__attr);
        attr = &__attr;
    }

    return tt_dopen_ntv(&dir->sys_dir, path, attr);
}

void tt_dclose(IN tt_dir_t *dir)
{
    TT_ASSERT(dir != NULL);

    tt_dclose_ntv(&dir->sys_dir);
}

tt_bool_t tt_fs_exist(IN const tt_char_t *path)
{
    TT_ASSERT(path != NULL);

    return tt_fs_exist_ntv(path);
}

tt_result_t tt_fs_rename(IN const tt_char_t *from, IN const tt_char_t *to)
{
    TT_ASSERT(from != NULL);
    TT_ASSERT(to != NULL);

    return tt_fs_rename_ntv(from, to);
}

tt_result_t tt_fs_link(IN const tt_char_t *path, IN const tt_char_t *link)
{
    TT_ASSERT(path != NULL);
    TT_ASSERT(link != NULL);

    return tt_fs_link_ntv(path, link);
}

tt_result_t tt_fs_symlink(IN const tt_char_t *path, IN const tt_char_t *link)
{
    TT_ASSERT(path != NULL);
    TT_ASSERT(link != NULL);

    return tt_fs_symlink_ntv(path, link);
}

tt_result_t tt_fs_readlink(IN const tt_char_t *link,
                           OUT tt_char_t *path,
                           IN tt_u32_t len)
{
    TT_ASSERT(link != NULL);
    TT_ASSERT(path != NULL);

    return tt_fs_readlink_ntv(link, path, len);
}

tt_result_t tt_fs_realpath(IN const tt_char_t *path,
                           OUT tt_char_t *resolved,
                           IN tt_u32_t len)
{
    TT_ASSERT(path != NULL);
    TT_ASSERT(resolved != NULL);

    if (len != 0) {
        return tt_fs_realpath_ntv(path, resolved, len);
    } else {
        TT_ERROR("len can not be 0");
        return TT_E_BADARG;
    }
}

tt_result_t __fs_component_init(IN tt_component_t *comp,
                                IN tt_profile_t *profile)
{
    if (!TT_OK(tt_fs_component_init_ntv())) {
        return TT_FAIL;
    }

    tt_atomic_s32_set(&__file_opened, 0);
    tt_atomic_s32_set(&__dir_opened, 0);

    return TT_SUCCESS;
}

void __fs_component_exit(IN tt_component_t *comp)
{
    tt_fs_status_dump(TT_FS_STATUS_ALL);

    tt_fs_component_exit_ntv();
}

tt_char_t *__parent_dir(IN const tt_char_t *path)
{
    tt_char_t *p;
    tt_u32_t len;

    p = tt_strrchr(path, '/');
#if TT_ENV_OS_IS_WINDOWS
    if (p == NULL) {
        p = tt_strrchr(path, '\\');
    }
#endif

    if ((p == NULL) || ((p == (path + 1)) && (path[0] == '.')) ||
        ((p == (path + 2)) && (path[0] == '.') && (path[1] == '.'))) {
        return NULL;
    }

    len = (tt_u32_t)(p - path);
    p = tt_malloc(len + 1);
    if (p != NULL) {
        tt_memcpy(p, path, len);
        p[len] = 0;
        return p;
    } else {
        TT_ERROR("no mem for parent path");
        return NULL;
    }
}
