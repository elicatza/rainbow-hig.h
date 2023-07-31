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

#define RH_INDENT_SPACES 4

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
    void (*parse)(RHOpt opt);
    void **var;
    char *hint;
} RHFlag;

typedef struct {
    char *program;
    char usage[1000];
    char options[1000];
    char *description;
    // char *epilog;
    char *author;
} RHInfo;


#define RHARG_NULL NULL, 0, 0, NULL, NULL, NULL

static inline bool rh__arg_is_null(RHFlag arg);
static inline bool rh__arg_is_subcommand(RHFlag arg);
static inline bool rh__arg_is_long(RHFlag arg);
static inline bool rh__arg_is_short(RHFlag arg);
static size_t rh__arg_len(RHFlag arg);
static size_t rh__arg_len_longest_opt(RHFlag *args);
extern size_t rh__arg_len_longest_sub(RHFlag *args);

extern RHInfo rh_info_constructor(char *description, char *author, char *program);

extern void rh__gen_info_usage(RHInfo *info);
extern void rh__gen_info_options(RHInfo *info, RHFlag *args);
extern void rh__gen_info(RHInfo *info, RHFlag *args);

extern char *rh_args_shift(int *argc, char ***argv);
extern void rh_args_parse(int argc, char **argv, RHFlag *args, RHInfo *info);

extern void rh_parser_str(RHOpt opt);
extern void rh_parser_bool(RHOpt opt);

extern void rh_action_help(RHOpt opt);


#ifdef RH_IMPLEMENTATION

#endif // RH_IMPLEMENTATION

