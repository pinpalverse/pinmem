#include <pinmem/pinmem.h>
#include <stdio.h>
#include <string.h>

int main() {
  set_pinmem_debug(false);
  set_pinmem_debug_allocation_attempt(false);

  char* buf = pmalloc(150);
  strcpy(buf, "Hi there sdfokksdfoksdofksdofkok");
  _dump_mem_table();
  printf("%s\n", buf);
  pfree(buf);

  return 0;
}
