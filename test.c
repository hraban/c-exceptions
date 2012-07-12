#include <stdio.h>
#include <stdlib.h>

#include "exceptions.h"

static void
throw_if_odd(int x, exception_t e)
{
    if (x == 31415) {
        THROW(E_PI);
    }
    if (x % 2) {
        THROW(e);
    }
}

static void
try_and_catch(int x)
{
    BEGIN_TRY
    {
        throw_if_odd(x, E2);
        printf("%d is even\n", x);
    }
    CATCH(E1)
    {
        printf("Caught E1\n");
    }
    CATCH(E2)
        printf("Caught E2\n");
    CATCH(E_PI)
    {
        printf("That's (almost) PI!\n");
    }
    FINALLY
    {
        printf("I have no idea what just happened.\n");
    }
    END_TRY
}

int
main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    try_and_catch(0);
    try_and_catch(1);
    try_and_catch(2);
    try_and_catch(31415);

    return 0;
}
