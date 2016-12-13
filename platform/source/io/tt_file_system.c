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

#include <io/tt_file_system.h>

#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <memory/tt_memory_alloc.h>
#include <misc/tt_assert.h>
#include <misc/tt_util.h>

#include <tt_cstd_api.h>
#include <tt_file_system_native.h>

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

static tt_result_t __fix_flag(IN OUT tt_u32_t *flag);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_fs_component_register()
{
    static tt_component_t comp;

    tt_component_itf_t itf = {
        __fs_component_init,
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

// ========================================
// file operations
// ========================================

tt_result_t tt_fcreate(IN const tt_char_t *path,
                       IN tt_u32_t flag,
                       IN tt_file_attr_t *attr)
{
    tt_file_attr_t __attr;

    TT_ASSERT(path != NULL);

    if (!TT_OK(__fix_flag(&flag))) {
        return TT_FAIL;
    }

    if (attr == NULL) {
        tt_file_attr_default(&__attr);
        attr = &__attr;
    }

    return tt_fcreate_ntv(path, flag, attr);
}

tt_result_t tt_fremove(IN const tt_char_t *path)
{
    TT_ASSERT(path != NULL);
    return tt_fremove_ntv(path);
}

tt_result_t tt_fopen(OUT tt_file_t *file,
                     IN const tt_char_t *path,
                     IN tt_u32_t flag,
                     IN tt_file_attr_t *attr)
{
    tt_file_attr_t __attr;

    TT_ASSERT(file != NULL);
    TT_ASSERT(path != NULL);

    if (!TT_OK(__fix_flag(&flag))) {
        return TT_FAIL;
    }

    if (attr == NULL) {
        tt_file_attr_default(&__attr);
        attr = &__attr;
    }

    // flag can be passed to portlayer directly
    if (!TT_OK(tt_fopen_ntv(&file->sys_file, path, flag, attr))) {
        return TT_FAIL;
    }

    file->flag = flag;
    tt_memcpy(&file->attr, &attr, sizeof(tt_file_attr_t));

    return TT_SUCCESS;
}

tt_result_t tt_fclose(IN tt_file_t *file)
{
    TT_ASSERT(file != NULL);

    if (TT_OK(tt_fclose_ntv(&file->sys_file))) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

void tt_file_attr_default(IN tt_file_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_memset(attr, 0, sizeof(tt_file_attr_t));
}

tt_result_t tt_fread(IN tt_file_t *file,
                     OUT tt_u8_t *buf,
                     IN tt_u32_t buf_len,
                     OUT tt_u32_t *read_len)
{
    tt_u32_t len = 0;
    tt_result_t result = TT_SUCCESS;

    TT_ASSERT(file != NULL);
    TT_ASSERT(buf != NULL);

    if (buf_len == 0) {
        *read_len = 0;
        return TT_SUCCESS;
    }

    while (len < buf_len) {
        tt_u32_t n = 0;

        result = tt_fread_ntv(&file->sys_file,
                              TT_PTR_INC(tt_u8_t, buf, len),
                              buf_len - len,
                              &n);
        if (TT_OK(result)) {
            len += n;
        } else {
            break;
        }
    }
    TT_ASSERT(len <= buf_len);

    if (len > 0) {
        TT_SAFE_ASSIGN(read_len, len);
        return TT_SUCCESS;
    } else if (result == TT_END) {
        return TT_END;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_fwrite(IN tt_file_t *file,
                      IN tt_u8_t *buf,
                      IN tt_u32_t buf_len,
                      OUT tt_u32_t *write_len)
{
    tt_u32_t len = 0;
    tt_result_t result = TT_SUCCESS;

    TT_ASSERT(file != NULL);
    TT_ASSERT(buf != NULL);

    if (buf_len == 0) {
        *write_len = 0;
        return TT_SUCCESS;
    }

    while (len < buf_len) {
        tt_u32_t n = 0;

        result = tt_fwrite_ntv(&file->sys_file,
                               TT_PTR_INC(tt_u8_t, buf, len),
                               buf_len - len,
                               &n);
        if (TT_OK(result)) {
            len += n;
        } else {
            break;
        }
    }
    TT_ASSERT(len <= buf_len);

    if (len > 0) {
        TT_SAFE_ASSIGN(write_len, len);
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_fseek(IN tt_file_t *file,
                     IN tt_u32_t whence,
                     IN tt_s64_t distance,
                     OUT tt_u64_t *position)
{
    TT_ASSERT(file != NULL);

    return tt_fseek_ntv(&file->sys_file, whence, distance, position);
}

// ========================================
// directory operations
// ========================================

void tt_dir_attr_default(IN tt_dir_attr_t *attr)
{
    TT_ASSERT(attr != NULL);

    tt_memset(attr, 0, sizeof(tt_dir_attr_t));
}

tt_result_t tt_dcreate(IN const tt_char_t *path, IN tt_dir_attr_t *attr)
{
    tt_dir_attr_t __attr;

    TT_ASSERT(path != NULL);

    if (attr == NULL) {
        tt_dir_attr_default(&__attr);
        attr = &__attr;
    }

    return tt_dcreate_ntv(path, attr);
}

tt_result_t tt_dremove(IN const tt_char_t *path, IN tt_u32_t flag)
{
    TT_ASSERT(path != NULL);
    return tt_dremove_ntv(path, flag);
}

tt_result_t tt_dopen(OUT tt_dir_t *dir,
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

    if (!TT_OK(tt_dopen_ntv(&dir->sys_dir, path, attr))) {
        return TT_FAIL;
    }

    tt_memcpy(&dir->attr, &attr, sizeof(tt_dir_attr_t));

    return TT_SUCCESS;
}

tt_result_t tt_dclose(OUT tt_dir_t *dir)
{
    TT_ASSERT(dir != NULL);

    if (TT_OK(tt_dclose_ntv(&dir->sys_dir))) {
        return TT_SUCCESS;
    } else {
        return TT_FAIL;
    }
}

tt_result_t tt_dread(IN tt_dir_t *dir,
                     IN tt_u32_t flag,
                     OUT tt_dir_entry_t *dentry)
{
    TT_ASSERT(dir != NULL);
    TT_ASSERT(dentry != NULL);

    // pass flag directly
    return tt_dread_ntv(&dir->sys_dir, flag, dentry);
}

// ========================================
// misc
// ========================================

tt_result_t tt_fcontent(IN const tt_char_t *path, OUT tt_blob_t *content)
{
    tt_file_t file;
    tt_u64_t file_len;
    tt_u8_t *p;
    tt_u32_t read_len = 0;

    tt_u32_t __done = 0;
#define __P2F_FOPEN (1 << 0)
#define __P2F_TALLOC (1 << 1)

    TT_ASSERT(path != NULL);
    TT_ASSERT(content != NULL);

    // get file size
    if (!TT_OK(tt_fopen(&file, path, TT_FO_RDONLY, NULL))) {
        goto f2b_fail;
    }
    __done |= __P2F_FOPEN;

    if (!TT_OK(tt_fseek(&file, TT_FPOS_END, 0, &file_len))) {
        goto f2b_fail;
    }
    if (file_len <= 0) {
        TT_ERROR("invalid file size[%ld]", file_len);
        goto f2b_fail;
    }
    if (file_len > 0x7fffffff) {
        TT_ERROR("too large file[%ld]", file_len);
        goto f2b_fail;
    }

    // alloc and read
    p = (tt_u8_t *)tt_mem_alloc((tt_u32_t)file_len);
    if (p == NULL) {
        TT_ERROR("no mem for reading file");
        goto f2b_fail;
    }
    __done |= __P2F_TALLOC;

    if (!TT_OK(tt_fread(&file, p, (tt_u32_t)file_len, &read_len))) {
        TT_ERROR("reading failed");
        goto f2b_fail;
    }
    if (read_len != (tt_u32_t)file_len) {
        TT_ERROR("size[%u], read[%u]", (tt_u32_t)file_len, read_len);
        goto f2b_fail;
    }
    // next read should return TT_END

    content->addr = p;
    content->len = read_len;

    tt_fclose(&file);
    return TT_SUCCESS;

f2b_fail:

    if (__done & __P2F_TALLOC) {
        tt_mem_free(p);
    }

    if (__done & __P2F_FOPEN) {
        tt_fclose(&file);
    }

    return TT_FAIL;
}

tt_result_t __fs_component_init(IN tt_component_t *comp,
                                IN tt_profile_t *profile)
{
    tt_result_t result = TT_FAIL;

    result = tt_fs_component_init_ntv();
    if (!TT_OK(result)) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t __fix_flag(IN OUT tt_u32_t *flag)
{
    tt_u32_t __flag = *flag;

    if (__flag & TT_FO_APPEND) {
        // append implies write permission
        __flag |= TT_FO_WRONLY;
    }

    if ((__flag & TT_FO_EXCL) && !(__flag & TT_FO_CREAT)) {
        TT_ERROR("TT_FO_EXCL without TT_FO_CREAT");
        return TT_FAIL;
    }

    if (__flag & TT_FO_TRUNC) {
        // truncate implies write permission
        __flag |= TT_FO_WRONLY;

        // as the behavior of O_TRUNC when file does not
        // exist is not specified, we rule that TT_FO_TRUNC
        // would create an empty file if the file does not
        // exsit
        __flag |= TT_FO_CREAT;
    }

    if ((__flag & TT_FO_HINT_SEQUENTIAL) && (__flag & TT_FO_HINT_RANDOM)) {
        TT_ERROR("TT_FO_HINT_SEQUENTIAL and TT_FO_HINT_RANDOM both specified");
        return TT_FAIL;
    }

    *flag = __flag;
    return TT_SUCCESS;
}
