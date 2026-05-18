#include "pinmem.h"
#include <stdio.h>
#include <string.h>

    // _dump_mem_table();
int main(){
    PIN_DEBUG = true;

    char* buf = pmalloc(150);
    strcpy(buf, "Hi there sdfokksdfoksdofksdofkok");
    _dump_mem_table();
    printf("%s\n", buf);
    pfree(buf);

    return 0;
}