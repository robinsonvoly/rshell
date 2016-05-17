#!/bin/sh

#Multiple command test cases
ls -a; ls -l;
echo this; echo that
echo a; echo b || echo c
echo a && echo b || echo c; echo d
ls a; echo hello && mkdir test || echo world; git status
ls a; echo hello; mkdir test
rm test && echo hello
echo this || echo that || echo no
echo this && echo that && echo no
echo this || echo that || echo no;echo this && echo that && echo no
mkdir test; rm -r test;
ls -a || ls -l || touch test_folder && echo no
cat README.md && cat license.md
xyz || abc || ls -a; xyz && abc && ls -a