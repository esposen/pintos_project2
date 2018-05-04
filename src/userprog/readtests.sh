#!/bin/bash

make

rm build/tests/userprog/read-bad-fd.result 
rm build/tests/userprog/read-bad-ptr.result 
rm build/tests/userprog/read-boundary.result 
rm build/tests/userprog/read-normal.result 
rm build/tests/userprog/read-stdout.result 
rm build/tests/userprog/read-zero.result 

make build/tests/userprog/read-bad-fd.result 
make build/tests/userprog/read-bad-ptr.result 
make build/tests/userprog/read-boundary.result 
make build/tests/userprog/read-normal.result 
make build/tests/userprog/read-stdout.result 
make build/tests/userprog/read-zero.result 


egrep 'PASS|FAIL' build/tests/userprog/read*.result