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

#include <algorithm/tt_buffer.h>
#include <init/tt_component.h>
#include <init/tt_profile.h>
#include <io/tt_io_worker_group.h>
#include <misc/tt_assert.h>

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

void tt_file_attr_default(IN tt_file_attr_t *attr)
{
    TT_ASSERT(attr != NULL);
}

tt_result_t tt_fcreate(IN const tt_char_t *path, IN OPT tt_file_attr_t *attr)
{
    tt_file_attr_t __attr;

    TT_ASSERT(path != NULL);

    if (attr == NULL) {
        tt_file_attr_default(&__attr);
        attr = &__attr;
    }

    return tt_fcreate_ntv(path, attr);
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

    TT_ASSERT(file != NULL);
    TT_ASSERT(path != NULL);

    if (attr == NULL) {
        tt_file_attr_default(&__attr);
        attr = &__attr;
    }

    return tt_fopen_ntv(&file->sys_file, path, flag, attr);
}

void tt_fclose(IN tt_file_t *file)
{
    TT_ASSERT(file != NULL);

    tt_fclose_ntv(&file->sys_file);
}

tt_u8_t *tt_fcontent(IN const tt_char_t *path, OUT OPT tt_u64_t *size)
{
    tt_file_t f;
    tt_u8_t *buf;
    tt_u64_t len;

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

    TT_SAFE_ASSIGN(size, len);
    return buf;
}

tt_result_t tt_fcontent_buf(IN const tt_char_t *path, OUT tt_buf_t *buf)
{
    tt_file_t f;
    tt_u64_t len;

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

void tt_dir_attr_default(IN tt_dir_attr_t *attr)
{
    TT_ASSERT(attr != NULL);
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

tt_result_t __fs_component_init(IN tt_component_t *comp,
                                IN tt_profile_t *profile)
{
    if (!TT_OK(tt_fs_component_init_ntv())) {
        return TT_FAIL;
    }

    return TT_SUCCESS;
}
