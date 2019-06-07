#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -c -o tmp tmp.s
  cc -c -o func_test ./sample/test.c
  cc tmp func_test
  FUNC_TEST_RESULT=`./a.out`
  actual=$FUNC_TEST_RESULT
  echo "./sample/test.c"
  cat ./sample/test.c

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi 
}

try "tapu!" "foo();"

echo ok
