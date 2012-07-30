#include "exceptions.h"

_exc_state_t *_exc_state_stack = NULL;
exception_t _exc_pending;

void
_exc_push_state(_exc_state_t *statep)
{
    statep->prev = _exc_state_stack;
    _exc_state_stack = statep;
}

_exc_state_t *
_exc_pop_state(void)
{
    assert(_exc_state_stack != NULL);
    _exc_state_t *p = _exc_state_stack;
    _exc_state_stack = _exc_state_stack->prev;
    return p;
}

int
_exc_issubexc(const exception_class_t *e1, const exception_class_t *e2)
{
    return (e1 == e2) || (e1 != NULL && _exc_issubexc(e1->parent, e2));
}

