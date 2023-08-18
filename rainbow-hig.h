#ifndef RH_H
#define RH_H

#ifndef RH_ASSERT
#include <assert.h>
#define RH_ASSERT(s) assert(s)
#endif // RH_ASSERT

#define RH_INDENT 4

#ifndef RHDEF
#ifdef RH_STATIC
#define RHDEF static
#else
#define RHDEF extern
#endif
#endif


typedef struct {
    int *argc;
    char ***argv;
    void **var;
    char *flag_type;
} RHOpt;

typedef struct {
    char *program;
    char *version;
    char usage[1000];
    char options[1000];
    char *description;
    // char *epilog;
    char *author;
} RHInfo;

typedef struct {
    char *longarg;
    char shortarg;
    char *argtype;
    void (*parse)(RHOpt opt, RHInfo info);
    void **var;
    char *hint;
} RHArg;

#define RHARG_NULL "", '\0', "", NULL, NULL, ""
#define RHARG_HELP "help", 'h', "", rh_action_help, NULL, "Prints help message and exit"
#define RHARG_VERSION "version", 'V', "", rh_action_version, NULL, "Prints version and exit"

// TODO: Make functions?
#define RHARG_FLAG(longarg, shortarg, typehint, parser, var, hint) (longarg), (shortarg), (typehint), (parser), (void *) &(var), (hint)
#define RHARG_SUB(longarg, shortarg, subcmd, hint) (longarg), (shortarg), "", NULL, (void *) &(subcmd), (hint)
#define RHARG_ARG(typehint, parser, var) "", 0, (typehint), (parser), (void *) &(var), ""

RHDEF char *rh_args_shift(int *argc, char ***argv);

// TODO: Make static?
RHDEF char *rh_args_backshift(int *argc, char ***argv);
RHDEF void rh_args_parse(int argc, char **argv, RHArg *args, RHInfo *info);
RHDEF RHInfo rh_info_constructor(char *description, char *author, char *version, char *program);

RHDEF void rh_parser_str(RHOpt opt, RHInfo info);
RHDEF void rh_parser_bool(RHOpt opt, RHInfo info);
RHDEF void rh_parser_uint(RHOpt opt, RHInfo info);

RHDEF void rh_action_help(RHOpt opt, RHInfo info);
RHDEF void rh_action_version(RHOpt opt, RHInfo info);

#ifdef RH_IMPLEMENTATION

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool rh__arg_is_null(RHArg arg)
{
    if (strlen(arg.longarg) == 0 &&
            arg.shortarg == 0 &&
            strlen(arg.argtype) == 0 &&
            arg.parse == NULL &&
            arg.var == NULL &&
            strlen(arg.hint) == 0) {
        return true;
    }
    return false;
}

static bool rh__arg_is_sub(RHArg arg)
{
    if ((arg.shortarg != 0 || strlen(arg.longarg) != 0) &&
            arg.parse == NULL &&
            arg.var != NULL &&
            strlen(arg.argtype) == 0) {
        return true;
    }
    return false;
}

static bool rh__arg_is_flag(RHArg arg)
{
    if ((arg.shortarg != 0 || strlen(arg.longarg) != 0) &&
            arg.parse != NULL) {
        return true;
    }
    return false;
}

static bool rh__arg_is_arg(RHArg arg)
{
    if ((arg.shortarg == 0 && strlen(arg.longarg) == 0) &&
            arg.parse != NULL &&
            strlen(arg.argtype) != 0 &&
            strlen(arg.hint) == 0) {
        return true;
    }
    return false;
}

static bool rh__flag_is_long(RHArg arg)
{
    if (strlen(arg.longarg) == 0) return false;
    return true;
}

static bool rh__flag_is_short(RHArg arg)
{
    if (arg.shortarg == 0) {
        return false;
    }
    return true;
}

static size_t rh__arg_len(RHArg arg)
{
    size_t len = 0;
    if (rh__flag_is_short(arg) && rh__flag_is_long(arg)) len += 2;

    if (rh__arg_is_sub(arg)) {
        if (rh__flag_is_short(arg)) len += 1;
        if (rh__flag_is_long(arg)) len += strlen(arg.longarg);
    } else {
        if (rh__flag_is_short(arg)) len += 2;
        if (rh__flag_is_long(arg)) len += strlen(arg.longarg) + 2;
    }

    if (strlen(arg.argtype) != 0) {
        len += 3;
        len += strlen(arg.argtype);
    }

    return len;
}

