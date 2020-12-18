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
#include "temp-command.h"
#include "button-command.h"
#include "led-command.h"

const command *commands[] = {
    &reset_cmd,
    &help_cmd,
    &adc_cmd,
    &vref_cmd,
    &temp_cmd,
    &button_cmd,
    &led_cmd,
    NULL,
};

bool command_match_name(const command *cmd, const char *name)
{
    return strcasecmp(cmd->name, name) == 0;
}