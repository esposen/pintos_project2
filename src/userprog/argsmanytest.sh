#!/bin/bash

make

pintos -v -k -T 60 --qemu --filesys-size=2 -p tests/userprog/args-many -a args-many -- -q -f 
pintos --gdb -- run 'args-many a b c d e f g h i j k l m n o p q r s t u v'
