#include <stdio.h>
#include <stdbool.h>

#define RH_IMPLEMENTATION
#include "./rainbow-hig.h"


typedef struct {
    bool help;
    char *str;
} Args;

Args args_init(void)
{
    return (Args) {
        .help = false,
        .str = "",
    };
}


int main(int argc, char **argv)
{
    rh__arg_is_null((RHArg) { RHARG_NULL });
    Args args_buf = args_init();

    // Warning no type checking for var. Could result in unexpected behaviour
    RHArg sub_args[] = {
        { "help", 'h', "", rh_parser_bool, (void *) &args_buf.help, "Show this message and exit"  },
        { "foo",  'f', "", rh_parser_bool, (void *) &args_buf.help, "A boolean" },
        { RHARG_NULL }
    };

    // Use null values to denote end
    // Instead of flags use: [String],
    // Multiple args: <file>...Required
    RHArg args[] = {
        { "help",     'h', "",          rh_parser_bool, (void *) &args_buf.help, "Show this message and exit" },
        { "str",      's', "[NAME]",    rh_parser_str,  (void *) &args_buf.str,  "Enter a string"  },
        { "bar",       0,  "[FILE...]", rh_parser_str,  (void *) &args_buf.str,  "Another string"  },
        { "rec",      'r', "",          NULL,           (void *) &sub_args,      "Recursive subcommand" },
        { "debuging", 'd', "",          NULL,           (void *) &sub_args,      "Another recursive subcommand" },
        { RHARG_NULL }
    };

    RHInfo info = rh_info_constructor("Rainbow high like the song, but a program!", "0.1.0", "Me", argv[0]);

    rh_args_parse(argc, argv, args, &info);
    printf("%s\n%s\n", info.usage, info.options);
    printf("Help: %s\n", args_buf.help ? "true" : "false");
    printf(" Str: %s\n", (char *) args_buf.str);

    return 0;
}

