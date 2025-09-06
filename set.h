/*
BSD 3-Clause License

Copyright (c) 2024, Mashpoe
Copyright (c) 2025, Simile (or not?)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#ifdef __cpp_decltype
#include <type_traits>
#define typeof(T) std::remove_reference<std::add_lvalue_reference<decltype(T)>::type>::type
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>

// generic type for internal use
typedef void* set;
// number of elements in a set
typedef size_t set_size_t;
// number of bytes for a type
typedef size_t set_type_t;

// TODO: more rigorous check for typeof support with different compilers
#if _MSC_VER == 0 || __STDC_VERSION__ >= 202311L || defined __cpp_decltype

// shortcut defines

// set_addr is a set* (aka type**)
#define set_add_dst(set_addr)\
	((typeof(*set_addr))(\
	    _set_add_dst((set*)set_addr, sizeof(**set_addr))\
	))
#define set_insert_dst(set_addr, pos)\
	((typeof(*set_addr))(\
	    _set_insert_dst((set*)set_addr, sizeof(**set_addr), pos)))

#define set_add(set_addr, value)\
        if (!set_contains((set*)set_addr, (unsigned char)value)) { \
	    (*set_add_dst(set_addr) = value); \
	}
#define set_insert(set_addr, pos, value)\
        if (!set_contains((set*)set_addr, (unsigned char)value)) { \
	    (*set_insert_dst(set_addr, pos) = value); \
	}

#else

#define set_add_dst(set_addr, type)\
	((type*)_set_add_dst((set*)set_addr, sizeof(type)))
#define set_insert_dst(set_addr, type, pos)\
	((type*)_set_insert_dst((set*)set_addr, sizeof(type), pos))

#define set_add(set_addr, type, value)\
        if (!set_contains((set*)set_addr, (unsigned char)value)) { \
	    (*set_add_dst(set_addr, type) = value); \
	}
#define set_insert(set_addr, type, pos, value)\
        if (!set_contains((set*)set_addr, (unsigned char)value)) { \
	    (*set_insert_dst(set_addr, type, pos) = value); \
	}

#endif

// st is a set (aka type*)
#define set_erase(st, pos, len)\
	(_set_erase((set)st, sizeof(*st), pos, len))
#define set_remove(st, pos)\
	(_set_remove((set)st, sizeof(*st), pos))

#define set_reserve(set_addr, capacity)\
	(_set_reserve((set*)set_addr, sizeof(**set_addr), capacity))

#define set_copy(st)\
	(_set_copy((set)st, sizeof(*st)))

set set_create(void);

void set_free(set st);

void* _set_add_dst(set* set_addr, set_type_t type_size);

void* _set_insert_dst(set* set_addr, set_type_t type_size, set_size_t pos);

void _set_erase(set set_addr, set_type_t type_size, set_size_t pos, set_size_t len);

void _set_remove(set set_addr, set_type_t type_size, set_size_t pos);

void set_pop(set st);

void _set_reserve(set* set_addr, set_type_t type_size, set_size_t capacity);

set _set_copy(set st, set_type_t type_size);

set_size_t set_size(set st);

set_size_t set_capacity(set st);

bool set_contains(set* set_addr, unsigned char value);

// closing bracket for extern "C"
#ifdef __cplusplus
}
#endif
