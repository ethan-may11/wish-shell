# wish — A Unix Shell

A custom Unix shell written in C, built for an Operating Systems course. `wish` (the Wisconsin Shell) implements core shell behavior from scratch using POSIX system calls: process creation, program execution, I/O redirection, and parallel command execution.

> Course project based on the `wish` assignment from *Operating Systems: Three Easy Pieces* (Arpaci-Dusseau). The implementation is my own.

## Features
- **Interactive and batch modes**  runs an interactive `wish>` prompt, or executes commands from a script file passed as an argument.
- **Built-in commands**  `cd`, `exit`, and `path` (sets the search path used to locate executables).
- **External commands**  resolved against the configurable search path and executed via `fork()` / `execv()`.
- **Output redirection**  `command > file` sends stdout and stderr to a file.
- **Parallel execution**  `cmd1 & cmd2 & cmd3` launches commands concurrently and waits for all of them to finish.
- **Error handling**  consistent reporting for malformed input, bad redirection, and unknown commands.

## Build
```
make
```
Or compile directly:
```
gcc -Wall -Werror -o wish main.c wish.c
```

## Run
Interactive mode:
```
./wish
```
Batch mode (commands read from a file):
```
./wish script.txt
```

## Example
```
wish> path /bin /usr/bin
wish> ls -la > out.txt
wish> cd projects
wish> ls & pwd & whoami
wish> exit
```

## Concepts
Process creation and management (`fork`, `execv`, `waitpid`), file-descriptor redirection (`open`, `dup2`), executable path resolution (`access`), and manual string tokenization/parsing in C.

## Files
- `main.c`  command parsing, built-ins, redirection handling, and the main read–eval loop
- `wish.c`  external command resolution and execution (`run_external`)
- `wish.h`  shared declarations
- `Makefile`  build configuration
