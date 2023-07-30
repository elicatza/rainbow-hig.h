#ifndef RH_H
#define RH_H

#ifndef RH_ASSERT
#include <assert.h>
#define RH_ASSERT(s) assert(s)
#endif // RH_ASSERT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum {
    RH_ARG_REQUIRED = 1<<1,
    RH_ARG_OPTIONAL = 1<<2,
} RHFlagType;

typedef struct {
    int *argc;
    char ***argv;
    void **var;
    char *opt;
    int flag_type;
} RHOpt;


typedef struct {
    char *longarg;
    char shortarg;
    RHFlagType argtype;
    void (*parse)(RHOpt opt);
    void **var;
} RHFlag;

/*
typedef struct {
    char *help;
    char *usage;
} RHFoo;
*/

#define RHARG_NULL NULL, 0, 0, NULL, NULL

static inline bool rh__arg_is_null(RHFlag arg);

extern char *rh_args_shift(int *argc, char ***argv);
extern void rh_args_parse(int argc, char **argv, RHFlag *args);

extern void rh_parser_str(RHOpt opt);
extern void rh_parser_bool(RHOpt opt);
extern void rh_action_help(RHOpt opt);


#ifdef RH_IMPLEMENTATION

#endif // RH_IMPLEMENTATION

extern void rh_args_parse(int argc, char **argv, RHFlag *args)
{
    size_t i;
    // char *l, *m, *n, *o, *p, *q, *r, *s;
    char *ip;
    do {
        char *arg = rh_args_shift(&argc, &argv);
        for (i = 0; !rh__arg_is_null(args[i]); ++i) {
            RHOpt opt = {
                .argc = &argc,
                .argv = &argv,
                .opt = arg,
                .flag_type = args[i].argtype,
                .var = args[i].var,
            };

            // Short flag
            if (arg[0] == '-' && arg[1] != '-') {
                printf("Short flag: ");
                for (ip = arg + 1; *ip != '\0'; ++ip) {
                    printf("%c", *ip);
                    if (*ip == args[i].shortarg) {
                        args[i].parse(opt);
                    } // TODO: else error message (invalid flag)
                }
                printf("\n");
                break;
            }

            // Long flag
            if (arg[0] == '-' && arg[1] == '-') {
                if (strcmp(arg + 2, args[i].longarg) == 0) {
                    printf("Long flag: %s\n", args[i].longarg);
                    args[i].parse(opt);
                    printf("outstr: %s\n", (char *) *args[i].var);
                    break;
                }
                continue;
            }

            // Sub parser
            if (!strcmp(arg, args[i].longarg) || (strlen(arg) == 1 && arg[0] == args[i].shortarg)) {
                rh_args_parse(argc, argv, (RHFlag*) args[i].var);
            }
        }
    } while (argc > 0);
}

extern void rh_parser_str(RHOpt opt)
{
    *(char **) opt.var = rh_args_shift(opt.argc, opt.argv);
}

extern void rh_parser_bool(RHOpt opt)
{
    *(bool *) opt.var = true;
}

extern void rh_action_help(RHOpt opt)
{
    (void) opt;
}

extern char *rh_args_shift(int *argc, char ***argv)
{
    RH_ASSERT(*argc > 0);
    char *rt = **argv;
    *argc -= 1;
    *argv += 1;
    return rt;
}

static inline bool rh__arg_is_null(RHFlag arg)
{
    if (arg.longarg == NULL &&
            arg.shortarg == 0 &&
            arg.argtype == 0 &&
            arg.parse == NULL &&
            arg.var == NULL) {
        return true;
    }
    return false;
}

#endif // RH_H
