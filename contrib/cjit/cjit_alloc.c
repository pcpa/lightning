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
 * Initialization
 */
static hash_t	*intern;

/*
 * Implementation
 */
void
init_alloc(void)
{
    intern = new_hash(hash_string);
}

void *
xmalloc(size_t size)
{
    void	*pointer = malloc(size);

    assert(pointer != NULL);

    return (pointer);
}

void *
xrealloc(void *pointer, size_t size)
{
    pointer = realloc(pointer, size);

    assert(pointer != NULL);

    return (pointer);
}

void *
xcalloc(size_t nmemb, size_t size)
{
    void	*pointer = calloc(nmemb, size);

    assert(pointer != NULL);

    return (pointer);
}

char *
xintern(const char *string)
{
    entry_t	*entry;

    if ((entry = get_hash(intern, (char *)string)) == NULL) {
	entry = (entry_t *)xmalloc(sizeof(entry_t));
	entry->name.string = xstrdup(string);
	entry->value = entry->name.string;
	put_hash(intern, entry);
    }

    return (entry->value);
}

char *
xstrdup(const char *string)
{
    char	*pointer = strdup(string);

    assert(pointer != NULL);

    return (pointer);
}
