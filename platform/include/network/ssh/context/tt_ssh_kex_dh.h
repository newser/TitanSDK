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
@file tt_ssh_kex_dh.h
@brief ts ssh key exchange, dh
*/

#ifndef __TT_SSH_KEX_DH__
#define __TT_SSH_KEX_DH__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <algorithm/tt_buffer.h>
#include <network/ssh/tt_ssh_def.h>

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

typedef struct
{
    tt_ssh_kex_alg_t alg;
    // tt_dh_t dh;

    // exchange value sent by the client
    tt_buf_t e;
    // exchange value sent by the server
    tt_buf_t f;
} tt_sshkexdh_t;

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

tt_export tt_result_t tt_sshkexdh_create(IN tt_sshkexdh_t *kexdh,
                                         IN tt_ssh_kex_alg_t alg);

tt_export void tt_sshkexdh_destroy(IN tt_sshkexdh_t *kexdh);

tt_export tt_result_t tt_sshkexdh_compute(IN tt_sshkexdh_t *kexdh,
                                          IN tt_u8_t *peerpub,
                                          IN tt_u32_t peerpub_len);

tt_export tt_result_t tt_sshkexdh_set_e(IN tt_sshkexdh_t *kexdh, IN tt_u8_t *e,
                                        IN tt_u32_t e_len, IN tt_bool_t format);
// load dh pubkey to e, should only be used by ssh client
tt_export tt_result_t tt_sshkexdh_load_e(IN tt_sshkexdh_t *kexdh);

tt_export tt_result_t tt_sshkexdh_set_f(IN tt_sshkexdh_t *kexdh, IN tt_u8_t *f,
                                        IN tt_u32_t f_len, IN tt_bool_t format);
// load dh pubkey to f, should only be used by ssh server
tt_export tt_result_t tt_sshkexdh_load_f(IN tt_sshkexdh_t *kexdh);

tt_export tt_result_t tt_sshkexdh_get_k(IN tt_sshkexdh_t *kexdh,
                                        OUT tt_buf_t *k);

#endif /* __TT_SSH_KEX_DH__ */
