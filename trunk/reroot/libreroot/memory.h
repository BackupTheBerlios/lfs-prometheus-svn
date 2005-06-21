// libreroot string stack support declarations
// Copyright (C) 2003-2005 Oliver Brakmann <oliverbrakmann@users.berlios.de> &
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

#ifndef MEMORY_H
# define MEMORY_H

# include <stdbool.h>

// A malloc wrapper.
void *reroot_alloc (unsigned const size) __attribute__ ((malloc));

// A doubly linked list item.
typedef struct reroot_stack_item
{
	struct reroot_stack_item *next,
	                         *prev;
	unsigned length;
	char string [];
} reroot_stack_item;

// A doubly linked list of strings, with stack semantics.
typedef struct reroot_stack
{
	reroot_stack_item *start,
	                  *end;
	unsigned items,
	         length;
} reroot_stack;

// Functions for working with string stacks.
void reroot_stack_init (reroot_stack *const restrict stack);
void reroot_stack_push (reroot_stack *const restrict stack,
	char const *const restrict string);
void reroot_stack_insert (reroot_stack *const restrict stack,
	char const *const restrict string);
void reroot_stack_pop (reroot_stack *const restrict stack);
void reroot_stack_empty (reroot_stack *const restrict stack);
char *reroot_stack_string (reroot_stack const *const restrict stack,
	char const *const restrict delim, bool const prefix); 

#endif
