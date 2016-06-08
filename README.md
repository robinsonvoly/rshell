#CS100 Assignment 4
Rshell is a program designed to imitate the bash of many command lines. It allows for the access and use of many different bash commands. It prints a command prompt (i.e $) and reads commands on a single line. The user will be able to input commands such as (ls, echo, mkdir, cat, etc.) that are in the terminal bin folder along with the necessary flags. The commands have the form:
```
   cmd = executable[argumentList][connector cmd]
```
and can be separated by three connectors ( &&, ||, ;). 

######hw3 Version Addition:
Additionally, there has been added functionality over the previous hw2 version so that the command line is now able to run test commands such as:
```
   $ test -e test/file/path
```
Along with the symbolic version:
```
   $ [ -e test/file/path ]
```

The following commands are able to be used:
```
    -e  checks if file/directory exists
    -f  checks if file/directory exists and is a regular file
    -d  checks if file/directory exists and is a directory
```
If no tag is specified the -e will be used by default.

Precedence Operators have been added as well with "()" parentheses. Such as:
```
   $ (echo A && echo B) || (echo C && echo D)
```

######hw4 Version Addition:
Added functionality includes input redirection "<", output redirection ">" and ">>", as well as piping "|". Implemented utilizing Unix functiond dup and pipe.

Example usage:
```
   $ cat < existingInputFile | tr A-Z a-z | tee newOutputFile1 | tr a-z A-Z > newOutputFile2
```

####Installation:
```
   $ git clone https://github.com/rvoly001/rshell.git
   $ cd rshell
   $ git checkout hw3
   $ make
   $ bin/rshell
```

####Test Cases:
All test files have been placed in the tests folder. Each test case has several different commands that were tested to see functionality of the program.
- test_test.sh
- precedence_test.sh
- commented_command.sh
- exit.sh

####Bugs:
- There is a bug with the execution of the || and && connectors in which or's are sometimes prioritized before any and connectors.

####Authors:
Robinson Vo-Ly (rvoly001)
