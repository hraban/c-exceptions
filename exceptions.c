#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

#if TRACE_EXCEPTIONS
#  define EXC_TRACE(...) fprintf(stderr, __VA_ARGS__)
#else
#  define EXC_TRACE(...) ((void) 0)
#endif

#define BEGIN_TRY \
    { \
        int _exc_good = 1; \
        int _exc_did_finally = 0; \
        int _exc_did_catch = 0; \
        state_t _exc_state; \
        EXC_TRACE("Entering try block %p\n", (void *)&_exc_state); \
        push_state(&_exc_state); \
        if (!setjmp(_exc_state.env) || (_exc_good = 0)) {

#define CATCH(e) \
        } else if (exnum == e && (_exc_did_catch = 1)) { \
            EXC_TRACE("Caught exception %u in block %p\n", exnum, \
                    (void *)&_exc_state); \

#define FINALLY \
        } \
        if (_exc_did_finally++) { \
            fprintf(stderr, "WARNING: Multiple FINALLY statements in " \
                    __FILE__ ":%d", __LINE__); \
        } else { \
            EXC_TRACE("Entering FINALLY in block %p\n", (void *)&_exc_state); \
            /* This will throw exceptions from within FINALLY back up */ \
            pop_state(); \
        } \
        {

#define END_TRY \
        EXC_TRACE("Leaving try block %p\n", (void *)&_exc_state); \
        } \
        if (!_exc_did_finally) { \
            pop_state(); \
        } \
        if (!_exc_good && !_exc_did_catch) { \
            THROW(exnum); \
        } \
    }

#define THROW(e) \
    do { \
        exnum = e; \
        if (state_stack == NULL) { \
            fprintf(stderr, "Unhandled exception: %u\n", e); \
            abort(); \
        } \
        longjmp(state_stack->env, 1); \
    } while (0)

typedef struct state {
    jmp_buf env;
    struct state *prev;
} state_t;

typedef enum
{
    E1,
    E2,
    E_PI,
} exception_t;

/* TODO: Create default exception handlers. */
static state_t *state_stack = NULL;

static exception_t exnum;

static void
push_state(state_t *statep)
{
    statep->prev = state_stack;
    state_stack = statep;
}

static state_t *
pop_state(void)
{
    assert(state_stack != NULL);
    state_t *p = state_stack;
    state_stack = state_stack->prev;
    return p;
}

/*********************/

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
