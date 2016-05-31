#Commands with preceding exit
echo "Testing exit"
exit

echo "Testing logout"
logout

echo "Testing command followed by exit"
ls -a
exit

echo hello
exit

echo "Testing connectors followed by exit"
echo no || echo yea && echo this
exit

echo no; echo yea; echo this;
exit

echo "Testing exit with parentheses"
(echo a || echo b)
exit
