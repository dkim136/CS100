#!/bin/sh

echo
echo "Testing exit command"

echo
../bin/rshell.out < "exit1.txt"
echo
../bin/rshell.out < "exit2.txt"
echo
../bin/rshell.out <"exit3.txt"
echo
echo "Test Complete"
echo