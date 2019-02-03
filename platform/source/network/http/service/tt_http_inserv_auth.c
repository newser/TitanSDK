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

#include <network/http/service/tt_http_inserv_auth.h>

#include <network/http/tt_http_parser.h>
#include <network/http/tt_http_render.h>

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define __DEFAULT_DIGEST_PWD "titansdk"

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

typedef struct
{
    tt_result_t (*get_pwd)(IN tt_char_t *username,
                           IN tt_u32_t username_len,
                           IN void *param,
                           OUT tt_char_t **pwd,
                           OUT tt_u32_t *pwd_len);
    union
    {
        void *get_pwd_param;
        const tt_char_t *pwd;
    };

#if 1 // test
    const tt_char_t *fixed_nonce;
#endif

    tt_http_auth_t auth;
    tt_bool_t new_nonce : 1;
} tt_http_inserv_auth_t;

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static void __destroy_s_auth(IN tt_http_inserv_t *s);

static tt_result_t __create_ctx(IN tt_http_inserv_t *s, IN OPT void *ctx);

static void __destroy_ctx(IN tt_http_inserv_t *s, IN void *ctx);

// __clear_ctx() is not nessary, the service can handle it
static tt_http_inserv_itf_t s_auth_itf = {__destroy_s_auth,
                                          NULL,
                                          __create_ctx,
                                          __destroy_ctx,
                                          NULL};

static tt_http_inserv_action_t __s_auth_on_hdr(IN tt_http_inserv_t *s,
                                               IN void *ctx,
                                               IN tt_http_parser_t *req,
                                               OUT tt_http_resp_render_t *resp);

