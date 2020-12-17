/* 
 * File:   command.c
 * Author: Jani Juhani Sinervo
 *
 * Created on 17 December 2020, 22:40
 */

#include "command.h"
#include <string.h>

command *commands[] = {
    NULL,
};

bool command_match_name(const command *cmd, const char *name)
{
    return strcasecmp(cmd->name, name) == 0;
}