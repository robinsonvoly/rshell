#!/bin/sh

#Comments within commands

echo "Testing command with a comment"
echo hello #this is a comment
ls -a # this outputs file directory

echo "Testing command with a comment, no space"
git status#output git status

echo "Testing command with a comment and connectors"
ls -a || echo no#this works
echo no && echo hi || echo what # commented
mkdir test#no
rm test -r#-i

echo "Testing command with a comment in middle of command"
echo hello# echo no this will not output after a hashtag
echo yes || echo no #etc