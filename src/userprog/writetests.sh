#!/bin/bash

make

rm build/tests/userprog/write-bad-fd.result
rm build/tests/userprog/write-bad-ptr.result
rm build/tests/userprog/write-boundary.result
rm build/tests/userprog/write-normal.result
rm build/tests/userprog/write-stdin.result
rm build/tests/userprog/write-zero.result


make build/tests/userprog/write-bad-fd.result
make build/tests/userprog/write-bad-ptr.result
make build/tests/userprog/write-boundary.result
make build/tests/userprog/write-normal.result
make build/tests/userprog/write-stdin.result
make build/tests/userprog/write-zero.result

egrep 'PASS|FAIL' build/tests/userprog/write*.result