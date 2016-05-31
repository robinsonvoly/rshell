#!/bin/bash

#Test the new test commands
#Single and true:
test hw3
test -e hw3
test -d src
test -f readme.md

[ hw3 ]
[ -e hw3 ]
[ -d src ]
[ -f hw3 ]

#Single and false
test hw2
test -e hw2
test -d hw2
test -f a

[ n ]
[ -e b ]
[ -d c ]
[ -f d ]

#With connectors
test hw && echo no
echo no && test -d src || echo no
[ -e hello ] || echo yes
echo a || echo b && [ -f hw3 ]

#With parentheses
(echo a || echo b) && test hw3
test hw3 && (echo a && echo b)
([ -d src ] || echo a) && echo b
