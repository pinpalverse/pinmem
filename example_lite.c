#include "pinmem_lite.h"
#include <stdio.h>
#include <string.h>

int main(){
    PIN_DEBUG = true;

    char* buf = pmalloc(100,1);
    strcpy(buf, "Hi there");
    printf("%s\n", buf);
    pfree(buf,1);

    return 0;
}