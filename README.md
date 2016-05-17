#CS100 Assignment 2
Rshell is a program designed to imitate the bash of many command lines. It allows for the access and use of many different bash commands. It prints a command prompt (i.e $) and reads commands on a single line. The user will be able to input commands such as (ls, echo, mkdir, cat, etc.) that are in the terminal bin folder along with the necessary flags. The commands have the form:
```
   cmd = executable[argumentList][connector cmd]
```
and can be separated by three connectors ( &&, ||, ;). 

####Installation:
```
   $ git clone https://github.com/rvoly001/rshell.git
   $ cd rshell
   $ git checkout hw2
   $ make
   $ bin/rshell
```

####Test Cases:
All test files have been placed in the tests folder. Each test case has several different commands that were tested to see functionality of the program.
- single_command.sh
- multi_command.sh
- commented_command.sh
- exit.sh

####Bugs:
- There is a bug with the execution of the || and && connectors in which or's are sometimes prioritized before any and connectors.
