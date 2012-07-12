#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

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

extern state_t *state_stack;
extern exception_t exnum;

void push_state(state_t *statep);
state_t * pop_state(void);

#endif

