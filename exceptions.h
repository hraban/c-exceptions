#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <assert.h>
#include <setjmp.h>
#include <stdint.h>
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
        if (setjmp(_exc_state.env) == 0) { \

#define CATCH(e) \
        } else if (issubexc(exnum.type, &e)) { \
            _exc_good = 0; \
            _exc_did_catch = 1; \
            EXC_TRACE("Caught exception %p in block %p\n", (void *)&e, \
                    (void *)&_exc_state); \

#define FINALLY \
        } else { \
            _exc_good = 0; \
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
            fprintf(stderr, "Unhandled exception at %s:%d\n", \
                    __FILE__, __LINE__); \
            abort(); \
        } \
        longjmp(state_stack->env, 1); \
    } while (0)

#define THROW_NEW(cls) \
    do { \
        exception_t ex = { .type = &cls }; \
        THROW(ex); \
    } while (0)

#define CREATE_EXCEPTION(p) { .parent = p }

typedef struct state {
    jmp_buf env;
    struct state *prev;
} state_t;

typedef struct exception_class {
    const struct exception_class *parent;
} exception_class_t;

typedef struct exception {
    const exception_class_t *type;
} exception_t;

extern state_t *state_stack;
extern exception_t exnum;

void push_state(state_t *statep);
state_t * pop_state(void);
int issubexc(const exception_class_t *e1, const exception_class_t *e2);

#endif

