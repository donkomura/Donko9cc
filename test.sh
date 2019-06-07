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

try 32 "
a = 5;
x = 2;
res = 1;
for (i = 1; i <= a; i = i + 1) {
  res = res * x;
}
return res;
"

echo ok
