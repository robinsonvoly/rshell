#!/bin/sh

#Multiple command test cases
echo "Testing two commands separated by semicolon"
ls -a; ls -l;
echo this; echo that

echo "Testing two commands separated by &&, no spaces"
ls&&ls -a

echo "Testing multiple commands separated by semicolon"
echo a; echo b; echo c;

echo "Testing multiple commands separated by semicolon with connectors"
echo a; echo b || echo c
echo a && echo b || echo c; echo d
ls a; echo hello && mkdir test || echo world; git status
ls a; echo hello; mkdir test

echo "Testing multiple commands with multiple connectors"
rm test && echo hello
echo this || echo that || echo no
echo this && echo that && echo no
echo this || echo that || echo no;echo this && echo that && echo no
mkdir test; rm -r test;
ls -a || ls -l || touch test_folder && echo no
cat README.md && cat license.md

echo "Testing commands with invalid executables or bin commands"
xyz || abc || ls -a; xyz && abc && ls -a
xyz end