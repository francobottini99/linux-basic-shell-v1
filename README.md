# MyShell - Command Line Interpreter

This project implements a command line interpreter called MyShell in the C programming language, designed to run in GNU/Linux environments. The primary goal is to provide an interactive shell with basic functionalities and the ability to execute internal commands, external programs, and background scripts.

### Author:
- **Bottini, Franco Nicolas**

## Main Features

### 1. Command Line Prompt
MyShell displays a prompt indicating the username, hostname, and current directory. The format is as follows:

```
username@hostname:~$
```

### 2. Internal Commands
MyShell supports several internal commands:

- **cd \<directory\>**: Changes the current directory to \<directory\>. If \<directory\> is not specified, it reports the current directory. If the directory does not exist, an error message is displayed. Additionally, this command updates the PWD environment variable. The command also supports the `cd -` option, which returns to the last working directory (OLDPWD).

- **clr**: Clears the screen.

- **echo \<comment\|env var\>**: Displays \<comment\> on the screen followed by a newline. Multiple spaces/tabs are reduced to a single space.

- **quit**: Exits MyShell.

### 3. Program Invocation
User input that is not an internal command is interpreted as a program invocation. Execution is performed using `fork` and `execl`. MyShell supports both relative and absolute paths.

### 4. Batch File
MyShell can execute commands from a file when invoked with an argument. For example:

```
./myshell batchfile
```

The batchfile contains a set of line commands for MyShell to execute. When the end of the file (EOF) is reached, MyShell exits.

### 5. Background Execution
If a command ends with an ampersand (&), the shell returns to the prompt immediately after launching the program in the background. A message is displayed indicating the job and process ID:

```
[<job id>] <process id>
```

Example:
```
$ echo 'hello' &
[1] 10506
hello
```

## Compilation and Execution

To compile the project, run:

```
make
```

To execute MyShell, use:

```
./myshell [batchfile]
```

- If a batchfile is provided as an argument, MyShell will execute the commands from the file and close when the end of the file is reached.
- If no argument is provided, MyShell will display the prompt and wait for user commands via stdin.
