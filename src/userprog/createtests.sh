#!/bin/bash

make

rm build/tests/userprog/create-bad-ptr.result 
rm build/tests/userprog/create-bound-none.result
rm build/tests/userprog/create-empty-many.result
rm build/tests/userprog/create-exists.result
rm build/tests/userprog/create-long.result
rm build/tests/userprog/create-normal.result
rm build/tests/userprog/create-null.result

make build/tests/userprog/create-bad-ptr.result 
make build/tests/userprog/create-bound-none.result
make build/tests/userprog/create-empty-many.result
make build/tests/userprog/create-exists.result
make build/tests/userprog/create-long.result
make build/tests/userprog/create-normal.result
make build/tests/userprog/create-null.result

egrep 'PASS|FAIL' build/tests/userprog/create*.result