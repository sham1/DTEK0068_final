/*
 * File:   help-command.h
 * Author: Jani Juhani Sinervo
 *
 * Created on 18 December 2020, 09:17
 */

#include "help-command.h"
#include "util.h"

static void help_command_init(void);
static bool help_command_execute(char *arglist, const char *arglist_end);
static void help_command_print_help_text(void);

const command help_cmd = {
    .name = "HELP",
    .short_help_blurb = "Displays help for commands",

    .init = &help_command_init,
    .execute = &help_command_execute,
    .print_help_text = &help_command_print_help_text,
};

static void help_command_init(void)
{
}

static bool help_command_execute(char *arglist, const char *arglist_end)
{
    char *arg = arglist;
    // If we got arguments, let's check if they match a command
    if (iterate_args(&arg, &arglist, arglist_end)) {
        bool found_command = false;
        for (const command **cmd = commands; *cmd != NULL; ++cmd) {
            if (command_match_name(*cmd, arg)) {
                const command *c = *cmd;
                printf("Available %s commands:\r\n", c->name);

                c->print_help_text();

                found_command = true;
                break;
            }
        }

        if (!found_command) {
            printf("HELP: No such command: %s\r\n", arg);
        }

        return found_command;
    } else {
        printf("Available commands:\r\n");
        for (const command **cmd = commands; *cmd != NULL; ++cmd) {
            const command *c = *cmd;
            printf("\t%s\t%s\r\n", c->name, c->short_help_blurb);
        }
    }
    return true;
}

static void help_command_print_help_text(void)
{
    printf("\tHELP\tPrint a summary of available commands\r\n");
    printf("\tHELP <command>\tShow help for given command\r\n");
}
