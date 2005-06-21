// libreroot string stack support
// Copyright (C) 2003-2004 Oliver Brakmann <oliverbrakmann@users.berlios.de> &
// Gareth Jones <gareth_jones@users.berlios.de>
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA

#include <errno.h>	// errno.
#include <error.h>	// error.
#include <stdlib.h>	// free & malloc.
#include <string.h>	// strcat, strcpy & strlen.

#include "memory.h"

// Wrapper for malloc.
void *
reroot_alloc (unsigned const size)
{
	void *const restrict ptr = malloc (size);
	if (!ptr)
		error (1, errno, "libreroot: malloc () failed");
	return ptr;
}

// Initialize empty stack.
// FIXME: Inline this?
void
reroot_stack_init (reroot_stack *const restrict stack)
{
	stack->start = stack->end = 0;
	stack->items = stack->length = 0;
}

// Add string to stack.
void
reroot_stack_push (reroot_stack *const restrict stack,
                   char const *const restrict string)
{
	reroot_stack_item *const restrict end = stack->end;

	// Allocate new item.
	unsigned const length = strlen (string);
	reroot_stack_item *const restrict item =
		reroot_alloc (sizeof (reroot_stack_item) + length + 1);

	// Initialize new item.
	item->next = 0;
	item->prev = end;
	item->length = length;
	strcpy (item->string, string);

	// Append to list.
	if (end)
		end->next = item;
	else
		stack->start = item;
	stack->end = item;
	stack->length += length;
	++stack->items;
}

// Insert string at start of stack.
void
reroot_stack_insert (reroot_stack *const restrict stack,
                     char const *const restrict string)
{
	reroot_stack_item *const restrict start = stack->start;

	//Allocate new item.
	unsigned const length = strlen (string);
	reroot_stack_item *const restrict item =
		reroot_alloc (sizeof (reroot_stack_item) + length + 1);

	// Initialize new item.
	item->next = start;
	item->prev = 0;
	item->length = length;
	strcpy (item->string, string);

	// Prepend to list.
	if (start)
		start->prev = item;
	else
		stack->end = item;
	stack->start = item;
	stack->length += length;
	++stack->items;
}

// Remove last string from stack.
void
reroot_stack_pop (reroot_stack *const restrict stack)
{
	reroot_stack_item *const restrict end = stack->end;

	// Return if stack is empty.
	if (!end)
		return;

	// Remove from list.
	reroot_stack_item *const restrict prev = end->prev;
	stack->length -= end->length;
	--stack->items;
	if (prev)
		prev->next = 0;
	else
		stack->start = 0;
	stack->end = prev;

	free (end);
}

// Empty stack, freeing the memory used.
void
reroot_stack_empty (reroot_stack *const restrict stack)
{
	reroot_stack_item *restrict current = stack->end,
	                  *next;
	while (current)
	{
		next = current->prev;
		free (current);
		current = next;
	}

	// Reinitialize stack.
	reroot_stack_init (stack);
}

// Return a string built from the strings in stack.  The strings are separated
// by the specified deliminator.  If prefix is true, an extra deliminator will
// prefix the string.  The returned string must be freed later.
char *
reroot_stack_string (reroot_stack const *const restrict stack,
                     char const *const restrict delim,
                     bool const prefix)
{
	// Allocate string, allowing for deliminators & trailing null.
	unsigned const items = stack->items;
	char *restrict string = reroot_alloc (stack->length +
	                                      (prefix? items : items - 1) *
	                                      strlen (delim) + 1);

	// Make sure string starts empty.
	*string = 0;

	// Construct string from stack.
	reroot_stack_item const *restrict current = stack->start;
	while (current)
	{
		// Add deliminator if not start, or prefix required.
		if (prefix || *string)
			strcat (string, delim);

		strcat (string, current->string);
		current = current->next;
	}

	return string;
}
