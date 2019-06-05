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

try 7 "a = 4; b = 3; return (a + b);"
try 4 "return 8 / 2;"
try 25 "a = 3; b = 5 * 6 - 8; return a + b;"

echo ok
