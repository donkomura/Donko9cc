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

try 7 "HOGE____64 = 4; pue=3; return HOGE____64+pue;"
try 4 "return 8 / 2;"
try 14 "aaaa = 3; bbbbb = 5 * 6 - 8; return aaaa + bbbbb / 2;"
try 1 "a = 1; b = 2; c = 3; d = 4; e = 5; return a * b / c + e - d;"
try 1 "if (1) return 1; else return 0;"
try 10 "a = 2 * 3; b = 6; if (a == b) return 2 * 5; else return 0;"
echo ok
