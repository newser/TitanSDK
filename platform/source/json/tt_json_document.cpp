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
#include <json/tt_json_document.h>

#include <algorithm/tt_buffer_format.h>
#include <io/tt_file_system.h>
#include <json/tt_json_value.h>
#include <misc/tt_assert.h>

#include <tt_cstd_api.h>
}

#include <rapidjson/document.h>
#include <rapidjson/encodedstream.h>
#include <rapidjson/encodings.h>
#include <rapidjson/writer.h>

using namespace rapidjson;

////////////////////////////////////////////////////////////
// internal macro
////////////////////////////////////////////////////////////

#define PARSE_FLAG 0

////////////////////////////////////////////////////////////
// internal type
////////////////////////////////////////////////////////////

class jw_mem
{
public:
    typedef char Ch;

    jw_mem(tt_buf_t *buf): buf_(buf), err_(TT_FALSE) {}

    Ch Peek() const
    {
        TT_ASSERT(0);
        return '\0';
    }

    Ch Take()
    {
        TT_ASSERT(0);
        return '\0';
    }

    size_t Tell() const
    {
        TT_ASSERT(0);
        return 0;
    }

    Ch *PutBegin()
    {
        TT_ASSERT(0);
        return 0;
    }

    void Put(Ch c)
    {
        if (!err_ && !TT_OK(tt_buf_put_u8(buf_, c))) { err_ = TT_TRUE; }
    }

    void Flush() {}

    size_t PutEnd(Ch *)
    {
        TT_ASSERT(0);
        return 0;
    }

    tt_bool_t error() { return err_; }

private:
    jw_mem(const jw_mem &);
    jw_mem &operator=(const jw_mem &);

    tt_buf_t *buf_;
    tt_bool_t err_ : 1;
};

class jw_file
{
public:
    typedef char Ch;

    jw_file(IN const tt_char_t *path):
        path_(path), err_(TT_FALSE), opened_(TT_FALSE)
    {
    }

    ~jw_file()
    {
        if (opened_) { tt_fclose(&f_); }
    }

    Ch Peek() const
    {
        TT_ASSERT(0);
        return '\0';
    }

    Ch Take()
    {
        TT_ASSERT(0);
        return '\0';
    }

    size_t Tell() const
    {
        TT_ASSERT(0);
        return 0;
    }

    Ch *PutBegin()
    {
        TT_ASSERT(0);
        return 0;
    }

    void Put(Ch c)
    {
        if (!err_) {
            if (!opened_) {
                if (!TT_OK(tt_fopen(&f_, path_,
                                    TT_FO_CREAT | TT_FO_TRUNC | TT_FO_WRITE,
                                    nullptr))) {
                    err_ = TT_TRUE;
                    return;
                }
                opened_ = TT_TRUE;
            }

            if (!TT_OK(tt_fwrite_all(&f_, (tt_u8_t *)&c, 1))) {
                err_ = TT_TRUE;
                return;
            }
        }
    }

    void Flush()
    {
        if (!err_ && opened_) { tt_fdatasync(&f_); }
    }

    size_t PutEnd(Ch *)
    {
        TT_ASSERT(0);
        return 0;
    }

    tt_bool_t error() { return err_; }

private:
    const tt_char_t *path_;
    tt_file_t f_;
    tt_bool_t err_ : 1;
    tt_bool_t opened_ : 1;
};

////////////////////////////////////////////////////////////
// extern declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variant
////////////////////////////////////////////////////////////

static UTFType tt_s_utftype_map[TT_JDOC_ENCODING_NUM] = {kUTF8,    kUTF8,
                                                         kUTF16LE, kUTF16BE,
                                                         kUTF32LE, kUTF32BE};

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

static const tt_char_t *__pec_str(IN ParseErrorCode pec);

////////////////////////////////////////////////////////////
// interface implementation
////////////////////////////////////////////////////////////

