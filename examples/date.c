#define RH_IMPLEMENTATION
#include "../rainbow-hig.h"

typedef struct {
    char *date;
    char *datefile;
    char *format;
    bool utc;
} MyArgs;

int main(int argc, char **argv)
{
    // Set default values
    MyArgs myargs = {
        .utc = false,
        .date = "now",
        .datefile = "",
        .format = "",
    };

    // Define flags and args
    RHArg args[] = {
        { RHARG_HELP },
        { RHARG_VERSION },
        { "date", 'd', "STRING", rh_parser_str,   (void *) &myargs.date,     "Display time described by STRING"  },
        { "utc",  'u', "", rh_parser_bool,        (void *) &myargs.utc,      "Use Coordinated Universal Time (UTC)"  },
        { "file", 'f', "DATEFILE", rh_parser_str, (void *) &myargs.datefile, "File containing rows of date strings"  },
        { "",     0,   "[FORMAT]", rh_parser_str, (void *) &myargs.format,   "" },
        { RHARG_NULL },
    };

    RHInfo info = rh_info_constructor("print or set the system date and time", "Not me", "0.1.0", argv[0]);
    rh_args_parse(argc, argv, args, &info);

    printf("Dating...\n\n");
    printf("date: %s\n", myargs.date);
    printf("datefile: %s\n", myargs.datefile);
    printf("format: %s\n", myargs.format);
    printf("utc: %s\n", myargs.utc ? "true" : "false");
    return 0;
}
