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

extern "C" {
#include <json/tt_json_value.h>

#include <json/tt_json_document.h>
#include <misc/tt_util.h>
}

#include <rapidjson/document.h>

using namespace rapidjson;

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

tt_result_t tt_json_value_component_init(IN struct tt_component_s *comp,
                                         IN struct tt_profile_s *profile)
{
    tt_jval_t jv;

    if (sizeof(jv.reserved) < sizeof(Value)) {
        TT_ERROR("sizeof(jv.reserved)[%d] < sizeof(Value)[%d]",
                 sizeof(jv.reserved),
                 sizeof(Value));
        return TT_FAIL;
    }

    if (sizeof(tt_ptr_t) < sizeof(Value::MemberIterator)) {
        TT_ERROR("sizeof(tt_ptr_t)[%d] < sizeof(Value::MemberIterator)[%d]",
                 sizeof(tt_ptr_t),
                 sizeof(Value::MemberIterator));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_json_value_component_exit(IN struct tt_component_s *comp)
{
}

void tt_jval_init(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    new (v) Value();
}

void tt_jval_destroy(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    v->~Value();
}

tt_bool_t tt_jval_cmp(IN tt_jval_t *a, IN tt_jval_t *b)
{
    Value *v1 = reinterpret_cast<Value *>(a);
    Value *v2 = reinterpret_cast<Value *>(b);
    return *v1 == *v2;
}

// ========================================
// null
// ========================================

tt_bool_t tt_jval_is_null(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return TT_BOOL(v->IsNull());
}

void tt_jval_set_null(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    v->SetNull();
}

// ========================================
// bool
// ========================================

void tt_jval_init_bool(IN tt_jval_t *jv, IN tt_bool_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    new (v) Value((bool)val);
}

tt_bool_t tt_jval_is_bool(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return TT_BOOL(v->IsBool());
}

tt_bool_t tt_jval_get_bool(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return TT_BOOL(v->GetBool());
}

void tt_jval_set_bool(IN tt_jval_t *jv, IN tt_bool_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    v->SetBool(val);
}

// ========================================
// number
// ========================================

tt_bool_t tt_jval_is_number(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return TT_BOOL(v->IsNumber());
}

void tt_jval_init_u32(IN tt_jval_t *jv, IN tt_u32_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    new (v) Value((unsigned int)val);
}

tt_bool_t tt_jval_is_u32(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return TT_BOOL(v->IsUint());
}

tt_u32_t tt_jval_get_u32(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return v->GetUint();
}

void tt_jval_set_u32(IN tt_jval_t *jv, IN tt_u32_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    v->SetUint(val);
}

void tt_jval_init_s32(IN tt_jval_t *jv, IN tt_s32_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    new (v) Value((int)val);
}

tt_bool_t tt_jval_is_s32(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return TT_BOOL(v->IsInt());
}

tt_s32_t tt_jval_get_s32(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return v->GetInt();
}

void tt_jval_set_s32(IN tt_jval_t *jv, IN tt_s32_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    v->SetInt(val);
}

void tt_jval_init_u64(IN tt_jval_t *jv, IN tt_u64_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    new (v) Value((uint64_t)val);
}

tt_bool_t tt_jval_is_u64(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return TT_BOOL(v->IsUint64());
}

tt_u64_t tt_jval_get_u64(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return v->GetUint64();
}

void tt_jval_set_u64(IN tt_jval_t *jv, IN tt_u64_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    v->SetUint64(val);
}

void tt_jval_init_s64(IN tt_jval_t *jv, IN tt_s64_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    new (v) Value((int64_t)val);
}

tt_bool_t tt_jval_is_s64(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return TT_BOOL(v->IsInt64());
}

tt_s64_t tt_jval_get_s64(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return v->GetInt64();
}

void tt_jval_set_s64(IN tt_jval_t *jv, IN tt_s64_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    v->SetInt64(val);
}

void tt_jval_init_double(IN tt_jval_t *jv, IN tt_double_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    new (v) Value((double)val);
}

tt_bool_t tt_jval_is_double(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return TT_BOOL(v->IsDouble());
}

tt_double_t tt_jval_get_double(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return v->GetDouble();
}

void tt_jval_set_double(IN tt_jval_t *jv, IN tt_double_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    v->SetDouble(val);
}

// ========================================
// string
// ========================================

void tt_jval_init_strn(IN tt_jval_t *jv,
                       IN const tt_char_t *val,
                       IN tt_u32_t len,
                       IN tt_jdoc_t *jd)
{
    Value *v = reinterpret_cast<Value *>(jv);
    new (v) Value(val, len);

    jv->extra = (tt_ptr_t)jd->p;
}

void tt_jval_create_strn(IN tt_jval_t *jv,
                         IN const tt_char_t *val,
                         IN tt_u32_t len,
                         IN tt_jdoc_t *jd)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jd->p;
    new (v) Value(val, len, d->GetAllocator());

    jv->extra = (tt_ptr_t)jd->p;
}

tt_bool_t tt_jval_is_str(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return TT_BOOL(v->IsString());
}

const tt_char_t *tt_jval_get_str(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return v->GetString();
}

void tt_jval_set_strn(IN tt_jval_t *jv,
                      IN const tt_char_t *val,
                      IN tt_u32_t len)
{
    Value *v = reinterpret_cast<Value *>(jv);
    v->SetString(val, len);
}

void tt_jval_copy_strn(IN tt_jval_t *jv,
                       IN const tt_char_t *val,
                       IN tt_u32_t len)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    v->SetString(val, len, d->GetAllocator());
}

