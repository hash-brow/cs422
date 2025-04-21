#include <stdio.h>

struct tmp {
        int x;

        tmp() :
                x(69)
        {
        }
};

int main(void) {
        struct tmp* ptr = new tmp;
        printf("%d\n", ptr->x);
        return 0;
}