static size_t rh__arg_len_longest(RHArg *args, bool (*condition)(RHArg arg))
{
    size_t i;
    size_t max_len = 0;
    for (i = 0; !rh__arg_is_null(args[i]); ++i) {
        if (!condition(args[i])) continue;
        size_t tmp_len = rh__arg_len(args[i]);
        if (tmp_len > max_len) {
            max_len = tmp_len;
        }
    }
    return max_len;
}

static void rh__args_validate(RHArg *args)
{
    static unsigned call_counter;
    ++call_counter;
    size_t i;
    for (i = 0; !rh__arg_is_null(args[i]); ++i) {
        if (args[i].longarg == NULL) args[i].longarg = "";
        if (args[i].argtype == NULL) args[i].argtype = "";
        if (args[i].hint == NULL) args[i].hint = "";
        if (!rh__arg_is_arg(args[i]) && !rh__arg_is_flag(args[i]) && !rh__arg_is_sub(args[i])) {
            fprintf(stderr, "RH_ERROR: Could not classify argarray %d at row %zu\n", call_counter, i + 1);
            fprintf(stderr, "Please supply more fields to resolve this error.\n");
            exit(1);
        }
    }
}

// TODO: check if words should be plural. i.e. option(s)
static void rh__gen_info_usage(RHInfo *info, RHArg *args)
{
    size_t i;
    char argbuf[256] = { '\0' };

    bool has_sub = false, has_flag = false;
    for (i = 0; !rh__arg_is_null(args[i]); ++i) {
        if (rh__arg_is_sub(args[i])) has_sub = true;
        if (rh__arg_is_flag(args[i])) has_flag = true;
        if (!rh__arg_is_arg(args[i])) continue;
        strcat(argbuf, " ");
        strcat(argbuf, args[i].argtype);
    }

    char *options;
    if (has_flag) options = " [options]";
    else options = "";

    char *subs;
    if (has_sub) subs = " [subcommand]";
    else subs = "";


    int rv = snprintf(info->usage, 1000,
            "\x1b[35mUSAGE:\x1b[0m\n%*s%s%s%s%s\n",
            RH_INDENT, "",
            info->program,
            subs,
            options,
            argbuf);

    RH_ASSERT(rv >= 0 && rv != 1000);
    RH_ASSERT(rv != 1000);
}

static void rh__gen_info_option_line(RHArg arg, int longestopt, char *dest, size_t sz)
{
    RH_ASSERT(!rh__arg_is_arg(arg));
    int optlen = rh__arg_len(arg);
    size_t buflen = longestopt + 2 * RH_INDENT + strlen(arg.hint) + 2;  // 10 is for color and newline
    RH_ASSERT(sz >= buflen);

    char longflag[optlen + 1];
    snprintf(longflag, optlen + 1, "--%s", arg.longarg);
    char shortflag[3] = { '-', arg.shortarg, '\0' };

    size_t placeholder_len = strlen(arg.argtype) + 3;
    char placeholder[placeholder_len];
    placeholder[0] = '\0';
    if (strlen(arg.argtype) != 0) {
        snprintf(placeholder, placeholder_len + 1, " <%s>", arg.argtype);
    }

    char *seperator = ", ";
    if (rh__arg_is_sub(arg)) {
        shortflag[0] = arg.shortarg;
        shortflag[1] = '\0';
        strncpy(longflag, arg.longarg, optlen);
    }

    if (rh__flag_is_short(arg) && !rh__flag_is_long(arg)) {
        longflag[0] = '\0';
        seperator = "";
    }

    if (!rh__flag_is_short(arg) && rh__flag_is_long(arg)) {
        shortflag[0] = '\0';
        seperator = "";
    }

    int rv = snprintf(dest, 1000,
            "%*s%s%s%s%s%*s%s\n",
            RH_INDENT, "",
            shortflag, seperator, longflag,
            placeholder,
            longestopt - optlen + RH_INDENT, "",
            arg.hint);
    RH_ASSERT(rv >= 0 && rv != 1000);
}

static void rh__gen_info_options(RHInfo *info, RHArg *args)
{
    char buf[1000];
    size_t i;

    int longestopt = rh__arg_len_longest(args, rh__arg_is_flag);
    if (longestopt != 0) {
        strcpy(info->options, "\x1b[35mOPTIONS:\x1b[0m\n");
        for (i = 0; !rh__arg_is_null(args[i]); ++i) {
            if (!rh__arg_is_flag(args[i])) continue;

            rh__gen_info_option_line(args[i], longestopt, buf, 1000);
            strcat(info->options, buf);
        }
    }

    int longestsub = rh__arg_len_longest(args, rh__arg_is_sub);
    if (longestsub != 0) {
        strcat(info->options, "\n\x1b[35mSUBCOMMANDS:\x1b[0m\n");
        for (i = 0; !rh__arg_is_null(args[i]); ++i) {
            if (!rh__arg_is_sub(args[i])) continue;

            rh__gen_info_option_line(args[i], longestsub, buf, 1000);
            strcat(info->options, buf);
        }
    }

}

