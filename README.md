# Basic shell

Basic shell with job control. It offers the most basic functionality of a linux shell, a program that allows the execution of processes. Processes can be interrupted, continued, and executed on the background.

:pushpin: [Summary explanation](https://onestepcode.com/writing-basic-shell/).

## Compilation

Clone the repo and launch makefile.

```
$ make
```

Alternatively, compile as:

```
$ gcc -o shell -Wall src/main.c src/signals.c src/jobs.c src/wrappers.c
```

An executable `shell` will be generated.

## Features

- Runs executables in the current working directory or the specified path.
- Runs one process in the foreground or several in the background.
- The foreground job can be terminated by typing _Ctrl+C_ and stopped by typing _Ctrl+Z_.
- Built-in commands: `fg`, `bg`, `jobs`, and `quit`.
  - `fg <id>`: send the process specified by `<id>` to the foreground.
  - `bg <id>`: send the process specified by `<id>` to the background.
  - `jobs`: displays a list of currently active jobs.
  - `quit`: terminate the shell.
- Processes can be identified by a process id (PID) or a job id (JID) preceded by `%`.

```
> bg %1      // Resume job with JID=1
> bg 14501   // Resume job with PID=14501
```

- Exit shell by pressing _Ctrl+D_ or typing `quit`.

## Example shell session

For this demo, `wait` and `write` are dummy executables present on the current working directory that run in an infinite loop. The `^C` and `^Z` characters below are the result of typing _Ctrl+C_ and _Ctrl+Z_.

```
$ ./shell
> ls
ls: Command not found.
> /bin/ls
jobs.c	 jobs.h   main.c   Makefile   README.md  signals.c   signals.h	wrappers.c   wrappers.h   write
jobs.c~  jobs.h~  main.c~  Makefile~  shell	 signals.c~  wait	wrappers.c~  wrappers.h~
> wait &
[1] 17715			wait &
> wait
^Z
Job [2] 17716 stopped by signal: Stopped
> fg %2
^C
Job [2] 17716 terminated by signal: Interrupt
> write
1...
1...
1...
1...
^C
Job [2] 17718 terminated by signal: Interrupt
> wait &
[2] 17719			wait &
> jobs
[1] 17715 Running		wait  &
[2] 17719 Running		wait  &
> quit
```

## Sources

- Core read-eval loop taken from [CS:APP](http://csapp.cs.cmu.edu/).
- Helper wrapper functions taken from [`csapp.c`](http://csapp.cs.cmu.edu/3e/ics3/code/src/csapp.c]).

## License

MIT
