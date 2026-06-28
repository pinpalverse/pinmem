#include <pinmem/pinmem.h>
#include <stdio.h>
#include <string.h>

int main() {
  // Must be changed from CMakeLists.txt due to the pipelining of the compilation process
  printf("%s %s\n", PINMEM_DEBUG ? "true" : "false", PINMEM_DEBUG_ALLOCATION_ATTEMPT ? "true" : "false");

  char* buf = pmalloc(150);
  strcpy(buf, "Hi there sdfokksdfoksdofksdofkok");
  _dump_mem_table();
  printf("%s\n", buf);
  pfree(buf);

  return 0;
}
