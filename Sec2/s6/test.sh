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

try 0 "10 == 4"
try 1 "3 != 2"
try 1 "(2 + 3) < 10"
try 1 "10 <= 10"
try 0 "2 + 3 - 5 * 1 > 20"
try 1 "10 >= 10"

echo ok
