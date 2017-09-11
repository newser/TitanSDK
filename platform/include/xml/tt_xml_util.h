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
@file tt_xml_util.h
@brief xml util

this file specifies xml util APIs
*/

#ifndef __TT_XML_UTIL__
#define __TT_XML_UTIL__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

#define TN(p) (*reinterpret_cast<tt_xnode_t *>(&(p)))

#define PN(t) (*reinterpret_cast<pugi::xml_node *>(&t))

#define TA(p) (*reinterpret_cast<tt_xattr_t *>(&(p)))

#define PA(t) (*reinterpret_cast<pugi::xml_attribute *>(&t))

#define P_XP(x) (reinterpret_cast<pugi::xpath_query *>((x)->p))

#define P_XPVS(x) (reinterpret_cast<pugi::xpath_variable_set *>((x)->p))

#define P_XPNS(x) (reinterpret_cast<pugi::xpath_node_set *>((x)->p))

#define P_ITER(i)                                                              \
    (*reinterpret_cast<pugi::xpath_node_set::const_iterator *>(&(i)))

#define T_ITER(i) (*reinterpret_cast<const void **>(&(i)))

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

#endif /* __TT_XML_UTIL__ */
