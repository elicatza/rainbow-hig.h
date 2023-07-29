#ifndef RH_H
#define RH_H


// Idea
// Define a structure like so:
// typedef struct {
//     bool help;
//     char* str;
// } Args;
//
// A function like so
// Args parseargs(int argc, char **argv);

// Or
// typedef struct {
//     char* longname;
//     char* shortname;
//     enum type;
//     void* (*function)(char *) *parse_function;
//     void *buffer;
// } Args;
// { "longargname", "shortargname", type, parse_function, *arg_buffer }
//
// A function like so
// Args parseargs(int argc, char **argv, struct *Args);

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
    unsigned int flag_type;
    char *optval;
} RHOpt;

typedef struct {
    char *longarg;
    char shortarg;
    RHFlagType argtype;
    void (*parse)(RHOpt *opt);
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

extern void rh_parser_str(RHOpt *opt);
extern void rh_parser_bool(RHOpt *opt);
extern void rh_action_help(RHOpt *opt);


#ifdef RH_IMPLEMENTATION

#endif // RH_IMPLEMENTATION

extern void rh_args_parse(int argc, char **argv, RHFlag *args)
{
    // TODO: ensure unique flags
    // TODO: check required flags
    size_t i;
    char *ip;
    do {
        char *arg = rh_args_shift(&argc, &argv);
        for (i = 0; !rh__arg_is_null(args[i]); ++i) {
            RHOpt opt = {
                .argc = &argc,
                .argv = &argv,
                .var = args[i].var,
                .optval = arg,
                .flag_type = args[i].argtype,
            };

            // Short flag
            if (arg[0] == '-' && arg[1] != '-') {
                printf("Short flag: ");
                for (ip = arg + 1; *ip != '\0'; ++ip) {
                    printf("%c", *ip);
                    if (*ip == args[i].shortarg) {
                        args[i].parse(&opt);
                    } // TODO: else error message (invalid flag)
                }
                printf("\n");
                break;
            }

            // Long flag
            if (arg[0] == '-' && arg[1] == '-') {
                if (strcmp(arg + 2, args[i].longarg) == 0) {
                    printf("Long flag: %s\n", args[i].longarg);
                    args[i].parse(&opt);
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

extern void rh_parser_str(RHOpt *opt)
{
    // TODO: Global variable or option for size
    char buf[0x100];
    opt->optval = rh_args_shift(opt->argc, opt->argv);
    strncpy(buf, opt->optval, 0x100);
    printf("instr: %s\n", opt->optval);
    printf("instr: %s\n", buf);
    *opt->var = (void *) &buf;
    printf("instr: %s\n", (char *) &opt->var);
}

extern void rh_parser_bool(RHOpt *opt)
{
    bool rt = 1;
    *opt->var = (void *) &rt;
}

extern void rh_action_help(RHOpt *opt)
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
