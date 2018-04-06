#!/bin/bash

rm filesys.dsk
pintos-mkdisk filesys.dsk --filesys-size=2
pintos -f -q
