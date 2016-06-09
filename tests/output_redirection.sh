#!/bin/bash

#Commands with output redirection
echo abc > out.txt
echo no way >> out.txt
cat < out.txt > new.txt
cat < out.txt >> new.txt

echo this works >> new.txt