tt_result_t tt_jdoc_create(IN tt_jdoc_t *jd)
{
    TT_ASSERT(jd != nullptr);

    jd->p = new Document();
    if (jd->p == nullptr) {
        TT_ERROR("fail to new json doc");
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

void tt_jdoc_destroy(IN tt_jdoc_t *jd)
{
    TT_ASSERT(jd != nullptr);

    delete (Document *)jd->p;
}

void tt_jdoc_clear(IN tt_jdoc_t *jd)
{
    Document *p = static_cast<Document *>(jd->p);
    p->SetNull();
}

tt_jval_t *tt_jdoc_get_root(IN tt_jdoc_t *jd)
{
    Document *p = static_cast<Document *>(jd->p);
    return reinterpret_cast<tt_jval_t *>(p);
}

void tt_jdoc_set_root(IN tt_jdoc_t *jd, IN TO tt_jval_t *jv)
{
    Document *p = static_cast<Document *>(jd->p);
    Value *v = reinterpret_cast<Value *>(jv);
    p->Swap(*v);
    tt_jval_destroy(jv);
}

void tt_jdoc_parse_attr_default(IN tt_jdoc_parse_attr_t *attr)
{
    TT_ASSERT(attr != nullptr);

    attr->encoding = TT_JDOC_AUTO;
}

tt_result_t tt_jdoc_parse(IN tt_jdoc_t *jd, IN void *buf, IN tt_u32_t len,
                          IN OPT tt_jdoc_parse_attr_t *attr)
{
    TT_ASSERT(jd != nullptr);
    TT_ASSERT(buf != nullptr);

    Document *p = static_cast<Document *>(jd->p);

    tt_jdoc_parse_attr_t __attr;
    if (attr == nullptr) {
        tt_jdoc_parse_attr_default(&__attr);
        attr = &__attr;
    }

    ParseResult pr;
    // clang-format off
    MemoryStream ms((MemoryStream::Ch *)buf, len);
    switch (attr->encoding) {
        case TT_JDOC_UTF8: {
            EncodedInputStream<UTF8<>, MemoryStream> eis(ms);
            pr = p->ParseStream<PARSE_FLAG, UTF8<> >(eis);
        } break;
        case TT_JDOC_UTF16_LE: {
            EncodedInputStream<UTF16LE<>, MemoryStream> eis(ms);
            pr = p->ParseStream<PARSE_FLAG, UTF16LE<> >(eis);
        } break;
        case TT_JDOC_UTF16_BE: {
            EncodedInputStream<UTF16BE<>, MemoryStream> eis(ms);
            pr = p->ParseStream<PARSE_FLAG, UTF16BE<> >(eis);
        } break;
        case TT_JDOC_UTF32_LE: {
            EncodedInputStream<UTF32LE<>, MemoryStream> eis(ms);
            pr = p->ParseStream<PARSE_FLAG, UTF32LE<> >(eis);
        } break;
        case TT_JDOC_UTF32_BE: {
            EncodedInputStream<UTF32BE<>, MemoryStream> eis(ms);
            pr = p->ParseStream<PARSE_FLAG, UTF32BE<> >(eis);
        } break;
        case TT_JDOC_AUTO:
        default: {
            AutoUTFInputStream<unsigned int, MemoryStream> ais(ms);
            pr = p->ParseStream<PARSE_FLAG, AutoUTF<unsigned int> >(ais);
        } break;
    }
    // clang-format on
    if (pr.IsError()) {
        TT_ERROR("fail to parse json: %s", __pec_str(pr.Code()));
        return TT_FAIL;
    }

    return TT_SUCCESS;
}

tt_result_t tt_jdoc_parse_file(IN tt_jdoc_t *jd, IN const tt_char_t *path,
                               IN OPT tt_jdoc_parse_attr_t *attr)
{
    tt_buf_t buf;
    tt_buf_init(&buf, nullptr);
    if (!TT_OK(tt_fcontent_buf(path, &buf)) || !TT_OK(tt_buf_put_u8(&buf, 0))) {
        tt_buf_destroy(&buf);
        return TT_FAIL;
    }

    tt_result_t result =
        tt_jdoc_parse(jd, TT_BUF_RPOS(&buf), TT_BUF_RLEN(&buf) - 1, attr);
    tt_buf_destroy(&buf);
    return result;
}

void tt_jdoc_render_attr_default(IN tt_jdoc_render_attr_t *attr)
{
    TT_ASSERT(attr != nullptr);

    attr->encoding = TT_JDOC_UTF8;
    attr->bom = TT_FALSE;
}

tt_result_t tt_jdoc_render(IN tt_jdoc_t *jd, IN tt_buf_t *buf,
                           IN OPT tt_jdoc_render_attr_t *attr)
{
    TT_ASSERT(jd != nullptr);
    TT_ASSERT(buf != nullptr);

    Document *p = static_cast<Document *>(jd->p);
    TT_ASSERT(p != nullptr);

    tt_jdoc_render_attr_t __attr;
    if (attr == nullptr) {
        tt_jdoc_render_attr_default(&__attr);
        attr = &__attr;
    }

    jw_mem jm(buf);
    AutoUTFOutputStream<unsigned int, jw_mem>
        aos(jm, tt_s_utftype_map[attr->encoding], attr->bom);
    // clang-format off
    Writer<AutoUTFOutputStream<unsigned int, jw_mem>,
           UTF8<>,
           AutoUTF<unsigned int> >
        writer(aos);
    // clang-format on
    if (!p->Accept(writer) || jm.error()) { return TT_FAIL; }

    return TT_SUCCESS;
}

tt_result_t tt_jdoc_render_file(IN tt_jdoc_t *jd, IN const tt_char_t *path,
                                IN OPT tt_jdoc_render_attr_t *attr)
{
    TT_ASSERT(jd != nullptr);
    TT_ASSERT(path != nullptr);

    Document *p = static_cast<Document *>(jd->p);
    TT_ASSERT(p != nullptr);

    tt_jdoc_render_attr_t __attr;
    if (attr == nullptr) {
        tt_jdoc_render_attr_default(&__attr);
        attr = &__attr;
    }

    jw_file jf(path);
    AutoUTFOutputStream<unsigned int, jw_file>
        aos(jf, tt_s_utftype_map[attr->encoding], attr->bom);
    // clang-format off
    Writer<AutoUTFOutputStream<unsigned int, jw_file>,
           UTF8<>,
           AutoUTF<unsigned int> >
        writer(aos);
    // clang-format on
    if (!p->Accept(writer) || jf.error()) { return TT_FAIL; }

    return TT_SUCCESS;
}

// ========================================
// json val
// ========================================

#if 0
tt_bool_t tt_jdoc_contain(IN tt_jdoc_t *jd, IN const tt_char_t *name)
{
    Document *p = static_cast<Document *>(jd->p);
    if (p->IsObject()) {
        return TT_BOOL(p->HasMember(name));
    } else {
        return TT_FALSE;
    }
}

tt_jval_t *tt_jdoc_find(IN tt_jdoc_t *jd, IN const tt_char_t *name)
{
    Document *p = static_cast<Document *>(jd->p);
    if (!p->IsObject()) {
        return nullptr;
    }

    Value::MemberIterator i = p->FindMember(name);
    if (i != p->MemberEnd()) {
        // get value reference, then its address
        return reinterpret_cast<tt_jval_t *>(&(*i));
    } else {
        return nullptr;
    }
}
#endif

const tt_char_t *__pec_str(IN ParseErrorCode pec)
{
    switch (pec) {
    case kParseErrorNone: return "No error.";
    case kParseErrorDocumentEmpty: return "The document is empty.";
    case kParseErrorDocumentRootNotSingular:
        return "The document root must not follow by other values.";
    case kParseErrorValueInvalid: return "Invalid value.";
    case kParseErrorObjectMissName: return "Missing a name for object member.";
    case kParseErrorObjectMissColon:
        return "Missing a colon after a name of object member.";
    case kParseErrorObjectMissCommaOrCurlyBracket:
        return "Missing a comma or '}' after an object member.";
    case kParseErrorArrayMissCommaOrSquareBracket:
        return "Missing a comma or ']' after an array element.";
    case kParseErrorStringUnicodeEscapeInvalidHex:
        return "Incorrect hex digit after \\u escape in string.";
    case kParseErrorStringUnicodeSurrogateInvalid:
        return "The surrogate pair in string is invalid.";
    case kParseErrorStringEscapeInvalid:
        return "Invalid escape character in string.";
    case kParseErrorStringMissQuotationMark:
        return "Missing a closing quotation mark in string.";
    case kParseErrorStringInvalidEncoding: return "Invalid encoding in string.";
    case kParseErrorNumberTooBig:
        return "Number too big to be stored in double.";
    case kParseErrorNumberMissFraction: return "Miss fraction part in number.";
    case kParseErrorNumberMissExponent: return "Miss exponent in number.";
    case kParseErrorTermination: return "Parsing was terminated.";
    case kParseErrorUnspecificSyntaxError: return "Unspecific syntax error.";
    default: return "Unknown";
    }
}
