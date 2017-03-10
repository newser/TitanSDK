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
@file tt_file_system.h
@brief file system APIs

this file defines file system APIs
*/

#ifndef __TT_FILE_SYSTEM__
#define __TT_FILE_SYSTEM__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_blob.h>

#include <tt_file_system_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_FPOS_BEGIN TT_FPOS_BEGIN_NTV
#define TT_FPOS_CUR TT_FPOS_CUR_NTV
#define TT_FPOS_END TT_FPOS_END_NTV
#define TT_FPOS_NULL (~0)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef enum {
    TT_FSTYPE_UNKNOWN,
    TT_FSTYPE_FILE,
    TT_FSTYPE_DIR,

    TT_FSTYPE_NUM
} tt_fs_type_t;
#define TT_FSTYPE_VALID(t) ((t) < TT_FSTYPE_NUM)

typedef struct tt_file_attr_s
{
    // to describe security related, such as access control
    // to be implemented..
    tt_u32_t security;
} tt_file_attr_t;

typedef struct tt_fs_file_s
{
    tt_file_ntv_t sys_file;

    tt_u32_t flag;
    tt_file_attr_t attr;
    tt_bool_t aio_enable : 1;
} tt_file_t;

typedef struct tt_fs_dir_attr_s
{
    // to describe security related, such as access control
    // to be implemented..
    tt_u32_t security;
} tt_dir_attr_t;

typedef struct
{
    tt_dir_ntv_t sys_dir;

    tt_dir_attr_t attr;
} tt_dir_t;

typedef struct tt_dir_entry_s
{
    tt_char_t name[TT_MAX_FILE_NAME_LEN + 1];
    tt_fs_type_t type;
} tt_dir_entry_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

/**
@fn void tt_fs_component_register()
register os file system wrapper
*/
extern void tt_fs_component_register();

// ========================================
// file operations
// ========================================

/**
@fn tt_result_t tt_fcreate(IN const tt_char_t *path,
                              IN tt_u32_t flag,
                              IN tt_file_attr_t *attr)
create a file

@param [in] path path of file to be created
@param [in] flag flag used to create file
@param [in] attr attributes of opened file

@return
- TT_SUCCESS if file is created
- TT_FAIL otherwise

@note
- this function only create unexisted file, it would fail if specified file
  already exist
*/
extern tt_result_t tt_fcreate(IN const tt_char_t *path,
                              IN tt_u32_t flag,
                              IN tt_file_attr_t *attr);

/**
@fn tt_result_t tt_fremove(IN const tt_char_t *path)
delete a file

@param [in] path path of file to be deleted

@return
- TT_SUCCESS if file is removed
- TT_FAIL otherwise

@note
- if file is a link, then only the link but not the targe file is removed
- file that is removed but had opened and not closed could be read and write
*/
extern tt_result_t tt_fremove(IN const tt_char_t *path);

/**
@fn tt_result_t tt_fopen(OUT tt_file_t *file,
                            IN const tt_char_t *path,
                            IN tt_u32_t flag,
                            IN tt_file_attr_t *attr)
open a file

@param [out] file ts file struct to be opened
@param [in] path path of file to be opened
@param [in] flag flag used to open file
@param [in] attr attributes of opened file

@return
- TT_SUCCESS if file is opened
- TT_FAIL otherwise

@note
- this function can not be used to open a directory
- some flags listed below are just hints that may not take effect on
  all platforms
- if @ref file is opened again but not closed, resource is leaked
- it's encouraged to init @ref file to 0, otherwise there are little
  possibility that this function would fail due to file->opened_magic
  equaling some magic number accidentally
*/
extern tt_result_t tt_fopen(OUT tt_file_t *file,
                            IN const tt_char_t *path,
                            IN tt_u32_t flag,
                            IN tt_file_attr_t *attr);
