#!/usr/bin/env bash

echo
echo "Testing input redirect"

echo
../bin/rshell < input/input_command.txt 
echo
echo "Test Complete"
echo