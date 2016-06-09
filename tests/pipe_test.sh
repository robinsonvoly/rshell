#!/bin/bash

#Commands with pipes
ls | tr a-z A-Z 
echo hello | tr a-z A-Z 
echo abc | tr A-Z a-z >> out.txt 
(echo a && echo b) || (echo c && echo d) && cat < tempfile.txt | tr abcde xxxxx
echo hell | tr a-z A-Z || echo abc > out.txt
exit