static tt_http_inserv_cb_t s_auth_cb = {
    NULL, __s_auth_on_hdr, NULL, NULL, NULL, NULL,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static void __new_nonce(IN tt_http_inserv_auth_t *sa,
                        IN tt_http_auth_ctx_t *ac);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_http_inserv_t *tt_http_inserv_auth_create(
    IN OPT tt_http_inserv_auth_attr_t *attr)
{
    tt_http_inserv_auth_attr_t __attr;
    tt_http_inserv_t *s;
    tt_http_inserv_auth_t *sa;
    tt_http_auth_t *ha;

    if (attr == NULL) {
        tt_http_inserv_auth_attr_default(&__attr);
        attr = &__attr;
    }

    if ((attr->get_pwd == NULL) && (attr->get_pwd_param == NULL)) {
        TT_ERROR("no password specified");
        return NULL;
    }

    s = tt_http_inserv_create(TT_HTTP_INSERV_AUTH,
                              sizeof(tt_http_inserv_auth_t),
                              &s_auth_itf,
                              &s_auth_cb);
    if (s == NULL) {
        return NULL;
    }

    sa = TT_HTTP_INSERV_CAST(s, tt_http_inserv_auth_t);

    sa->get_pwd = attr->get_pwd;
    sa->get_pwd_param = attr->get_pwd_param;

    sa->fixed_nonce = attr->fixed_nonce;

    ha = &sa->auth;
    tt_http_auth_init(ha);
    // if realm is cstr, no need to own it
    if ((attr->realm_len > 0) &&
        !TT_OK(tt_blobex_set(&ha->realm,
                             (tt_u8_t *)attr->realm,
                             attr->realm_len,
                             !attr->realm_cstr))) {
        goto fail;
    }
    if ((attr->domain_len > 0) &&
        !TT_OK(tt_blobex_set(&ha->domain,
                             (tt_u8_t *)attr->domain,
                             attr->domain_len,
                             !attr->domain_cstr))) {
        goto fail;
    }
    ha->qop_mask = attr->qop_mask;
    ha->scheme = TT_HTTP_AUTH_DIGEST;
    ha->alg = attr->alg;

    sa->new_nonce = attr->new_nonce;

    return s;

fail:
    tt_http_inserv_release(s);
    return NULL;
}

void tt_http_inserv_auth_attr_default(IN tt_http_inserv_auth_attr_t *attr)
{
    attr->get_pwd = NULL;
    attr->pwd = __DEFAULT_DIGEST_PWD;

    attr->fixed_nonce = NULL;

    attr->realm = NULL;
    attr->realm_len = 0;
    attr->realm_cstr = TT_FALSE;

    attr->domain = NULL;
    attr->domain_len = 0;
    attr->domain_cstr = TT_FALSE;

    attr->qop_mask = TT_HTTP_QOP_AUTH;
    attr->alg = TT_HTTP_AUTH_MD5;
    attr->new_nonce = TT_FALSE;
}

void __destroy_s_auth(IN tt_http_inserv_t *s)
{
    tt_http_inserv_auth_t *sa = TT_HTTP_INSERV_CAST(s, tt_http_inserv_auth_t);

    tt_http_auth_destroy(&sa->auth);
}

tt_result_t __create_ctx(IN tt_http_inserv_t *s, IN OPT void *ctx)
{
    tt_http_inserv_auth_ctx_t *c = (tt_http_inserv_auth_ctx_t *)ctx;

    tt_http_auth_ctx_init(&c->auth_ctx);

    return TT_SUCCESS;
}

void __destroy_ctx(IN tt_http_inserv_t *s, IN void *ctx)
{
    tt_http_inserv_auth_ctx_t *c = (tt_http_inserv_auth_ctx_t *)ctx;

    tt_http_auth_ctx_destroy(&c->auth_ctx);
}

tt_http_inserv_action_t __s_auth_on_hdr(IN tt_http_inserv_t *s,
                                        IN void *ctx,
                                        IN tt_http_parser_t *req,
                                        OUT tt_http_resp_render_t *resp)
{
    tt_http_inserv_auth_t *sa = TT_HTTP_INSERV_CAST(s, tt_http_inserv_auth_t);
    tt_http_inserv_auth_ctx_t *c = (tt_http_inserv_auth_ctx_t *)ctx;
    tt_http_auth_ctx_t *ac = &c->auth_ctx;

    tt_http_hdr_t *h;
    tt_http_auth_t *local_auth, *remote_auth;
    tt_char_t *pwd;
    tt_u32_t pwd_len, qop, rlen;
    tt_http_method_t mtd;
    tt_char_t response[160]; // enough for sha512

    // check if request has an Authorization header
    h = tt_http_parser_get_auth(req);
    if (h == NULL) {
        goto auth_fail;
    }

    // do verification
    local_auth = &sa->auth;
    remote_auth = tt_http_hdr_auth_get(h);
    TT_ASSERT(remote_auth != NULL);

    // must have same realm, nonce
    if (tt_blobex_cmp(&remote_auth->realm, &local_auth->realm) != 0) {
        goto auth_fail;
    }

    // note nonce is saved in ac but not local_auth
    if ((ac->nonce_len == 0) ||
        (tt_blobex_memcmp(&remote_auth->nonce, ac->nonce, ac->nonce_len) !=
         0)) {
        goto auth_fail;
    }

    if (sa->get_pwd == NULL) {
        TT_ASSERT(sa->pwd != NULL);
        pwd = (tt_char_t *)sa->pwd;
        pwd_len = tt_strlen(sa->pwd);
    } else if (!TT_OK(sa->get_pwd(tt_blobex_addr(&remote_auth->username),
                                  tt_blobex_len(&remote_auth->username),
                                  sa->get_pwd_param,
                                  &pwd,
                                  &pwd_len))) {
        // no such user
        goto auth_fail;
    }

    if (remote_auth->qop_mask & TT_HTTP_QOP_AUTH) {
        qop = TT_HTTP_QOP_AUTH;
    } else if (remote_auth->qop_mask & TT_HTTP_QOP_AUTH_INT) {
        tt_http_resp_render_set_status(resp, TT_HTTP_STATUS_NOT_IMPLEMENTED);
        return TT_HTTP_INSERV_ACT_DISCARD;

        qop = TT_HTTP_QOP_AUTH_INT;
    } else {
        qop = 0;
    }

    /*
     rfc7231:
     The method token is case-sensitive.
     standardized methods are defined in all-uppercase US-ASCII letters.
     */
    mtd = tt_http_parser_get_method(req);

    if (!TT_OK(tt_http_auth_ctx_calc(ac,
                                     remote_auth,
                                     pwd,
                                     pwd_len,
                                     qop,
                                     (tt_char_t *)tt_g_http_method[mtd],
                                     tt_g_http_method_len[mtd],
                                     NULL,
                                     0,
                                     response))) {
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return TT_HTTP_INSERV_ACT_DISCARD;
    }

    if (tt_blobex_memcmp(&remote_auth->response,
                         response,
                         tt_http_auth_ctx_digest_len(ac) << 1) != 0) {
#if 1
        tt_hex_dump(tt_blobex_addr(&remote_auth->response),
                    tt_blobex_len(&remote_auth->response),
                    16);
        tt_hex_dump((tt_u8_t *)response,
                    tt_http_auth_ctx_digest_len(ac) << 1,
                    16);
#endif
        goto auth_fail;
    }

    if (sa->new_nonce) {
        __new_nonce(sa, ac);
        // todo: authorization-info
    }

    return TT_HTTP_INSERV_ACT_PASS;

auth_fail:
    if (TT_OK(tt_http_render_add_www_auth(&resp->render, &sa->auth, TT_TRUE))) {
        tt_http_auth_t *ha;

        ha = tt_http_render_get_www_auth(&resp->render);
        TT_ASSERT(ha != NULL);
        __new_nonce(sa, ac);
        tt_blobex_set(&ha->nonce,
                      (tt_u8_t *)ac->nonce,
                      ac->nonce_len,
                      TT_FALSE);

        tt_http_resp_render_set_status(resp, TT_HTTP_STATUS_UNAUTHORIZED);
    } else {
        tt_http_resp_render_set_status(resp,
                                       TT_HTTP_STATUS_INTERNAL_SERVER_ERROR);
    }
    return TT_HTTP_INSERV_ACT_DISCARD;
}

void __new_nonce(IN tt_http_inserv_auth_t *sa, IN tt_http_auth_ctx_t *ac)
{
#if 1
    if (sa->fixed_nonce != NULL) {
        ac->nonce_len = tt_strlen(sa->fixed_nonce);
        TT_ASSERT(ac->nonce_len < (sizeof(ac->nonce) - 1));
        tt_memcpy(ac->nonce, sa->fixed_nonce, ac->nonce_len);
        ac->nonce[ac->nonce_len] = 0;
        return;
    }
#endif

    tt_http_auth_ctx_new_nonce(ac);
}
