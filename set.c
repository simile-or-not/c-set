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

#include "set.h"
#include <string.h>
#include <stdint.h>

typedef struct {
	set_size_t size;
	set_size_t capacity;
	unsigned char data[];
} set_header;

set_header* set_get_header(set st) { return &((set_header*)st)[-1]; }

set set_create(void) {
	set_header* h = (set_header*)malloc(sizeof(set_header));
	h->capacity = 0;
	h->size = 0;

	return &h->data;
}

void set_free(set st) { free(set_get_header(st)); }

set_size_t set_size(set st) { return set_get_header(st)->size; }

set_size_t set_capacity(set st) { return set_get_header(st)->capacity; }

set_header* set_realloc(set_header* h, set_type_t type_size) {
	set_size_t new_capacity = (h->capacity == 0) ? 1 : h->capacity * 2;
	set_header* new_h = (set_header*)realloc(h, sizeof(set_header) + new_capacity * type_size);
	new_h->capacity = new_capacity;

	return new_h;
}

bool set_has_space(set_header* h) {
	return h->capacity - h->size > 0;
}

#define set_contains(set_addr, value) ({ \
	set_header* h = set_get_header(*set_addr); \
	for (int i = 0; i != set_size(&h->data); i++) { \
	    if ((&h->data)[i] == value) { \
                return true; \
            } \
	} \
	return false; \
})

void* _set_add_dst(set* set_addr, set_type_t type_size) {
	set_header* h = set_get_header(*set_addr);

	if (!set_has_space(h)) {
		h = set_realloc(h, type_size);
		*set_addr = h->data;
	}

	return &h->data[type_size * h->size++];
}

void* _set_insert_dst(set* set_addr, set_type_t type_size, set_size_t pos) {
	set_header* h = set_get_header(*set_addr);

	set_size_t new_length = h->size + 1;

	// make sure there is enough room for the new element
	if (!set_has_space(h)) {
		h = set_realloc(h, type_size);
		*set_addr = h->data;
	}
	// move trailing elements
	memmove(&h->data[(pos + 1) * type_size],
		&h->data[pos * type_size],
		(h->size - pos) * type_size);

	h->size = new_length;

	return &h->data[pos * type_size];
}

void _set_erase(set st, set_type_t type_size, set_size_t pos, set_size_t len) {
	set_header* h = set_get_header(st);
	memmove(&h->data[pos * type_size],
		&h->data[(pos + len) * type_size],
		(h->size - pos - len) * type_size);

	h->size -= len;
}

void _set_remove(set st, set_type_t type_size, set_size_t pos) {
	_set_erase(st, type_size, pos, 1);
}

void set_pop(set st) { --set_get_header(st)->size; }

void _set_reserve(set* set_addr, set_type_t type_size, set_size_t capacity) {
	set_header* h = set_get_header(*set_addr);
	if (h->capacity >= capacity) {
		return;
	}

	h = (set_header*)realloc(h, sizeof(set_header) + capacity * type_size);
	h->capacity = capacity;
	*set_addr = &h->data;
}

set _set_copy(set st, set_type_t type_size) {
	set_header* h = set_get_header(st);
	size_t alloc_size = sizeof(set_header) + h->size * type_size;
	set_header* copy_h = (set_header*)malloc(alloc_size);
	memcpy(copy_h, h, alloc_size);
	copy_h->capacity = copy_h->size;

	return &copy_h->data;
}
