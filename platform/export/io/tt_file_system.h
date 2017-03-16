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

#include <tt_file_system_native.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct tt_file_attr_s
{
    tt_u32_t reserved;
} tt_file_attr_t;

typedef struct tt_fs_file_s
{
    tt_file_ntv_t sys_file;
} tt_file_t;

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
 @fn void tt_file_attr_default(IN tt_file_attr_t *attr)
 get default file attribute

 @param [in] attr file attribute
 */
extern void tt_file_attr_default(IN tt_file_attr_t *attr);

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
                              IN OPT tt_file_attr_t *attr);

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
                            IN OPT tt_file_attr_t *attr);
#define TT_FO_READ (1 << 0)
#define TT_FO_WRITE (1 << 1)
#define TT_FO_APPEND (1 << 3)
#define TT_FO_CREAT (1 << 4)
#define TT_FO_EXCL (1 << 5)
#define TT_FO_TRUNC (1 << 6)

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
#define TT_FSEEK_BEGIN (0)
#define TT_FSEEK_CUR (1)
#define TT_FSEEK_END (2)

#endif /* __TT_FILE_SYSTEM_FB__ */
