/* 
 * File:   command.c
 * Author: Jani Juhani Sinervo
 *
 * Created on 17 December 2020, 22:40
 */

#include "command.h"
#include <string.h>

#include "reset-command.h"
#include "help-command.h"
#include "vref-command.h"
#include "adc-command.h"

command *commands[] = {
    &reset_cmd,
    &help_cmd,
    &adc_cmd,
    &vref_cmd,
    NULL,
};

bool command_match_name(const command *cmd, const char *name)
{
    return strcasecmp(cmd->name, name) == 0;
}