#!/bin/bash

rm filesys.dsk
cd ../examples
make
cd ../userprog
pintos-mkdisk filesys.dsk --filesys-size=2
pintos -f -q
pintos -p ../examples/echo -a echo -- -q
pintos -p ../examples/pwd -a pwd -- -q
