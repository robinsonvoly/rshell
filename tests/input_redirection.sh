#!/bin/bash

#Commands with input redirection
echo abc > out.txt
cat < out.txt > new.txt
echo nono >> out.txt
cat < out.txt >> new.txt
exit
