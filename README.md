# 42 Exam04
## _Microshell, like minishell but micro_
```
Assignment name  : microshell
Expected files   : *.c *.h
Allowed functions: malloc, free, write, close, fork, waitpid, signal, kill, exit, chdir, execve, dup, dup2, pipe, strcmp, strncmp
--------------------------------------------------------------------------------------

Write a program that will behave like executing a shell command
- The command line to execute will be the arguments of this program
- Executable's path will be absolute or relative but your program must not build a path (from the PATH variable for example)
- You must implement "|" and ";" like in bash
	- we will never try a "|" immediately followed or preceded by nothing or "|" or ";"
- Your program must implement the built-in command cd only with a path as argument (no '-' or without parameters)
	- if cd has the wrong number of argument your program should print in STDERR "error: cd: bad arguments" followed by a '\n'
	- if cd failed your program should print in STDERR "error: cd: cannot change directory to path_to_change" followed by a '\n' with path_to_change replaced by the argument to cd
	- a cd command will never be immediately followed or preceded by a "|"
- You don't need to manage any type of wildcards (*, ~ etc...)
- You don't need to manage environment variables ($BLA ...)
- If a system call, except execve and chdir, returns an error your program should immediatly print "error: fatal" in STDERR followed by a '\n' and the program should exit
- If execve failed you should print "error: cannot execute executable_that_failed" in STDERR followed by a '\n' with executable_that_failed replaced with the path of the failed executable (It should be the first argument of execve)
- Your program should be able to manage more than hundreds of "|" even if we limit the number of "open files" to less than 30.

for example this should work:

$>./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell
microshell
i love my microshell
$>

```
Hints:
Don't forget to pass the environment variable to execve

Hints:
Do not leak file descriptors!
---
## IT DOES NO ALLOCATE MEMORY

This program does not requiere any type of parsing, so you dont really need to allocate memory as you can just work with **argv**.
The main function of this program is [execve][exec] (man 2 execve). The second argument passed to this argument has to be a NULL 
terminated string, so if you were to change "|" or ";" **execve** wouldn't read any further.
Example:
- `./microshell /bin/ls -la "|" /usr/bin/wc -l "|" /usr/bin/bc ";" /bin/ls "|" /usr/bin/grep micro`

In this case you would: 
```C
    ...
    argv[4] = NULL;
    execve(argv[1], &argv[1], env);
    ...
```
By doing this you dont have to allocate memory nor parse info, you just have to NULL strings.

## FORKING

When a program is executed by execve at the end it exits, so the process thats running it's killed. To avoid our main program dying 
we create subproceses that execute this programs, these are called child processes. To do this we use [fork();][fork] it return an 
int, if it's value is 0 then we are in the child process, else we are in the main (father) process. When the child process is created
it copies all the memory from the main one, and both processes keep running, so be carefull with this as the father has to wait for the
child process to end before continuing.

## PIPES

For command [IO][io] redirection [dup2(fd1, fd2);][dup] (man dup2) is used, [dup(fd);][dup] does also work but I prefer using dup2.  
We have to declare a int fd[2][2], so we will use 2 pipes and dump the info between them. This can also be done with just one pipe 
but I don't know how that works. To do this we use [pipe][pipe] (man 2 pipe).
Before starting to work in this examn I'd really recomend you to understand how this commands work.

The pipe function creates and links two file descriptors, one becomes the reading end and the other one the writting one

![image](https://media.geeksforgeeks.org/wp-content/uploads/Process.jpg)

Once we've created the pipes we have to redirect the standard input/output, I'd recomend you reading about them a little bit. Then
using dup2 we should be able to complete that task. Beware of closing fds!

## CD BUILTING

It is a simple builting to program, the function that changes directory is [chdir][chdir] (man 2 chdir). This function takes the path 
to the directory you want to switch to and if possible changes directory otherwise it return -1, really simple. The one thing you really 
have to take in mind is that if you were to execute this in a child proccess you wouldn't change the main proccess's directory, you would 
just change the child proccess's.


## License

SUCK MY ASS

[//]: #

   [exec]: <https://man7.org/linux/man-pages/man2/execve.2.html>
   [io]: <https://en.wikipedia.org/wiki/Input/output>
   [dup]: <https://man7.org/linux/man-pages/man2/dup.2.html>
   [pipe]: <https://man7.org/linux/man-pages/man2/pipe.2.html>
   [chdir]: <https://man7.org/linux/man-pages/man2/chdir.2.html>
   [fork]: <https://man7.org/linux/man-pages/man2/fork.2.html>
