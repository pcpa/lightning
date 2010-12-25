/*
 * Copyright (C) 2010  Paulo Cesar Pereira de Andrade.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors:
 *	Paulo Cesar Pereira de Andrade
 */

#include "cjit.h"

/*
 * Prototypes
 */
static int
hash_key(hash_t *hash, void *name);

static int
hash_cmp(hash_t *hash, void *lvalue, void *rvalue);

static void
rehash(hash_t *hash);

/*
 * Implementation
 */
hash_t *
new_hash(int kind)
{
    hash_t	*hash;

    hash = (hash_t *)xmalloc(sizeof(hash_t));
    hash->count = 0;
    hash->entries = (entry_t **)xcalloc(hash->size = 32, sizeof(void *));
    hash->kind = kind;

    return (hash);
}

void
put_hash(hash_t *hash, entry_t *entry)
{
    entry_t	*prev, *ptr;
    int		 key = hash_key(hash, entry->name.pointer);

    for (prev = ptr = hash->entries[key]; ptr; prev = ptr, ptr = ptr->next) {
	if (hash_cmp(hash, entry->name.pointer, ptr->name.pointer) == 0)
	    error(NULL, "duplicated entry %p", entry->name.pointer);
    }
    if (prev == NULL)
	hash->entries[key] = entry;
    else
	prev->next = entry;
    entry->next = NULL;
    ++hash->count;
    if (hash->count > hash->size * 0.75)
	rehash(hash);
}

entry_t *
get_hash(hash_t *hash, void *name)
{
    entry_t	*entry;
    int		 key = hash_key(hash, name);

    for (entry = hash->entries[key]; entry; entry = entry->next) {
	if (hash_cmp(hash, entry->name.pointer, name) == 0)
	    return (entry);
    }
    return (NULL);
}

void
del_hash(hash_t *hash)
{
    entry_t	*next;
    entry_t	*entry;
    int		 offset;

    for (offset = 0; offset < hash->size; offset++) {
	for (entry = hash->entries[offset]; entry;) {
	    next = entry->next;
	    free(entry);
	    entry = next;
	}
    }
    free(hash->entries);
    free(hash);
}

static int
hash_key(hash_t *hash, void *name)
{
    char	*ptr;
    int		 key;

    if (hash->kind == hash_string) {
	for (key = 0, ptr = name; *ptr; ptr++)
	    key = (key << (key & 1)) ^ *ptr;
    }
    else
	key = (int)(long)name;

    return (key & (hash->size - 1));
}

static int
hash_cmp(hash_t *hash, void *lname, void *rname)
{
    long	value;

    if (hash->kind == hash_string)
	return (strcmp(lname, rname));

    value = (long)lname - (long)rname;
    return (value ? value < 0 ? -1 : 1 : 0);
}

static void
rehash(hash_t *hash)
{
    int		 i, size, key;
    entry_t	*entry, *next, **entries;

    size = hash->size;
    entries = (entry_t **)xcalloc(hash->size <<= 1, sizeof(void *));
    for (i = 0; i < size; i++) {
	for (entry = hash->entries[i]; entry; entry = next) {
	    next = entry->next;
	    key = hash_key(hash, entry->name.pointer);
	    entry->next = entries[key];
	    entries[key] = entry;
	}
    }
    free(hash->entries);
    hash->entries = entries;
}