// ========================================
// array
// ========================================

void tt_jval_init_array(IN tt_jval_t *jv, IN tt_jdoc_t *jd)
{
    Value *v = reinterpret_cast<Value *>(jv);
    new (v) Value(kArrayType);

    jv->extra = (tt_ptr_t)jd->p;
}

tt_bool_t tt_jval_is_array(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return v->IsArray();
}

void tt_jval_set_array(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    v->SetArray();
}

void tt_jarray_clear(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    v->Clear();
}

tt_bool_t tt_jarray_empty(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return v->Empty();
}

tt_u32_t tt_jarray_count(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return v->Size();
}

tt_u32_t tt_jarray_capacity(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return v->Capacity();
}

void tt_jarray_reserve(IN tt_jval_t *jv, IN tt_u32_t num)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    v->Reserve(v->Size() + num, d->GetAllocator());
}

void tt_jarray_push(IN tt_jval_t *jv, IN TO tt_jval_t *val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;
    Value *child = reinterpret_cast<Value *>(val);

    v->PushBack(*child, d->GetAllocator());
    tt_jval_destroy(val);
}

void tt_jarray_pop(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    v->PopBack();
}

tt_jval_t *tt_jarray_get(IN tt_jval_t *jv, IN tt_u32_t idx)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Value &child = v->operator[](idx);
    return reinterpret_cast<tt_jval_t *>(&child);
}

void tt_jarray_iter(IN tt_jval_t *jv, OUT tt_jarray_iter_t *iter)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Value::ValueIterator i = v->Begin();

    iter->v = v;
    iter->i = i;
}

tt_jval_t *tt_jarray_iter_next(IN OUT tt_jarray_iter_t *iter)
{
    Value *v = reinterpret_cast<Value *>(iter->v);
    Value::ValueIterator i = reinterpret_cast<Value::ValueIterator>(iter->i);

    if (i != v->End()) {
        tt_jval_t *jv = reinterpret_cast<tt_jval_t *>(i);
        iter->i = ++i;
        return jv;
    } else {
        return nullptr;
    }
}

void tt_jarray_remove(IN tt_jval_t *jv, IN OUT tt_jarray_iter_t *iter)
{
    Value *v = reinterpret_cast<Value *>(iter->v);
    Value::ValueIterator i = reinterpret_cast<Value::ValueIterator>(iter->i);

    iter->i = v->Erase(i);
}

void tt_jarray_push_null(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    v->PushBack(Value(kNullType).Move(), d->GetAllocator());
}

