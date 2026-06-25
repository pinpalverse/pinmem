// This version is going to be the lite version of pinmem
// as it uses a basic tracking system and a predictable linear
// stack array to store sizes


#ifndef __PIN_MEM
#define __PIN_MEM

#define SENTINEL 0xDE
#define STACK_SIZE 1024

#include <stdlib.h>
#include <string.h>
#include "pinlog/pinlog.h"
#include <stdio.h>
#include <stdbool.h>


static bool PIN_DEBUG = false;
static bool PIN_SHOW_DEBUG_WHEN_UNUSED = true;

static int _alloctable[STACK_SIZE] = {[0 ... STACK_SIZE - 1] = -1}; // GCC extension


void *pmalloc(size_t size, int tracker);
void pfree(void* p, int tracker);


#endif
