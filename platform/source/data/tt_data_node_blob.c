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

#include <data/tt_data_node_blob.h>

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

static void __db_destroy(IN tt_dtnode_t *dtn);

static tt_result_t __db_read(IN tt_dtnode_t *dtn,
                             OUT tt_u8_t **p,
                             OUT tt_u32_t *len);

static void __db_read_update(IN tt_dtnode_t *dtn, IN tt_u32_t len);

static tt_result_t __db_write(IN tt_dtnode_t *dtn,
                              IN tt_u8_t *p,
                              IN tt_u32_t len,
                              OUT OPT tt_u32_t *write_len);

static tt_dtnode_itf_t tt_s_dtn_blob_itf = {
    __db_destroy, __db_read, __db_read_update, __db_write,
};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_dtnode_t *tt_dtnode_blob_create(IN void *p,
                                   IN tt_u32_t len,
                                   IN tt_bool_t free)
{
    tt_dtnode_t *dtn;
    tt_dtnode_blob_t *db;

    TT_ASSERT(p != NULL);

    dtn = tt_dtnode_create(sizeof(tt_dtnode_blob_t), &tt_s_dtn_blob_itf);
    if (dtn == NULL) {
        return NULL;
    }

    db = TT_DTNODE_CAST(dtn, tt_dtnode_blob_t);

    db->blob.addr = p;
    db->blob.len = len;
    db->pos = 0;
    db->free = free;

    return dtn;
}

void __db_destroy(IN tt_dtnode_t *dtn)
{
    tt_dtnode_blob_t *db = TT_DTNODE_CAST(dtn, tt_dtnode_blob_t);

    if (db->free) {
        tt_blob_destroy(&db->blob);
    }
}

tt_result_t __db_read(IN tt_dtnode_t *dtn, OUT tt_u8_t **p, OUT tt_u32_t *len)
{
    tt_dtnode_blob_t *db = TT_DTNODE_CAST(dtn, tt_dtnode_blob_t);

    if (db->pos < db->blob.len) {
        *p = TT_PTR_INC(tt_u8_t, db->blob.addr, db->pos);
        *len = db->blob.len - db->pos;
        return TT_SUCCESS;
    } else {
        TT_ASSERT(db->pos == db->blob.len);
        *p = NULL;
        *len = 0;
        return TT_E_END;
    }
}

void __db_read_update(IN tt_dtnode_t *dtn, IN tt_u32_t len)
{
    tt_dtnode_blob_t *db = TT_DTNODE_CAST(dtn, tt_dtnode_blob_t);

    TT_ASSERT((db->pos + len) <= db->blob.len);
    db->pos += len;
}

tt_result_t __db_write(IN tt_dtnode_t *dtn,
                       IN tt_u8_t *p,
                       IN tt_u32_t len,
                       OUT OPT tt_u32_t *sent)
{
    tt_dtnode_blob_t *db = TT_DTNODE_CAST(dtn, tt_dtnode_blob_t);
    tt_u32_t space;

    TT_ASSERT(db->pos <= db->blob.len);
    space = db->blob.len - db->pos;

    if (space > len) {
        tt_memcpy(db->blob.addr, p, len);
        db->pos += len;
        TT_SAFE_ASSIGN(sent, len);
        return TT_SUCCESS;
    } else if (space == 0) {
        TT_SAFE_ASSIGN(sent, 0);
        return TT_E_END;
    } else {
        tt_memcpy(db->blob.addr, p, space);
        db->pos += space;
        TT_SAFE_ASSIGN(sent, space);
        return TT_SUCCESS;
    }
}
