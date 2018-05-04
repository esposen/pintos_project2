#!/bin/bash

make

rm build/tests/userprog/close-bad-fd.result 
rm build/tests/userprog/close-normal.result
rm build/tests/userprog/close-stdin.result
rm build/tests/userprog/close-stdout.result
rm build/tests/userprog/close-twice.result

make build/tests/userprog/close-normal.result
make build/tests/userprog/close-stdin.result
make build/tests/userprog/close-stdout.result
make build/tests/userprog/close-twice.result
make build/tests/userprog/close-bad-fd.result 

egrep 'PASS|FAIL' build/tests/userprog/close*.result