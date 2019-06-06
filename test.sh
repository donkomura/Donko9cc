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

try 1 "if (1) return 1; else return 0;"
try 6 "
atcoder = 2 * 3;
atcodeer = 6; 
if (atcoder == atcodeer) 
  return atcoder; 
else 
  return 0;"

try 5 "a=3; while(a > 0) a = a - 1; return a + 5;"
try 1 "b = 0; for (a = 0; a < 5; a = a + 1) if (a > 3) b = b + 1; return b;"
try 10 "
cnt = 0;
for (a = 0; a < 10; a = a + 1) 
  cnt = cnt + 1; 
return cnt;
"
echo ok
