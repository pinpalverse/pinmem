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
static int _alloctable[STACK_SIZE] = {[0 ... STACK_SIZE-1] = -1}; // GCC extension
static bool PIN_DEBUG_WHEN_UNUSED = true;
void *pmalloc(size_t size, int tracker)
{
    void *s = (void*)malloc(size);
    if (PIN_DEBUG)
    {
        if (tracker >= STACK_SIZE || tracker < 0) {pinlog(ERROR, "Tracker %d is not within the stack range [0, %d], if a must, change the stack size. Skipping tracking", tracker, STACK_SIZE);}
        else if (_alloctable[tracker] != -1) {pinlog(ERROR, "Tracker %d is already in use. Skipping tracking", tracker);}
        else{
            _alloctable[tracker] = size;
            memset(s, SENTINEL, size);
        }
    }
    else
    {
        if (PIN_DEBUG_WHEN_UNUSED) pinlog(INFO, "Nothing being tracked");
    }
    return s;
}

void pfree(void* p, int tracker)
{
    if (PIN_DEBUG)
    {
        if (tracker >= STACK_SIZE || tracker < 0) {pinlog(ERROR, "Tracker %d is not within the stack range [0, %d], if a must, change the stack size. Skipping tracking", tracker, STACK_SIZE);}
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
                pinlog(WARN, "PINMEM: %d %s not used", unused,
                       unused > 1 ? "bytes were" : "byte was");
            }
        }
        _alloctable[tracker] = -1;
    }
    else
    {
        if (PIN_DEBUG_WHEN_UNUSED) pinlog(INFO, "Nothing being tracked");
    }

    free(p);
}


#endif