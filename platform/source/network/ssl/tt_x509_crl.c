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

#include <network/ssl/tt_x509_crl.h>

#include <algorithm/tt_buffer.h>
#include <algorithm/tt_buffer_format.h>
#include <io/tt_file_system.h>
#include <misc/tt_assert.h>
#include <network/ssl/tt_ssl.h>

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

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

void tt_x509crl_init(IN tt_x509crl_t *x)
{
    TT_ASSERT(x != NULL);

    mbedtls_x509_crl_init(&x->crl);
}

tt_result_t tt_x509crl_add(IN tt_x509crl_t *x, IN tt_u8_t *buf, IN tt_u32_t len)
{
    TT_ASSERT(x != NULL);
    TT_ASSERT(buf != NULL);

    if (mbedtls_x509_crl_parse(&x->crl, buf, len) != 0) {
        TT_ERROR("fail to parse x509 cert");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_x509crl_add_file(IN tt_x509crl_t *x, IN const tt_char_t *path)
{
    tt_buf_t buf;
    int n;

    TT_ASSERT(x != NULL);
    TT_ASSERT(path != NULL);

    tt_buf_init(&buf, NULL);
    if (!TT_OK(tt_fcontent_buf(path, &buf)) || !TT_OK(tt_buf_put_u8(&buf, 0))) {
        tt_buf_destroy(&buf);
        return TT_FAIL;
    }

    n = mbedtls_x509_crl_parse(&x->crl, TT_BUF_RPOS(&buf), TT_BUF_RLEN(&buf));
    tt_buf_destroy(&buf);
    if (n != 0) {
        TT_ERROR("fail to parse x509 cert");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_x509crl_destroy(IN tt_x509crl_t *x)
{
    TT_ASSERT(x != NULL);

    mbedtls_x509_crl_free(&x->crl);
}

tt_u32_t tt_x509crl_dump(IN tt_x509crl_t *x, IN tt_char_t *buf, IN tt_u32_t len)
{
    TT_ASSERT(buf != NULL);

    return mbedtls_x509_crl_info(buf, len, "", &x->crl);
}
