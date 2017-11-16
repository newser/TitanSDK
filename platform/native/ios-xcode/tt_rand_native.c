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

#include <tt_rand_native.h>

#include <tt_sys_error.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

//#define __RAND_DEV "/dev/random"
#define __RAND_DEV "/dev/urandom"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static int __rand_fd = -1;

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_rng_component_init_ntv()
{
    __rand_fd = open(__RAND_DEV, O_RDONLY);
    if (__rand_fd < 0) {
        TT_ERROR_NTV("fail to open %s", __RAND_DEV);
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_rng_ntv(IN tt_u8_t *data, IN tt_u32_t data_len)
{
    tt_u32_t n = 0;
    tt_u32_t ret;

rag:
    ret = (tt_u32_t)read(__rand_fd, data + n, data_len - n);
    if (ret > 0) {
        n += ret;
        if (n < data_len) {
            goto rag;
        } else {
            return TT_SUCCESS;
        }
    } else if (errno == EINTR) {
        goto rag;
    } else {
        TT_ERROR_NTV("fail to read random data");
        return TT_FAIL;
    }
}
