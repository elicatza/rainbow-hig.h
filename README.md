# Rainbow High
Rainbow High a header only command line flag parser library.

## Goal
Make flags in C gay. I've seen other command line arg
parsers written in c, but none apealing to the man lovers.

## Features
- No memory management
- Generate help
- Generate mdoc
- One file
- Simple interface

## Naming
Rainbow High is a song from Evita. I could not find a better name that
is also the title of a musical theather song.

## Roadmap
- [X] Basic functionality
  - [X] Subcommands
  - [X] Flags
  - [X] Arguments
- [ ] Errors
- [X] Help page
- [ ] Generate mdoc section OPTIONS
- [ ] Reading stdin: `-`
- [ ] Terminate options: `--`
- [ ] Examples
  - [ ] git
  - [X] date
- [ ] Support --longarg=<var>
- [ ] Parsers
    - [X] String
    - [X] Boolean
    - [ ] Increment
    - [ ] FILE
    - [ ] Float
    - [ ] Double
    - [ ] size_t
    - [ ] Integers
    - [ ] Char
- [ ] Required arguments
- [ ] Follow [IEEE](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap12.html)
- [ ] Comply with [No color](https://bixense.com/clicolors/)
