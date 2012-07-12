#include "exceptions.h"

state_t *state_stack = NULL;
exception_t exnum;

void
push_state(state_t *statep)
{
    statep->prev = state_stack;
    state_stack = statep;
}

state_t *
pop_state(void)
{
    assert(state_stack != NULL);
    state_t *p = state_stack;
    state_stack = state_stack->prev;
    return p;
}