static void rh__gen_info(RHInfo *info, RHArg *args)
{
    rh__gen_info_usage(info, args);
    rh__gen_info_options(info, args);
}

RHDEF void rh_args_parse(int argc, char **argv, RHArg *args, RHInfo *info)
{
    size_t i;
    rh__args_validate(args);
    rh__gen_info(info, args);

    char *ip;
    do {
        char *arg = rh_args_shift(&argc, &argv);
        for (i = 0; !rh__arg_is_null(args[i]); ++i) {
            RHOpt opt = {
                .argc = &argc,
                .argv = &argv,
                .flag_type = args[i].argtype,
                .var = args[i].var,
            };

            // Short flag
            if (arg[0] == '-' && arg[1] != '-') {
                if (!rh__flag_is_short(args[i])) continue;
                if (args[i].parse == NULL) continue;

                for (ip = arg + 1; *ip != '\0'; ++ip) {
                    if (*ip == args[i].shortarg) {
                        args[i].parse(opt, *info);
                    } // TODO: else error message (invalid flag)
                }
                continue;
            }

            // Long flag
            if (arg[0] == '-' && arg[1] == '-') {
                if (!rh__flag_is_long(args[i])) continue;
                if (args[i].parse == NULL) continue;

                if (strcmp(arg + 2, args[i].longarg) == 0) {
                    args[i].parse(opt, *info);
                    break;
                }
                continue;
            }

            // Sub parser
            if (rh__arg_is_sub(args[i])) {
                if (strcmp(arg, args[i].longarg) == 0 || (strlen(arg) == 1 && arg[0] == args[i].shortarg)) {
                    rh_args_parse(argc, argv, (RHArg*) args[i].var, info);
                    break;
                }
            }

            if (rh__arg_is_arg(args[i])) {
                printf("argv: %s\n", *argv);
                printf("backshift: %s\n", rh_args_backshift(opt.argc, opt.argv));
                args[i].parse(opt, *info);
                args[i].shortarg += 1;
                break;
            }
        }
    } while (argc > 0);
}

RHDEF void rh_parser_str(RHOpt opt, RHInfo info)
{
    (void) info;
    if (*opt.var == NULL) return;
    *(char **) opt.var = rh_args_shift(opt.argc, opt.argv);
}

RHDEF void rh_parser_bool(RHOpt opt, RHInfo info)
{
    (void) info;
    if (opt.var == NULL) return;
    *(bool *) opt.var = true;
}

RHDEF void rh_parser_uint(RHOpt opt, RHInfo info)
{
    (void) info;
    if (opt.var == NULL) return;
    char *val = rh_args_shift(opt.argc, opt.argv);
    char *ptr;
    for (ptr = val; *ptr != '\0'; ++ptr) {
        if (!(*ptr >= '0' && *ptr <= '9')) {
            // TODO: Throw error
            *(unsigned int *) opt.var = 0L;
            return;
        }
    }
    *(unsigned int *) opt.var = strtoul(val, '\0', 10);
}

RHDEF void rh_action_help(RHOpt opt, RHInfo info)
{
    (void) opt;
    printf("%s\n%s", info.usage, info.options);
    exit(0);
}

RHDEF void rh_action_version(RHOpt opt, RHInfo info)
{
    (void) opt;
    printf("%s %s\n", info.program, info.version);
    exit(0);
}

RHDEF char *rh_args_shift(int *argc, char ***argv)
{
    RH_ASSERT(*argc > 0);
    char *rt = **argv;
    *argc -= 1;
    *argv += 1;
    return rt;
}

RHDEF char *rh_args_backshift(int *argc, char ***argv)
{
    char *rt = **argv;
    *argc += 1;
    *argv -= 1;
    printf("rt: %s\n", rt);
    return rt;
}

RHDEF RHInfo rh_info_constructor(char *description, char *author, char *version, char *program)
{
    return (RHInfo) {
        .author = author,
        .description = description,
        .options = "",
        .program = program,
        .usage = "",
        .version = version,
    };
}

#endif // RH_IMPLEMENTATION
#endif // RH_H

/*
The MIT License (MIT)

Copyright (c) 2023 Eliza Clausen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
