#!/bin/bash

rm filesys.dsk
cd ../examples
make
cd ../userprog
pintos-mkdisk filesys.dsk --filesys-size=2
pintos -p ../examples/echo -a echo -- -f -q
pintos -p ../examples/echo -a mbin/ls -- -f -q
#pintos -p ../examples/pwd -a pwd -- -q

#pintos -p build/tests/userprog/args-none -a args-none -- -q