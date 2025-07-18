#include <stdio.h>
#include <stdlib.h>

struct foo
{
    int val; 
}; 

int main(int argc, char *argv[])
{
    struct foo test_foo;
    test_foo.val = 100; 

    int val = &test_foo; 

    printf("%d\n", val);
    return EXIT_SUCCESS;
}
