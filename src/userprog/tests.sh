#!/bin/bash

make

./readtests.sh
./createtests.sh
./opentests.sh
./closetests.sh
./writetests.sh

egrep 'PASS|FAIL' build/tests/userprog/*.result
