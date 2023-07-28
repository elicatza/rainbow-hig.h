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
    RH_ARG_REQUIRED,
    RH_ARG_OPTIONAL,
} RHArgType;

typedef struct {
    char *longarg;
    char *shortarg;
    RHArgType argtype;
    void (*parse)(int *argc, char ***argv, void **var);
    void **var;
} RHArgs;

static char *rh_args_shift(int *argc, char ***argv);
extern void rh_args_parse(int argc, char **argv, RHArgs *args);

extern void rh_parser_str(int *argc, char ***argv, void **var);
extern void rh_parser_bool(int *argc, char ***argv, void **var);

#ifdef RH_IMPLEMENTATION

#endif // RH_IMPLEMENTATION

extern void rh_args_parse(int argc, char **argv, RHArgs *args)
{
    // TODO: ensure unique flags
    printf("size out: %zu\n", sizeof *args);
    RHArgs *inc_args;
    do {
        char *arg = rh_args_shift(&argc, &argv);
        for (inc_args = args; inc_args->longarg != 0; ++inc_args) {
            if (strcmp(arg, "--") == 1) {
                printf("Short flag\n");
            }

            if (!strcmp(arg, inc_args->longarg) || !strcmp(arg, inc_args->shortarg)) {
                printf("Found a match: %s\n", inc_args->longarg);
                inc_args->parse(&argc, &argv, inc_args->var);
            }
        }
    } while (argc > 0);
}

extern void rh_parser_str(int *argc, char ***argv, void **var)
{
    // TODO: Global variable or option for size
    char buf[0x100];
    char *tmp = rh_args_shift(argc, argv);
    strncpy(buf, tmp, 0x100);
    *var = buf;
}

extern void rh_parser_bool(int *argc, char ***argv, void **var)
{
    (void) argc;
    (void) argv;
    bool rt = 1;
    *var = (void*) &rt;
}


static char *rh_args_shift(int *argc, char ***argv)
{
    RH_ASSERT(*argc > 0);
    char *rt = **argv;
    *argc -= 1;
    *argv += 1;
    return rt;
}

#endif // RH_H