// flag
#define TT_FO_RDONLY TT_FO_RDONLY_NTV
#define TT_FO_WRONLY TT_FO_WRONLY_NTV
#define TT_FO_RDWR TT_FO_RDWR_NTV
#define TT_FO_APPEND TT_FO_APPEND_NTV // implies TT_FO_WRONLY
#define TT_FO_CREAT TT_FO_CREAT_NTV
#define TT_FO_EXCL TT_FO_EXCL_NTV
#define TT_FO_TRUNC TT_FO_TRUNC_NTV // implies TT_FO_WRONLY and TT_FO_CREAT
#define TT_FO_HINT_TEMPORARY TT_FO_TEMPORARY_NTV
#define TT_FO_HINT_SEQUENTIAL TT_FO_SEQUENTIAL_NTV
#define TT_FO_HINT_RANDOM TT_FO_RANDOM_NTV

/**
@fn tt_result_t tt_fclose(IN tt_file_t *file)
close a file

@param [in] path path of file to be deleted

@return
- TT_SUCCESS if file is closed
- TT_FAIL otherwise

@note
- behavior of passing a file opened asynchronously to this function
  is undefined
*/
extern tt_result_t tt_fclose(IN tt_file_t *file);

/**
@fn void tt_file_attr_default(IN tt_file_attr_t *attr)
get default file attribute

@param [in] attr file attribute
*/
extern void tt_file_attr_default(IN tt_file_attr_t *attr);

/**
@fn tt_result_t tt_fread(IN tt_file_t *file,
                            OUT tt_u8_t *buf,
                            IN tt_u32_t buf_len,
                            OUT tt_u32_t *read_len)
read a file

@param [in] file opened file
@param [out] buf stores data read from the file
@param [in] buf_len size of buf
@param [out] read_len stores how many bytes are read, can be NULL

@return
- TT_SUCCESS some content of file is read
- TT_END none is read and reached file end
- TT_FAIL none is read and some error occured

@note
- the bytes read may be less than buf_len
- read_len return how many bytes read only if return value is not TT_FAIL
- behavior of passing a file opened asynchronously to this function
  is undefined
*/
extern tt_result_t tt_fread(IN tt_file_t *file,
                            OUT tt_u8_t *buf,
                            IN tt_u32_t buf_len,
                            OUT tt_u32_t *read_len);

/**
@fn tt_result_t tt_fwrite(IN tt_file_t *file,
                             IN tt_u8_t *buf,
                             IN tt_u32_t buf_len,
                             OUT tt_u32_t *write_len)
write to a file

@param [in] file opened file
@param [in] buf stores data to be written to file
@param [in] buf_len size of buf
@param [out] write_len stores how many bytes are written, can be NULL

@return
- TT_SUCCESS some data are written
- TT_FAIL none is read and some error occured

@note
- the bytes written may be less than buf_len
- behavior of passing a file opened asynchronously to this function
  is undefined
*/
extern tt_result_t tt_fwrite(IN tt_file_t *file,
                             IN tt_u8_t *buf,
                             IN tt_u32_t buf_len,
                             OUT tt_u32_t *write_len);

/**
@fn tt_result_t tt_fseek(IN tt_file_t *file,
                            IN tt_u32_t whence,
                            IN tt_s64_t distance,
                            OUT tt_u32_t *position)
move file pointer

@param [in] file opened file
@param [in] whence from where to move file pointer
@param [in] distance how long the file pointer should be moved, can be negative
@param [out] position postion of file pointer after moving

@return
- TT_SUCCESS if seeking is done
- TT_FAIL otherwise

@note
- behavior of passing a file opened asynchronously to this function
  is undefined
*/
extern tt_result_t tt_fseek(IN tt_file_t *file,
                            IN tt_u32_t whence,
                            IN tt_s64_t distance,
                            OUT tt_u64_t *position);

// ========================================
// directory operations
// ========================================

