/* 
 * File:   util.c
 * Author: Jani Juhani Sinervo
 *
 * Created on 17 December 2020, 21:4
 */

#include "util.h"
#include <ctype.h>
#include <stdlib.h>

const char *iterate_args(char **pos,
        char **next_pos,
        const char *arg_list_end)
{
    // Are we at the end of the argument list?
    if (*pos >= arg_list_end) {
        // If so, scram.
        return NULL;
    }
    
    // Otherwise skip any excess whitespace.
    char *iter = *pos;
    while (isspace(*iter)) {
        ++iter;
    }
    *pos = iter;
    
    // And iterate until next whitespace or NUL
    while (!(*iter == '\0' || isspace(*iter))) {
        ++iter;
    }
    
    // We've reached the end of the current argument.
    // If it's a space, we can replace it with a NUL byte.
    if (isspace(*iter)) {
        *iter = '\0';
    }
    *next_pos = iter + 1;
    // Otherwise, since it's not a space it has to be the existing NUL-byte
    // and thus the end of this string.
    return *pos;
}