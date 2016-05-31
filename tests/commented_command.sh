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

echo "Testing command with comment in beginning"
#this is a comment echo hello world

echo "Testing command with parentheses and comment"
(echo a || echo b || echo c) && echo d # comment comment
#comment comment (echo no)
(echo no)#this is a comment
