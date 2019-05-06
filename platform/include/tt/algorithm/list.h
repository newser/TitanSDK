/* Copyright (C) 2017 haniu (niuhao.cn@gmail.com)
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
@file list.h
@brief list data structure

this file defines list data structure and related operations.

 - push_tail/push_head/pop_tail/pop_head
 - head/tail
 - insert_front/insert_back
 - count/empty
 - clear
 - contain/contain_all
 - find/find_from/find_last
 - remove(node)/remove_equal

*/

#ifndef __TT_LIST_CPP__
#define __TT_LIST_CPP__

////////////////////////////////////////////////////////////
// import header files
////////////////////////////////////////////////////////////

#include <tt/misc/util.h>

namespace tt {

////////////////////////////////////////////////////////////
// macro definition
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// type definition
////////////////////////////////////////////////////////////

class list;

class lnode
{
    friend class list;

public:
    lnode() = default;

    list *list() const { return list_; };
    lnode *prev() const { return prev_; };
    lnode *next() const { return next_; };
    bool in_list() { return list_ == nullptr; }

    void insert_front(lnode &n);
    void insert_back(lnode &n);
    lnode *remove();

private:
    void clear()
    {
        list_ = nullptr;
        prev_ = nullptr;
        next_ = nullptr;
    }

    class list *list_{nullptr};
    lnode *prev_{nullptr};
    lnode *next_{nullptr};

    TT_NON_COPYABLE(lnode)
};

class list
{
    friend class lnode;

public:
    list() = default;

    size_t count() const { return count_; }
    bool empty() const { return count_ == 0; }
    bool contain(const lnode &n) const { return n.list_ == this; }

    lnode *head() const { return head_; }
    lnode *tail() const { return tail_; }
    void push_head(lnode &n);
    void push_tail(lnode &n);
    lnode *pop_head();
    lnode *pop_tail();

    void clear()
    {
        while (pop_head() != nullptr)
            ;
    }

private:
    lnode *head_{nullptr};
    lnode *tail_{nullptr};
    size_t count_{0};

    TT_NON_COPYABLE(list)
};

////////////////////////////////////////////////////////////
// global variants
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// interface declaration
////////////////////////////////////////////////////////////

}

#endif /* __TT_LIST_CPP__ */
