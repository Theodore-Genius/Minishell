# Minishell

**Author:** Michael Lamaze 

## Overview

A functional Unix shell implemented in C. Supports built in commands, external program execution with `fork()`/`execvp()`, job control through `waitpid()`, and effective handling of `SIGINT` so the shell remains running after Ctrl+C. The shell provides a colorized prompt that displays the current working directory and supports built-in commands such as `cd`, `exit`, `pwd`, `lf`, and `lp`.

## Features

- **Built-in Commands:**  
`cd` — change directory (supports absolute, relative, and `~` for home)  
`pwd` — print current working directory  
`exit` — exit the shell  
`lf` — list files in current directory (skips `.` and `..`)  
`lp` — list running processes by reading `/proc/*/cmdline`  

- **External Program Execution:**  
Executes external programs using `fork()` and `execvp()`    
Supports up to 10 command-line arguments for external commands  

- **Signal Handling:**  
Custom `SIGINT` handler prevents the shell from terminating when the user presses Ctrl+C; input is interrupted and the prompt gets displayed again   

- **Robust I/O and Parsing:**  
Strips trailing newlines and ignores empty input lines    
Limits tokenized arguments to 10 to avoid over-run of the argument buffer    
Provides basic error messages for invalid commands, `cd` misuse, and failed system calls  

## Compilation & Execution
To compile the minishell, use:
```
gcc minishell.c
```
Then run the shell using:
```
./a.out
```
