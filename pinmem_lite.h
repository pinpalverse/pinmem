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
#include "pincrypto/pincrypto.h"


static bool PIN_DEBUG = false;
static bool PIN_SHOW_DEBUG_WHEN_UNUSED = true;

static int _alloctable[STACK_SIZE] = {[0 ... STACK_SIZE - 1] = -1}; // GCC extension


// Runs after main() to check for any left overs
__attribute__((destructor))
void postmain()
{
    if (PIN_DEBUG)
    {
        for (int i = 0; i < STACK_SIZE; i++)
        {
            if (_alloctable[i] != -1)
            {
                pinlog(WARN, "Memory of tracker No:%d  was not freed", i);
            }
        }
    }
}

void *pmalloc(size_t size, int tracker)
{
    void *s = (void*)malloc(size);
    if (PIN_DEBUG)
    {
        if (tracker >= STACK_SIZE || tracker < 0) {pinlog(ERROR, "Tracker No:%d is not within the stack range [0, %d], if a must, change the stack size. Skipping tracking", tracker, STACK_SIZE);}
        else if (_alloctable[tracker] != -1) {pinlog(ERROR, "Tracker No:%d is already in use. Skipping tracking", tracker);}
        else
        {
            _alloctable[tracker] = size;
            memset(s, SENTINEL, size);
        }
    }
    else
    {
        if (PIN_SHOW_DEBUG_WHEN_UNUSED) pinlog(INFO, "Nothing being tracked");
    }
    return s;
}

void pfree(void* p, int tracker)
{
    if (PIN_DEBUG)
    {
        if (tracker >= STACK_SIZE || tracker < 0) {pinlog(ERROR, "Tracker No:%d is not within the stack range [0, %d], if a must, change the stack size. Skipping tracking", tracker, STACK_SIZE);}
        else if (_alloctable[tracker] == -1) {pinlog(WARN, "The pointer you are trying to free up was not allocated through pmalloc or the tracker you have used is wrong, so no analysis will be done");}
        else
        {
            unsigned char *data = (unsigned char *)p;
            int unused = 0;
            for (int i = 0; i < _alloctable[tracker]; i++)
            {
                if (*data++ == SENTINEL) unused++;
            }
            if (unused > 0)
            {
                pinlog(WARN, "PINMEM: Tracker No:%d -> %d %s not used", tracker, unused,
                       unused > 1 ? "bytes were" : "byte was");
            }
        }
        _alloctable[tracker] = -1;
    }
    else
    {
        if (PIN_SHOW_DEBUG_WHEN_UNUSED) pinlog(INFO, "Nothing being tracked");
    }
    free(p);
}


#endif