// Populates info help, and usage
// Parse argv
extern void rh_args_parse(int argc, char **argv, RHFlag *args, RHInfo *info)
{
    rh__gen_info(info, args);

    size_t i;
    char *ip;
    do {
        char *arg = rh_args_shift(&argc, &argv);
        for (i = 0; !rh__arg_is_null(args[i]); ++i) {
            RHOpt opt = {
                .argc = &argc,
                .argv = &argv,
                .optval = arg,
                .flag_type = args[i].argtype,
                .var = args[i].var,
            };

            // Short flag
            if (arg[0] == '-' && arg[1] != '-') {
                if (!rh__arg_is_short(args[i])) continue;
                for (ip = arg + 1; *ip != '\0'; ++ip) {
                    if (*ip == args[i].shortarg) {
                        args[i].parse(opt);
                    } // TODO: else error message (invalid flag)
                }
                break;
            }

            // Long flag
            if (arg[0] == '-' && arg[1] == '-') {
                if (!rh__arg_is_long(args[i])) continue;
                if (strcmp(arg + 2, args[i].longarg) == 0) {
                    args[i].parse(opt);
                    break;
                }
                continue;
            }

            // Sub parser
            if (rh__arg_is_subcommand(args[i])) {
                if (!strcmp(arg, args[i].longarg) || (strlen(arg) == 1 && arg[0] == args[i].shortarg)) {
                    rh_args_parse(argc, argv, (RHFlag*) args[i].var, info);
                }
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

extern RHInfo rh_info_constructor(char *description, char *author, char *program)
{
    return (RHInfo) {
        .description = description,
        .author = author,
        .options = "",
        .program = program,
        .usage = "",
    };
}

static inline bool rh__arg_is_null(RHFlag arg)
{
    if (arg.longarg == NULL &&
            arg.shortarg == 0 &&
            arg.argtype == 0 &&
            arg.parse == NULL &&
            arg.var == NULL &&
            arg.hint == NULL) {
        return true;
    }
    return false;
}

static inline bool rh__arg_is_subcommand(RHFlag arg)
{
    if (arg.parse == NULL && arg.var != NULL) {
        return true;
    }
    return false;
}

static inline bool rh__arg_is_long(RHFlag arg)
{
    if (arg.longarg == NULL) return false;
    if (strcmp(arg.longarg, "") == 0) return false;
    return true;
}

static inline bool rh__arg_is_short(RHFlag arg)
{
    if (arg.shortarg == 0) {
        return false;
    }
    return true;
}

static size_t rh__arg_len(RHFlag arg)
{
    size_t len = 0;
    if (rh__arg_is_short(arg) && rh__arg_is_long(arg)) len += 2;

    if (rh__arg_is_subcommand(arg)) {
        if (rh__arg_is_short(arg)) len += 1;
        if (rh__arg_is_long(arg)) len += strlen(arg.longarg);
    } else {
        if (rh__arg_is_short(arg)) len += 2;
        if (rh__arg_is_long(arg)) len += strlen(arg.longarg) + 2;
    }

    return len;
}

extern size_t rh__arg_len_longest_opt(RHFlag *args)
{
    size_t i;
    size_t max_len = 0;
    for (i = 0; !rh__arg_is_null(args[i]); ++i) {
        if (rh__arg_is_subcommand(args[i])) continue;
        size_t tmp_len = rh__arg_len(args[i]);
        if (tmp_len > max_len) {
            max_len = tmp_len;
        }
    }
    return max_len;
}

extern size_t rh__arg_len_longest_sub(RHFlag *args)
{
    size_t i;
    size_t max_len = 0;
    for (i = 0; !rh__arg_is_null(args[i]); ++i) {
        if (!rh__arg_is_subcommand(args[i])) continue;
        size_t tmp_len = rh__arg_len(args[i]);
        if (tmp_len > max_len) {
            max_len = tmp_len;
        }
    }
    return max_len;
}

extern void rh__gen_info_usage(RHInfo *info)
{
    int rv = snprintf(info->usage, 1000,
            "\x1b[35mUSAGE:\x1b[0m\n%*s%s [options]\n",
            RH_INDENT_SPACES, "",
            info->program);
    RH_ASSERT(rv >= 0);
    RH_ASSERT(rv != 1000);
}

extern void rh__gen_info_options(RHInfo *info, RHFlag *args)
{
    char buf[1000];
    size_t i;
    int rv;

    int longestopt = rh__arg_len_longest_opt(args);
    if (longestopt != 0) {
        strcpy(info->options, "\x1b[35mOPTIONS:\x1b[0m\n");
        for (i = 0; !rh__arg_is_null(args[i]); ++i) {
            if (rh__arg_is_subcommand(args[i])) continue;

            int optlen = rh__arg_len(args[i]);

            char *fmt;
            if (rh__arg_is_short(args[i]) && rh__arg_is_long(args[i])) {
                fmt = "%*s\x1b[34m-%c, --%s\x1b[0m%*s%s\n";

            } else if (rh__arg_is_short(args[i])) {
                fmt = "%*s\x1b[34m-%c%s\x1b[0m%*s%s\n";


            } else if (rh__arg_is_long(args[i])) {
                fmt = "%*s\x1b[34m%c--%s\x1b[0m%*s%s\n";
            }

            rv = snprintf(buf, 1000,
                    fmt,
                    RH_INDENT_SPACES, "",
                    args[i].shortarg, args[i].longarg,
                    longestopt - optlen + RH_INDENT_SPACES, "",
                    args[i].hint);
            RH_ASSERT(rv >= 0 && rv != 1000);
            strcat(info->options, buf);
        }
    }

    int longestsub = rh__arg_len_longest_sub(args);
    if (longestsub != 0) {
        strcat(info->options, "\n\x1b[35mSUBCOMMANDS:\x1b[0m\n");
        for (i = 0; !rh__arg_is_null(args[i]); ++i) {
            if (!rh__arg_is_subcommand(args[i])) continue;

            int sublen = rh__arg_len(args[i]);
            rv = snprintf(buf, 1000,
                    "%*s\x1b[34m%s\x1b[0m%*s%s\n",
                    RH_INDENT_SPACES, "",
                    args[i].longarg,
                    longestsub - sublen + RH_INDENT_SPACES, "",
                    args[i].hint);
            RH_ASSERT(rv >= 0 && rv != 1000);
            strcat(info->options, buf);
        }
    }

}
extern void rh__gen_info(RHInfo *info, RHFlag *args)
{
    rh__gen_info_usage(info);
    // (void) args;
    rh__gen_info_options(info, args);
}


#endif // RH_H
