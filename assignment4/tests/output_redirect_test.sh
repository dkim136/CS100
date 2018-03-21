#!/usr/bin/env bash
echo
echo "Testing output redirect"

echo
../bin/rshell < input/output_command.txt
echo
echo "Test Complete"
echo