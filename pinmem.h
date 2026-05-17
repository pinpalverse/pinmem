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


typedef struct{int size; void* ptr; bool used;} PINMT ;
static bool PIN_DEBUG = true;


static PINMT _alloctable[STACK_SIZE] = {[0 ... STACK_SIZE -1] = {.size = -1, .ptr=nullptr,.used=false}}; // gcc extension
int count = 0;


// Runs after main() to check for any left overs
__attribute__((destructor))
void postmain()
{
    if (PIN_DEBUG)
    {
        for(int i = 0; i < count;i++){
            if(_alloctable[i].used) pinlog(WARN, "Pointer %p (of size %d) was not freed",_alloctable[i].ptr,_alloctable[i].size);
        }
    }
}

void *pmalloc(size_t size)
{
    if(count < STACK_SIZE && !_alloctable[count].used){
        _alloctable[count].ptr = (void*)malloc(size);
        memset(_alloctable[count].ptr, SENTINEL, size);
        _alloctable[count].size = size;
        _alloctable[count++].used = true;
    }else{
        if(PIN_DEBUG) pinlog(WARN, "Stack table ran out of space (STACKSIZE: %d, please increase it)",STACK_SIZE);
        return  nullptr;
    }
    return _alloctable[count-1].ptr;
}


void _dump_mem_table(){
    for(int i = 0; i < count;i++){
        if(_alloctable[i].size > 0){
            printf("== [%d] -> %p='%s' of size %d ==\n", i, _alloctable[i].ptr,(char*)_alloctable[i].ptr, _alloctable[i].size);
        }
}
}

void pfree(void* p)
{
    for(int i = 0; i < count;i++){
        if(_alloctable[i].ptr == p){

            unsigned char *data = (unsigned char *)p;
            int unused = 0;
            for (int j = 0; j < _alloctable[i].size; j++) {
                if (*data++ == SENTINEL) unused++;
            }
            if (unused > 0 && PIN_DEBUG)
            {
                pinlog(INFO, "PINMEM: %d %s not used", unused,
                       unused > 1 ? "bytes were" : "byte was");
            }

            free(_alloctable[i].ptr);
            _alloctable[i].size = 0;
            _alloctable[i].used = false;
            
            for (int k = i; k < count - 1; k++) {
                _alloctable[k] = _alloctable[k + 1];
            }

            if(count > 1) count--;
            return;
        }
    }
    if(PIN_DEBUG){
        pinlog(WARN, "Pointer %p was not found in pinmem's allocation table",p);
    }
}


#endif