#!/bin/bash

make

rm build/tests/userprog/exec-once.result
rm build/tests/userprog/exec-multiple.result
rm build/tests/userprog/exec-bound.result
rm build/tests/userprog/exec-bound-2.result
rm build/tests/userprog/exec-bound-3.result
rm build/tests/userprog/exec-arg.result
rm build/tests/userprog/exec-missing.result

make build/tests/userprog/exec-once.result
make build/tests/userprog/exec-multiple.result
make build/tests/userprog/exec-bound.result
make build/tests/userprog/exec-bound-2.result
make build/tests/userprog/exec-bound-3.result
make build/tests/userprog/exec-arg.result
make build/tests/userprog/exec-missing.result

# rm build/tests/userprog/halt.result 
# rm build/tests/userprog/args-none.result
# rm build/tests/userprog/args-many.result
# rm build/tests/userprog/exit.result


# make build/tests/userprog/halt.result 
# make build/tests/userprog/args-none.result
# make build/tests/userprog/args-many.result
# make build/tests/userprog/exit.result

egrep 'PASS|FAIL' build/tests/userprog/exec-*.result