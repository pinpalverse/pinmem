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

#define pmalloc(size,...) { _pmalloc(size, __FILE_NAME__, __func__, __LINE__) };  // The '...' is added for cross-operability between pinmem and pinmem-lite


typedef struct {int size; void *ptr; bool used; char *filename; char *function; int line;} PINMT;
static bool PIN_DEBUG = true;


static PINMT _alloctable[STACK_SIZE] = {[0 ... STACK_SIZE - 1] = {.size = -1, .ptr = nullptr, .used = false}}; // gcc extension
int count = 0;




// Runs after main() to check for any left overs
__attribute__((destructor))
void postmain()
{
    if (PIN_DEBUG)
    {
        for (int i = 0; i < count; i++)
        {
            if (_alloctable[i].used) pinlog(WARN, "Pointer %p(%s:%s:%d) (of size %d) was not freed",
                                                _alloctable[i].ptr, _alloctable[i].filename, _alloctable[i].function, _alloctable[i].line, _alloctable[i].size);
        }
    }
};

void *_pmalloc(size_t size, const char* filename, const char* function, int line)
{
    if (count < STACK_SIZE && !_alloctable[count].used)
    {
        _alloctable[count].ptr = (void*)malloc(size);
        memset(_alloctable[count].ptr, SENTINEL, size);
        _alloctable[count].size = size;
        _alloctable[count].filename = (char*)malloc(strlen(filename) +1);
        strncpy(_alloctable[count].filename, filename, strlen(filename));
        _alloctable[count].function = (char*)malloc(strlen(function) +1);
        strncpy(_alloctable[count].function, function, strlen(function));
        _alloctable[count].line = line;
        _alloctable[count++].used = true;
    }
    else
    {
        if (PIN_DEBUG) pinlog(WARN,
                                  "Stack table ran out of space (STACKSIZE: %d, please increase it)", STACK_SIZE);
        return  nullptr;
    }
    return _alloctable[count - 1].ptr;
}


void _dump_mem_table()
{
    for (int i = 0; i < count; i++)
    {
        if (_alloctable[i].size > 0)
        {
            
            printf("== [%d] -> %p(%s:%s:%d)='", i, _alloctable[i].ptr,
                   _alloctable[i].filename, _alloctable[i].function, _alloctable[i].line);
            if(_alloctable[i].size > 100){
            fwrite(_alloctable[i].ptr,1,48, stdout);
            printf("....");
            fwrite((void*)&_alloctable[i].ptr[_alloctable[i].size-48],1,48, stdout);
            }else{
                fwrite(_alloctable[i].ptr,1,_alloctable[i].size, stdout);
            }
            printf("' of size %d ==\n", _alloctable[i].size);
        }
    }
}

void pfree(void* p, ...)
{
    for (int i = 0; i < count; i++)
    {
        if (_alloctable[i].ptr == p)
        {
            unsigned char *data = (unsigned char *)p;
            int unused = 0;
            for (int j = 0; j < _alloctable[i].size; j++)
            {
                if (*data++ == SENTINEL) unused++;
            }
            if (unused > 0 && PIN_DEBUG)
            {
                pinlog(INFO, "PINMEM: %p(%s:%s:%d) %d %s not used", _alloctable[i].ptr,
                       _alloctable[i].filename, _alloctable[i].function, _alloctable[i].line, unused,
                       unused > 1 ? "bytes were" : "byte was");
            }
            free(_alloctable[i].ptr);
            _alloctable[i].ptr = nullptr;
            _alloctable[i].size = 0;
            _alloctable[i].used = false;
            free(_alloctable[i].filename);
            free(_alloctable[i].function);
            _alloctable[i].filename = nullptr;
            _alloctable[i].function = nullptr;
            _alloctable[i].line = -1;
            for (int k = i; k < count - 1; k++)
            {
                _alloctable[k] = _alloctable[k + 1];
            }
            if (count > 1) count--;
            return;
        }
    }
    if (PIN_DEBUG)
    {
        pinlog(WARN, "Pointer %p was not found in pinmem's allocation table", p);
    }
}


#endif