/**
@fn void tt_dir_attr_default(IN tt_dir_attr_t *attr)
get default directory attribute

@param [in] attr directory attribute
*/
extern void tt_dir_attr_default(IN tt_dir_attr_t *attr);

/**
@fn tt_result_t tt_dcreate(IN const tt_char_t *path,
                              IN tt_dir_attr_t *attr)
create a directory

@param [in] path path of directory to be created
@param [in] attr attributes of created directory

@return
- TT_SUCCESS if directory is created
- TT_FAIL otherwise

@note
- this function only create unexisted directory, it would fail if specified
  directory already exist
*/
extern tt_result_t tt_dcreate(IN const tt_char_t *path, IN tt_dir_attr_t *attr);

/**
@fn tt_result_t tt_dremove(IN const tt_char_t *path,
                              IN tt_u32_t flag)
delete a directory

@param [in] path path of directory to be deleted
@param [in] flag flags affecting removing specified directory

@return
- TT_SUCCESS if directory is removed
- TT_FAIL otherwise

@note
- this function can only delete directory but not a file
- directory that is removed but had opened and not closed is supposed unable
  to be read
- if TT_DRM_RECURSIVE is specified while there are ongoing operations on
  files or sub directoies, behavior was undefined
- behavior when using TT_DRM_RECURSIVE depends on platform, and may greatly
  impact performance, so do not use this flag frequently
*/
extern tt_result_t tt_dremove(IN const tt_char_t *path, IN tt_u32_t flag);
// flag
#define TT_DRM_RECURSIVE TT_DRM_RECURSIVE_NTV

/**
@fn tt_result_t tt_dopen(OUT tt_dir_t *dir,
                            IN const tt_char_t *path,
                            IN tt_dir_attr_t *attr)
open a directory

@param [out] dir ts directory struct to be opened
@param [in] path path of directory to be opened
@param [in] attr attributes of opened directory

@return
- TT_SUCCESS if directory is opened
- TT_FAIL otherwise

@note
- this function can only open exsiting directory, it would fail if specified
  directory does not exist
*/
extern tt_result_t tt_dopen(OUT tt_dir_t *dir,
                            IN const tt_char_t *path,
                            IN tt_dir_attr_t *attr);

/**
@fn tt_result_t tt_dclose(OUT tt_dir_t *dir)
close an opened directory

@param [in] dir ts directory struct to be closed

@return
- TT_SUCCESS if directory is closed
- TT_FAIL otherwise

@note
- behavior of passing a dir opened asynchronously to this function
  is undefined
*/
extern tt_result_t tt_dclose(OUT tt_dir_t *dir);

/**
@fn tt_result_t tt_dread(IN tt_dir_t *dir,
                            IN tt_u32_t flag,
                            OUT tt_dir_entry_t *dentry)
read an opened directory

@param [in] dir ts directory struct to be read
@param [in] flag what attributes to be read
@param [out] dentry entry of the directory, may be file or a subdirectory

@return
- TT_SUCCESS if directory is read
- TT_END if all entried have been returned
- TT_FAIL otherwise

@note
- returned dentry by this function is undefined when directory has been
  removed
- returned dentry by this function is undefined when content of directory
  is changed, such like removing file or renaming sub directories
- behavior of passing a dir opened asynchronously to this function
  is undefined
*/
extern tt_result_t tt_dread(IN tt_dir_t *dir,
                            IN tt_u32_t flag,
                            OUT tt_dir_entry_t *dentry);
// flag
#define TT_DREAD_TYPE TT_DREAD_TYPE_NTV

// ========================================
// misc
// ========================================

/*
 @note
 - this function would return TT_FAIL when reading empty file
 - it reads file in blocking mode, so care the performance
 - remember to free the returned content
 */
extern tt_result_t tt_fcontent(IN const tt_char_t *path,
                               OUT tt_blob_t *content);

#endif /* __TT_FILE_SYSTEM__ */
