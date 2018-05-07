#!/bin/bash

make

rm build/tests/userprog/bad-read.result
rm build/tests/userprog/bad-write.result
rm build/tests/userprog/bad-read2.result
rm build/tests/userprog/bad-write2.result
rm build/tests/userprog/bad-jump.result
rm build/tests/userprog/bad-jump2.result

make build/tests/userprog/bad-read.result
make build/tests/userprog/bad-write.result
make build/tests/userprog/bad-read2.result
make build/tests/userprog/bad-write2.result
make build/tests/userprog/bad-jump.result
make build/tests/userprog/bad-jump2.result

egrep 'PASS|FAIL' build/tests/userprog/bad*.result