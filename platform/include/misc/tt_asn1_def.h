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
@file tt_asn1_def.h
@brief ASN.1 definitions

this file includes ASN.1 definitions
*/

#ifndef __TT_ASN1_DEF__
#define __TT_ASN1_DEF__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

// ========================================
// tag values
// ========================================

#define TT_ASN1_CLASS_UNIVERSAL 0
#define TT_ASN1_CLASS_APPLICATION 1
#define TT_ASN1_CLASS_CONTEXT_SPECIFIC 2
#define TT_ASN1_CLASS_PRIVATE 3

// ========================================
// primitive/constructed
// ========================================

#define TT_ASN1_PC_PRIMITIVE 0
#define TT_ASN1_PC_CONSTRUCTED 1

// ========================================
// tag values
// ========================================

#define TT_ASN1_TAG_BOOLEAN 1
#define TT_ASN1_TAG_INTEGER 2
#define TT_ASN1_TAG_BIT_STRING 3
#define TT_ASN1_TAG_OCTET_STRING 4
#define TT_ASN1_TAG_NULL 5
#define TT_ASN1_TAG_OBJECT_IDENTIFIER 6
#define TT_ASN1_TAG_OBJECT_DESCRIPTOR 7
//#define TT_ASN1_TAG_ 8
#define TT_ASN1_TAG_REAL 9
#define TT_ASN1_TAG_ENUMERATED 10
#define TT_ASN1_TAG_UTF8_SRING 12
#define TT_ASN1_TAG_SEQUENCE 16
#define TT_ASN1_TAG_SET 17

// helper to make tag
#define TT_ASN1_MK_CLASS_PC_TAG(class, pc, tag)                                \
    (((class) << 6) | ((pc) << 5) | (tag))

#define TT_ASN1_TAG_UP(tag)                                                    \
    TT_ASN1_MK_CLASS_PC_TAG(TT_ASN1_CLASS_UNIVERSAL,                           \
                            TT_ASN1_PC_PRIMITIVE,                              \
                            (tag))
#define TT_ASN1_TAG_UC(tag)                                                    \
    TT_ASN1_MK_CLASS_PC_TAG(TT_ASN1_CLASS_UNIVERSAL,                           \
                            TT_ASN1_PC_CONSTRUCTED,                            \
                            (tag))

#define TT_ASN1_BOOLEAN TT_ASN1_TAG_UP(TT_ASN1_TAG_BOOLEAN)
#define TT_ASN1_INTEGER TT_ASN1_TAG_UP(TT_ASN1_TAG_INTEGER)
#define TT_ASN1_BIT_STRING TT_ASN1_TAG_UP(TT_ASN1_TAG_BIT_STRING)
#define TT_ASN1_OCTET_STRING TT_ASN1_TAG_UP(TT_ASN1_TAG_OCTET_STRING)
#define TT_ASN1_NULL TT_ASN1_TAG_UP(TT_ASN1_TAG_NULL)
#define TT_ASN1_OBJECT_IDENTIFIER TT_ASN1_TAG_UP(TT_ASN1_TAG_OBJECT_IDENTIFIER)
#define TT_ASN1_OBJECT_DESCRIPTOR TT_ASN1_TAG_UP(TT_ASN1_TAG_OBJECT_DESCRIPTOR)
#define TT_ASN1_TAG_VAL_REAL TT_ASN1_TAG_UP(TT_ASN1_TAG_REAL)
#define TT_ASN1_TAG_VAL_ENUMERATED TT_ASN1_TAG_UP(TT_ASN1_TAG_ENUMERATED)
#define TT_ASN1_UTF8_SRING TT_ASN1_TAG_UP(TT_ASN1_TAG_UTF8_SRING)
#define TT_ASN1_SEQUENCE TT_ASN1_TAG_UC(TT_ASN1_TAG_SEQUENCE)
#define TT_ASN1_SET TT_ASN1_TAG_UC(TT_ASN1_TAG_SET)

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_ASN1_DEF__ */
