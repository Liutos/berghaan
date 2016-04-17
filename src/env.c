#include "env.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

bool
env_position(env_t *env, const char *variable, int *x, int *y)
{
    int _x = 0, _y = 0;
    while (env != NULL) {
        _y = assoc_list_position(env->bindings, variable);
        if (_y != -1) {
            *x = _x;
            *y = _y;
            return true;
        }
        env = env->outer;
        _x++;
    }
    return false;
}

env_t *
env_new(env_t *outer)
{
    env_t *env = calloc(1, sizeof(env_t));
    env->bindings = assoc_list_new();
    env->outer = outer;
    return env;
}

env_t *
env_bind(env_t *env, const char *variable, object_t *value)
{
    env->bindings = assoc_list_push(env->bindings, variable, value);
    return env;
}

env_t *
env_push_back(env_t *env, const char *var, object_t *val)
{
    env->bindings = assoc_list_push_back(env->bindings, var, val);
    return env;
}

object_t *
env_at(env_t *env, int x, int y)
{
    for (int i = 0; i < x; i++) {
        assert(env != NULL);
        env = env->outer;
    }
    return (object_t *)assoc_list_at(env->bindings, y);
}

object_t *
env_reference(env_t *env, const char *variable)
{
    while (env != NULL) {
        object_t *value = assoc_list_search(env->bindings, variable);
        if (value != NULL)
            return value;
        env = env->outer;
    }
    return NULL;
}

env_t *
env_set(env_t *env, int x, int y, object_t *value)
{
    while (x > 0 )
        env = env->outer;
    assoc_list_set(env->bindings, y, value);
    return env;
}
