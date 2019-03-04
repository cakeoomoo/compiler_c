#!/bin/bash

try() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi
}

try 40 "10-4+34"

try 70 "10-4+34-4+34"

try 41 " 12 + 34 - 5 "

try 53 " 12 + 34 ? 5?2 "



try 113 "    555 - 444 ? 2 "


#try 0 1-2+4

echo OK
