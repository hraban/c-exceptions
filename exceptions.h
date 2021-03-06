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
        _exc_state_t _exc_state; \
        EXC_TRACE("Entering try block %p\n", (void *)&_exc_state); \
        _exc_push_state(&_exc_state); \
        if (setjmp(_exc_state.env) == 0) { \

#define CATCH(e) \
        } else if (_exc_issubexc(_exc_pending.type, &e)) { \
            _exc_good = 0; \
            /* This will throw exceptions from within CATCH back up */ \
            if (_exc_did_catch == 0) { \
                _exc_pop_state(); \
                _exc_did_catch = 1; \
            } \
            EXC_TRACE("Caught exception %s (%p) in block %p\n", #e, \
                    (void *)&e, (void *)&_exc_state); \

#define FINALLY \
        } else { \
            _exc_good = 0; \
        } \
        if (_exc_did_finally++) { \
            fprintf(stderr, "WARNING: Multiple FINALLY statements in %s:%d\n", \
                    __FILE__, __LINE__); \
        } else { \
            EXC_TRACE("Entering FINALLY in block %p\n", (void *)&_exc_state); \
            if (!_exc_did_catch) { \
                /* This will throw exceptions from within FINALLY back up */ \
                _exc_pop_state(); \
            } \
        } \
        {

#define END_TRY \
        } \
        EXC_TRACE("Leaving try block %p\n", (void *)&_exc_state); \
        if (!_exc_did_finally && !_exc_did_catch) { \
            _exc_pop_state(); \
        } \
        if (!_exc_good && !_exc_did_catch) { \
            THROW(_exc_pending); \
        } \
    }

#define THROW(e) \
    do { \
        _exc_pending = e; \
        if (_exc_state_stack == NULL) { \
            fprintf(stderr, "Unhandled exception at %s:%d\n", \
                    __FILE__, __LINE__); \
            abort(); \
        } \
        longjmp(_exc_state_stack->env, 1); \
    } while (0)

#define THROW_NEW(cls) \
    do { \
        exception_t ex = { .type = &cls }; \
        THROW(ex); \
    } while (0)

#define CREATE_EXCEPTION(p) { .parent = p }

/*** Public names ***/

typedef struct exception_class {
    const struct exception_class *parent;
} exception_class_t;

typedef struct exception {
    const exception_class_t *type;
} exception_t;

/*** Private names, for use by these macros only. ***/

typedef struct state {
    jmp_buf env;
    struct state *prev;
} _exc_state_t;

extern _exc_state_t *_exc_state_stack;
extern exception_t _exc_pending;

void _exc_push_state(_exc_state_t *statep);
_exc_state_t * _exc_pop_state(void);
int _exc_issubexc(const exception_class_t *e1, const exception_class_t *e2);

#endif

