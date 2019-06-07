#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -c -o tmp tmp.s 
  gcc -c -o func_test ./sample/test.c 
  gcc tmp func_test
  FUNC_TEST_RESULT=`./a.out`
  actual="$FUNC_TEST_RESULT"
  echo "./sample/test.c"
  cat ./sample/test.c
  echo $?

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi 
}

try "5" "return foo(2, 3);"

echo ok
