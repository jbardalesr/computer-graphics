#include <stdio.h>

void call(int, void (*f)(int));
void called(int);

int main(int argc, char const *argv[])
{
    call(5, called);
    getchar();
    return 0;
}

void call(int a, void (*f)(int))
{
    printf("a: %d\n", a);
    f(a * a + 2 * a + 1);
}

void called(int b)
{
    printf("b: %d\n", b);
}