void tt_jarray_push_bool(IN tt_jval_t *jv, IN tt_bool_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    v->PushBack(Value((bool)val).Move(), d->GetAllocator());
}

void tt_jarray_push_u32(IN tt_jval_t *jv, IN tt_u32_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    v->PushBack(Value((unsigned int)val).Move(), d->GetAllocator());
}

void tt_jarray_push_s32(IN tt_jval_t *jv, IN tt_s32_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    v->PushBack(Value((int)val).Move(), d->GetAllocator());
}

void tt_jarray_push_u64(IN tt_jval_t *jv, IN tt_u64_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    v->PushBack(Value((uint64_t)val).Move(), d->GetAllocator());
}

void tt_jarray_push_s64(IN tt_jval_t *jv, IN tt_s64_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    v->PushBack(Value((int64_t)val).Move(), d->GetAllocator());
}

void tt_jarray_push_double(IN tt_jval_t *jv, IN tt_double_t val)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    v->PushBack(Value((double)val).Move(), d->GetAllocator());
}

void tt_jarray_push_strn(IN tt_jval_t *jv,
                         IN const tt_char_t *val,
                         IN tt_u32_t len)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    v->PushBack(Value(val, len).Move(), d->GetAllocator());
}

void tt_jarray_copy_strn(IN tt_jval_t *jv,
                         IN const tt_char_t *val,
                         IN tt_u32_t len)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    v->PushBack(Value(val, len, d->GetAllocator()).Move(), d->GetAllocator());
}

// ========================================
// object
// ========================================

void tt_jval_init_obj(IN tt_jval_t *jv, IN tt_jdoc_t *jd)
{
    Value *v = reinterpret_cast<Value *>(jv);
    new (v) Value(kObjectType);

    jv->extra = (tt_ptr_t)jd->p;
}

tt_bool_t tt_jval_is_obj(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return TT_BOOL(v->IsObject());
}

void tt_jval_set_obj(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    v->SetObject();
}

void tt_jobj_clear(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    v->RemoveAllMembers();
}

tt_bool_t tt_jobj_empty(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return TT_BOOL(v->MemberCount() == 0);
}

tt_u32_t tt_jobj_member_count(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return v->MemberCount();
}

tt_u32_t tt_jobj_capacity(IN tt_jval_t *jv)
{
    Value *v = reinterpret_cast<Value *>(jv);
    return v->MemberCapacity();
}

void tt_jobj_reserve(IN tt_jval_t *jv, IN tt_u32_t num)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;
    v->MemberReserve(v->MemberCapacity() + num, d->GetAllocator());
}

tt_jval_t *tt_jobj_find(IN tt_jval_t *jv, IN const tt_char_t *name)
{
    Value *o = reinterpret_cast<Value *>(jv);
    Value::MemberIterator i = o->FindMember(name);

    if (i != o->MemberEnd()) {
        return reinterpret_cast<tt_jval_t *>(&i->value);
    } else {
        return nullptr;
    }
}

tt_bool_t tt_jobj_contain(IN tt_jval_t *jv, IN const tt_char_t *name)
{
    Value *o = reinterpret_cast<Value *>(jv);
    return TT_BOOL(o->HasMember(name));
}

void tt_jobj_add(IN tt_jval_t *jv,
                 IN const tt_char_t *name,
                 IN TO tt_jval_t *val)
{
    Value *o = reinterpret_cast<Value *>(jv);
    Value *v = reinterpret_cast<Value *>(val);
    Document *d = (Document *)jv->extra;

    o->AddMember(Value(name, tt_strlen(name)).Move(), *v, d->GetAllocator());
    tt_jval_destroy(val);
}

void tt_jobj_add_nv(IN tt_jval_t *jv,
                    IN TO tt_jval_t *name,
                    IN TO tt_jval_t *val)
{
    Value *o = reinterpret_cast<Value *>(jv);
    Value *n = reinterpret_cast<Value *>(name);
    Value *v = reinterpret_cast<Value *>(val);
    Document *d = (Document *)jv->extra;

    o->AddMember(*n, *v, d->GetAllocator());
    tt_jval_destroy(name);
    tt_jval_destroy(val);
}

