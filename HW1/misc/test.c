#include <stdio.h>

int main(void) {
    int *x; x = (int*)0x280000000; // 10 gb
    *x = 69;
    printf("hi %d\n", *x);
    return 0;
}
