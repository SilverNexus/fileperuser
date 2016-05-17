/**
 * @file jump_table.h
 * Holds information about the jump table that will be built
 * for Boyer-Moore string search.
 */

#ifndef JUMP_TABLE_H
#define JUMP_TABLE_H

#include <stddef.h>

extern size_t jump_tbl[];
extern size_t needle_len;

void setup_jump_table();

#endif
