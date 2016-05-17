#!/bin/sh

#Comments within commands
echo hello #this is a comment
ls -a # this outputs file directory
git status#output git status
ls -a || echo no#this works
echo no && echo hi || echo what # commented
mkdir test#no
rm test -r#-i
echo hello# echo no this will not output after a hashtag
echo yes || echo no #etc