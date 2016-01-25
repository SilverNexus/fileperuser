/**
 * @file jump_table.h
 * Last Modified 2016-01-25 by Daniel Hawkins
 *
 * Holds information about the jump table that will be built
 * for Boyer-Moore string search.
 */

#ifndef JUMP_TABLE_H
#define JUMP_TABLE_H

#include <stddef.h>

extern size_t *jump_tbl;
extern size_t needle_len;

void init_jump_table();

void setup_jump_table();

void cleanup_jump_table();

#endif
