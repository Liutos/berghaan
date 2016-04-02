#include "utils.h"

#include <string.h>

bool
utils_str_equal(const char *s1, const char *s2)
{
    if (strcmp(s1, s2) == 0)
        return true;
    return false;
}
