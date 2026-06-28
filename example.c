// #define PINMEM_DEBUG false
// #define PINMEM_DEBUG_ALLOCATION_ATTEMPT false

#include <stdio.h>
#include <string.h>

#include "include/pinmem/pinmem.h"

int main() {
  printf("%s %s\n", PINMEM_DEBUG ? "true" : "false", PINMEM_DEBUG_ALLOCATION_ATTEMPT ? "true" : "false");
  char* buf = pmalloc(150);
  strcpy(buf, "Hi there sdfokksdfoksdofksdofkok");
  _dump_mem_table();
  printf("%s\n", buf);
  pfree(buf);

  return 0;
}
