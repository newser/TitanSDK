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
@file tt_ssh_message.h
@brief ssh message
*/

#ifndef __TT_SSH_MESASGE__
#define __TT_SSH_MESASGE__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <network/ssh/tt_ssh_def.h>
#include <os/tt_atomic.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TT_SSHMSG_CAST(msg, type) TT_PTR_INC(type, msg, sizeof(tt_sshmsg_t))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

struct tt_sshmsg_s;

typedef enum {
    TT_SSH_RENDER_MODE_RAW,
    TT_SSH_RENDER_MODE_BINARY,

    TT_SSH_RENDER_MODE_NUM
} tt_ssh_render_mode_t;
#define TT_SSH_RENDER_MODE_VALID(m) ((m) < TT_SSH_RENDER_MODE_NUM)

typedef tt_result_t (*tt_sshmsg_create_t)(IN struct tt_sshmsg_s *msg);

typedef void (*tt_sshmsg_destroy_t)(IN struct tt_sshmsg_s *msg);

typedef void (*tt_sshmsg_dump_t)(IN struct tt_sshmsg_s *msg);

// - return TT_SUCCESS means the msg could be rendered
// - if returned TT_SUCCESS but len is set to 0, the rendering buf
//   won't reserve buf space.
typedef tt_result_t (*tt_sshmsg_render_prepare_t)(
    IN struct tt_sshmsg_s *msg,
    OUT tt_u32_t *len,
    OUT tt_ssh_render_mode_t *mode);

typedef tt_result_t (*tt_sshmsg_render_t)(IN struct tt_sshmsg_s *msg,
                                          IN OUT tt_buf_t *buf);

// - if parse() return success, it must guarantee data is updated, if it
//   returns fail, no need to retore data
typedef tt_result_t (*tt_sshmsg_parse_t)(IN struct tt_sshmsg_s *msg,
                                         IN tt_buf_t *data);

typedef struct
{
    tt_sshmsg_create_t create;
    tt_sshmsg_destroy_t destroy;
    tt_sshmsg_dump_t dump;

    tt_sshmsg_render_prepare_t render_prepare;
    tt_sshmsg_render_t render;

    tt_sshmsg_parse_t parse;
} tt_sshmsg_itf_t;

typedef struct tt_sshmsg_s
{
    tt_u32_t msg_id;
    tt_u32_t msg_private;
    tt_atomic_s32_t ref;

    tt_buf_t buf;
    tt_sshmsg_itf_t *itf;

    // render flags
    tt_bool_t rendered : 1;
    tt_u8_t pad_block : 2;

    // parse flags
    tt_bool_t save : 1;
} tt_sshmsg_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

// msg_len is extra length
extern tt_sshmsg_t *tt_sshmsg_create(IN tt_u32_t msg_id,
                                     IN tt_u32_t msg_len,
                                     IN tt_sshmsg_itf_t *itf);

extern void __sshmsg_destroy(IN tt_sshmsg_t *msg);

#define tt_sshmsg_ref(p) TT_REF_ADD(tt_sshmsg_t, p, ref)
#define tt_sshmsg_release(p)                                                   \
    TT_REF_RELEASE(tt_sshmsg_t, p, ref, __sshmsg_destroy)

extern void tt_sshmsg_dump(IN tt_sshmsg_t *msg);

extern tt_result_t tt_sshmsg_render(IN tt_sshmsg_t *msg,
                                    IN tt_u32_t block_len,
                                    IN OPT tt_buf_t *buf);

extern tt_result_t tt_sshmsg_parse(IN tt_buf_t *msg_buf,
                                   OUT tt_sshmsg_t **p_msg);

extern tt_result_t tt_sshmsg_parse_verxchg(IN tt_buf_t *msg_buf,
                                           OUT tt_sshmsg_t **p_msg);

extern tt_result_t tt_sshmsg_peek_payload(IN tt_buf_t *msg_buf,
                                          OUT tt_u8_t **payload,
                                          OUT tt_u32_t *payload_len);

extern const tt_char_t *tt_sshmsg_name(IN tt_u32_t msg_id);

#endif /* __TT_SSH_MESASGE__ */
