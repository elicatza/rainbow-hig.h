//       --explain <CODE>      Run `rustc --explain CODE`
//   -v, --verbose...          Use verbose output (-vv very verbose/build.rs output)
//   -q, --quiet               Do not print cargo log messages
//       --color <WHEN>        Coloring: auto, always, never
//   -C <DIRECTORY>            Change to DIRECTORY before doing anything (nightly-only)
//       --frozen              Require Cargo.lock and cache are up to date
//       --locked              Require Cargo.lock is up to date
//       --offline             Run without accessing the network
//       --config <KEY=VALUE>  Override a configuration value
//   -Z <FLAG>                 Unstable (nightly-only) flags to Cargo, see 'cargo -Z help' for details
// 
// Some common cargo commands are (see all commands with --list):
//     build, b    Compile the current package
//     check, c    Analyze the current package and report errors, but don't build object files
//     clean       Remove the target directory
//     doc, d      Build this package's and its dependencies' documentation
//     new         Create a new cargo package
//     init        Create a new cargo package in an existing directory
//     add         Add dependencies to a manifest file
//     remove      Remove dependencies from a manifest file
//     run, r      Run a binary or example of the local package
//     test, t     Run the tests
//     bench       Run the benchmarks
//     update      Update dependencies listed in Cargo.lock
//     search      Search registry for crates
//     publish     Package and upload this package to the registry
//     install     Install a Rust binary. Default location is $HOME/.cargo/bin
//     uninstall   Uninstall a Rust binary

#include <string.h>

#define RH_IMPLEMENTATION
#include "../rainbow-hig.h"

void action_list(RHOpt opt, RHInfo info);

int main(int argc, char **argv)
{
    char *bin = "";
    RHArg subcomand_build[] = {
        { RHARG_HELP },
        { "bin", 0, "NAME", rh_parser_str, (void *) &bin, "Build specified binary" },
        { RHARG_NULL }
    };

    RHArg args[] = {
        { RHARG_HELP },
        { RHARG_VERSION },
        { "list", 0, "", action_list, NULL, "List musicals" },
        { "", 0, "[FILE]...", action_list, NULL, "Files" },
        { "build", 0, "", NULL, (void *) &subcomand_build, "List musicals" },
        { RHARG_NULL }
    };

    RHInfo info = rh_info_constructor("Example inspired by Cargo", "Me", "0.1.0", argv[0]);
    rh_args_parse(argc, argv, args, &info);

    if (strlen(bin) != 0) printf("Building binary: %s...\n", bin);
}

void action_list(RHOpt opt, RHInfo info)
{
    (void) opt;
    (void) info;
    printf("Company\nFollies\nA Little Night Music\nThe Frogs\nPacific Overtures\nSweeney Todd\n");
}
