#include "pinmem.h"
#include <stdio.h>
#include <string.h>

int main(){
    PIN_DEBUG = true;
    PIN_DEBUG_WHEN_UNUSED = true;

    char* buf = pmalloc(100, 1);
    strcpy(buf, "Hi there");

    printf("%s\n", buf);
    pfree(buf,1);

    return 0;
}