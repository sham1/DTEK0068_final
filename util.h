/* 
 * File:   util.h
 * Author: Jani Juhani Sinervo
 *
 * Created on 17 December 2020, 21:52
 */

#ifndef UTIL_H
#define	UTIL_H

#ifdef	__cplusplus
extern "C" {
#endif

// Iterates through a given argument position, giving each space-separated
// argument as a return value.
//
// Example:
//
// char *foo = "bar baz";
// const char *foo_end = foo + strlen(foo);
//
// char *next_arg = NULL;
// for (char *arg = foo; iterate_args(&arg, &next_arg, foo_end);) {
//     printf("Arg: %s\", arg);
//     arg = next_arg;
// }
// That example will print:
//
// Arg: bar
// Arg: baz
//
// Returns NULL after the last argument. Will modify the argument list to add
// NUL bytes after arguments.
const char *iterate_args(char **pos,
        char **next_pos,
        const char *arg_list_end);

#ifdef	__cplusplus
}
#endif

#endif	/* UTIL_H */

