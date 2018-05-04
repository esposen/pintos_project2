make
cd build
pintos --gdb -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/close-normal -a close-normal -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run close-normal