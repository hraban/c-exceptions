#include <stdio.h>
#include <stdlib.h>

#include "exceptions.h"

const exception_class_t root_e = CREATE_EXCEPTION(NULL);
const exception_class_t odd_e = CREATE_EXCEPTION(&root_e);
const exception_class_t pi_e = CREATE_EXCEPTION(&odd_e);

static void
throw_if_odd(int x)
{
    if (x == 31415) {
        THROW_NEW(pi_e);
    }
    if (x % 2) {
        THROW_NEW(odd_e);
    }
}

static void
try_and_catch(int x)
{
    BEGIN_TRY
    {
        throw_if_odd(x);
        printf("%d is even", x);
    }
    /* When commented out, this will be caught as odd_e
    CATCH(pi_e)
    {
        printf("%d is (almost) PI", x);
    }
    */
    CATCH(odd_e)
    {
        printf("%d is odd", x);
    }
    FINALLY
    {
        printf(".\n");
    }
    END_TRY
}

static void
throw_in_finally(int x)
{
    BEGIN_TRY
    {
        throw_if_odd(x);
    }
    CATCH(odd_e)
    {
        /* No problem. */
    }
    FINALLY
    {
        THROW_NEW(odd_e);
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

    BEGIN_TRY
    {
        throw_in_finally(3);
    }
    CATCH(root_e)
    {
        printf("Caught root exception\n");
    }
    END_TRY

    return 0;
}
