#!/bin/bash

make

rm build/tests/userprog/halt.result 
rm build/tests/userprog/args-none.result
rm build/tests/userprog/args-many.result
rm build/tests/userprog/exit.result

make build/tests/userprog/halt.result 
make build/tests/userprog/args-none.result
make build/tests/userprog/args-many.result
make build/tests/userprog/exit.result

egrep 'PASS|FAIL' build/tests/userprog/*.result