void tt_jobj_add_null(IN tt_jval_t *jv, IN const tt_char_t *name)
{
    Value *o = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    o->AddMember(Value(name, tt_strlen(name)).Move(),
                 Value(kNullType).Move(),
                 d->GetAllocator());
}

void tt_jobj_add_bool(IN tt_jval_t *jv,
                      IN const tt_char_t *name,
                      IN tt_bool_t val)
{
    Value *o = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    o->AddMember(Value(name, tt_strlen(name)).Move(),
                 Value((bool)val).Move(),
                 d->GetAllocator());
}

void tt_jobj_add_u32(IN tt_jval_t *jv,
                     IN const tt_char_t *name,
                     IN tt_u32_t val)
{
    Value *o = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    o->AddMember(Value(name, tt_strlen(name)).Move(),
                 Value((unsigned int)val).Move(),
                 d->GetAllocator());
}

void tt_jobj_add_s32(IN tt_jval_t *jv,
                     IN const tt_char_t *name,
                     IN tt_s32_t val)
{
    Value *o = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    o->AddMember(Value(name, tt_strlen(name)).Move(),
                 Value((int)val).Move(),
                 d->GetAllocator());
}

void tt_jobj_add_u64(IN tt_jval_t *jv,
                     IN const tt_char_t *name,
                     IN tt_u64_t val)
{
    Value *o = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    o->AddMember(Value(name, tt_strlen(name)).Move(),
                 Value((uint64_t)val).Move(),
                 d->GetAllocator());
}

void tt_jobj_add_s64(IN tt_jval_t *jv,
                     IN const tt_char_t *name,
                     IN tt_s64_t val)
{
    Value *o = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    o->AddMember(Value(name, tt_strlen(name)).Move(),
                 Value((int64_t)val).Move(),
                 d->GetAllocator());
}

void tt_jobj_add_double(IN tt_jval_t *jv,
                        IN const tt_char_t *name,
                        IN tt_double_t val)
{
    Value *o = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    o->AddMember(Value(name, tt_strlen(name)).Move(),
                 Value((double)val).Move(),
                 d->GetAllocator());
}

void tt_jobj_add_strn(IN tt_jval_t *jv,
                      IN const tt_char_t *name,
                      IN const tt_char_t *val,
                      IN tt_u32_t len)
{
    Value *o = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    o->AddMember(Value(name, tt_strlen(name)).Move(),
                 Value(val, len).Move(),
                 d->GetAllocator());
}

void tt_jobj_copy_strn(IN tt_jval_t *jv,
                       IN const tt_char_t *name,
                       IN const tt_char_t *val,
                       IN tt_u32_t len)
{
    Value *o = reinterpret_cast<Value *>(jv);
    Document *d = (Document *)jv->extra;

    o->AddMember(Value(name, tt_strlen(name)).Move(),
                 Value(name, tt_strlen(name), d->GetAllocator()).Move(),
                 d->GetAllocator());
}

void tt_jobj_remove(IN tt_jval_t *jv, IN const tt_char_t *name)
{
    Value *o = reinterpret_cast<Value *>(jv);
    o->RemoveMember(name);
}

void tt_jobj_iter(IN tt_jval_t *jv, OUT tt_jobj_iter_t *iter)
{
    Value *v = reinterpret_cast<Value *>(jv);
    Value::MemberIterator i = v->MemberBegin();

    iter->v = v;
    // Value::MemberIterator has only a pointer member
    iter->i = *reinterpret_cast<tt_ptr_t *>(&i);
}

tt_jval_t *tt_jobj_iter_next(IN OUT tt_jobj_iter_t *iter)
{
    Value *v = reinterpret_cast<Value *>(iter->v);
    Value::MemberIterator i =
        *reinterpret_cast<Value::MemberIterator *>(&iter->i);

    if (i != v->MemberEnd()) {
        tt_jval_t *jv = reinterpret_cast<tt_jval_t *>(&i->value);
        ++i;
        iter->i = *reinterpret_cast<tt_ptr_t *>(&i);
        return jv;
    } else {
        return nullptr;
    }
}
