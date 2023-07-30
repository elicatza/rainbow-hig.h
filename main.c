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
    Args args_buf = args_init();

    // Warning no type checking for var. Could result in unexpected behaviour
    RHFlag sub_args[] = {
        { "foo", 'f', RH_ARG_OPTIONAL, rh_parser_bool, (void *) &args_buf.help },
        { RHARG_NULL }
    };

    // Use null values to denote end
    RHFlag args[] = {
        { "help", 'h', RH_ARG_OPTIONAL, rh_parser_bool, (void *) &args_buf.help },
        { "str",  's', RH_ARG_OPTIONAL, rh_parser_str,  (void *) &args_buf.str  },
        { "bar",  'b', RH_ARG_OPTIONAL, rh_parser_str,  (void *) &args_buf.str  },
        { "rec",  'r', RH_ARG_OPTIONAL, NULL,           (void *) &sub_args },
        { RHARG_NULL }
    };

    rh_args_parse(argc, argv, args);
    printf("\n");
    printf("Help: %s\n", args_buf.help ? "true" : "false");
    printf(" Str: %s\n", (char *) args_buf.str);

    return 0;
}

