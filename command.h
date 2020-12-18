/* 
 * File:   command.h
 * Author: Jani Juhani Sinervo
 *
 * Created on 17 December 2020, 22:35
 */

#ifndef COMMAND_H
#define	COMMAND_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
    
typedef struct COMMAND {
    const char *name;
    // Describes the command shortly, within the simple invocation of HELP.
    const char *short_help_blurb;
    
    void (*init)(void);
    bool (*execute)(char *arglist, const char *arglist_end);
    void (*print_help_text)(void);
} command;

bool command_match_name(const command *cmd, const char *name);

// A NULL-pointer terminated list of commands.
extern const command *commands[];

#ifdef	__cplusplus
}
#endif

#endif	/* COMMAND_H */

