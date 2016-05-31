#!/bin/bash

#Precedence commands
#Single:
(echo a); echo b
echo a; (echo b)

#Double:
(echo a || echo b)
(echo b && echo c); echo no#yes

#Multiple
(echo a || echo b) && (echo a || echo c)
(echo a && echo b) || (echo a)
