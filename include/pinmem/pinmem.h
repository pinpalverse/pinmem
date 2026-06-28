#ifndef __PINMEM_MEM
#define __PINMEM_MEM

#define SENTINEL 0xDE
#define STACK_SIZE 1024

#include <errno.h>
#include <pinlog/pinlog.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef PINMEM_DEBUG
#define PINMEM_DEBUG 1
#endif

#ifndef PINMEM_DEBUG_ALLOCATION_ATTEMPT
#define PINMEM_DEBUG_ALLOCATION_ATTEMPT 1
#endif

#define pmalloc(size, ...)                                                                                                       \
  _pmalloc(size, __FILE_NAME__, __func__, __LINE__);  // The '...' is added for cross-operability between pinmem and pinmem-lite

#define pfree(p, ...)                             \
  {                                               \
    _pfree(p, __FILE_NAME__, __func__, __LINE__); \
  };

typedef struct {
  int size;
  void* ptr;
  bool used;
  char* filename;
  char* function;
  int line;
} PINMT;

static PINMT _alloctable[STACK_SIZE] = {
    [0 ... STACK_SIZE - 1] = {.size = -1, .ptr = NULL, .used = false}
};  // gcc extension
static int count = 0;

void* prealloc(void* p, size_t new_size);

void* preallocarray(void* optr, size_t nmemb, size_t elem_size);

void* _pmalloc(size_t size, const char* filename, const char* function,
               int line);

void _dump_mem_table();

void _pfree(void* p, const char* filename, const char* function,
            int line);

#endif
