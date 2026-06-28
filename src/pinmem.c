#include "../include/pinmem/pinmem.h"

#include <stdlib.h>

// Runs after main() to check for any left overs
__attribute__((destructor)) void postmain() {
  pinlog(INFO, "Hi there");
  if (!PINMEM_DEBUG) return;
  for (int i = 0; i < count; i++) {
    if (_alloctable[i].used) pinlog(WARN,
                                    "Pointer %p(%s:%s:%d) (of size %d) was not freed",
                                    _alloctable[i].ptr,
                                    _alloctable[i].filename,
                                    _alloctable[i].function,
                                    _alloctable[i].line,
                                    _alloctable[i].size);
  }
};

void* prealloc(void* p, size_t new_size) {
  if (!PINMEM_DEBUG) {
    return realloc(p, new_size);
  }
  for (int i = 0; i < count; i++) {
    if (_alloctable[i].ptr == p) {
      _alloctable[i].ptr = realloc(_alloctable[i].ptr, new_size);
      _alloctable[i].size = new_size;
      return _alloctable[i].ptr;
    }
  }
  if (PINMEM_DEBUG) {
    pinlog(WARN, "Pointer %p was not found in pinmem's allocation table", p);
  }
  return NULL;
}

void* preallocarray(void* optr, size_t nmemb, size_t elem_size) {
  if (!PINMEM_DEBUG) {
    return reallocarray(optr, nmemb, elem_size);
  }
  size_t bytes;
  if (__builtin_mul_overflow(nmemb, elem_size, &bytes)) {
    pinlog(ERROR, "overflow detected");
    return NULL;
  }
  return realloc(optr, bytes);
}
void* _pmalloc(size_t size, const char* filename, const char* function,
               int line) {
  pinlog(INFO, "%s %s\n", PINMEM_DEBUG ? "true" : "false", PINMEM_DEBUG_ALLOCATION_ATTEMPT ? "true" : "false");

  if (!PINMEM_DEBUG) {
    return malloc(size);
  }
  if (PINMEM_DEBUG_ALLOCATION_ATTEMPT) {
    pinlog(INFO, "Attemping to allocate %d bytes for pointer in %s:%s():%d", size, filename, function, line);
  }
  if (count < STACK_SIZE && !_alloctable[count].used) {
    _alloctable[count].ptr = (void*)malloc(size);
    memset(_alloctable[count].ptr, SENTINEL, size);
    _alloctable[count].size = size;
    _alloctable[count].filename = (char*)malloc(strlen(filename) + 1);
    strncpy(_alloctable[count].filename, filename, strlen(filename));
    _alloctable[count].function = (char*)malloc(strlen(function) + 1);
    strncpy(_alloctable[count].function, function, strlen(function));
    _alloctable[count].line = line;
    _alloctable[count++].used = true;
  } else {
    pinlog(WARN,
           "Stack table ran out of space (%s:%s():%d) (STACKSIZE: %d, please increase it)",
           filename,
           function,
           line,
           STACK_SIZE);
    return NULL;
  }
  return _alloctable[count - 1].ptr;
}

void _dump_mem_table() {
  if (!PINMEM_DEBUG) {
    return;
  }
  for (int i = 0; i < count; i++) {
    if (_alloctable[i].size > 0) {
      printf("== [%d] -> %p(%s:%s():%d)='", i, _alloctable[i].ptr, _alloctable[i].filename, _alloctable[i].function, _alloctable[i].line);
      if (_alloctable[i].size > 100) {
        fwrite(_alloctable[i].ptr, 1, 48, stdout);
        printf("....");
        fwrite((void*)&_alloctable[i].ptr[_alloctable[i].size - 48], 1, 48, stdout);
      } else {
        fwrite(_alloctable[i].ptr, 1, _alloctable[i].size, stdout);
      }
      printf("' of size %d ==\n", _alloctable[i].size);
    }
  }
}

void _pfree(void* p, const char* filename, const char* function,
            int line) {
  if (!PINMEM_DEBUG) {
    return free(p);
  }
  for (int i = 0; i < count; i++) {
    if (_alloctable[i].ptr == p) {
      unsigned char* data = (unsigned char*)p;
      int unused = 0;
      for (int j = 0; j < _alloctable[i].size; j++) {
        if (*data++ == SENTINEL) unused++;
      }
      if (unused > 0 && PINMEM_DEBUG) {
        pinlog(INFO, "PINMEM: %p(%s:%s():%d) %d %s not used", _alloctable[i].ptr, _alloctable[i].filename, _alloctable[i].function, _alloctable[i].line, unused, unused > 1 ? "bytes were" : "byte was");
      }
      free(_alloctable[i].ptr);
      _alloctable[i].ptr = NULL;
      _alloctable[i].size = 0;
      _alloctable[i].used = false;
      free(_alloctable[i].filename);
      free(_alloctable[i].function);
      _alloctable[i].filename = NULL;
      _alloctable[i].function = NULL;
      _alloctable[i].line = -1;
      for (int k = i; k < count - 1; k++) {
        _alloctable[k] = _alloctable[k + 1];
      }
      if (count >= 1) count--;
      return;
    }
  }
  pinlog(WARN,
         "Pointer %p(%s:%s():%d) was not found in pinmem's allocation table",
         p,
         filename,
         function,
         line);
}
