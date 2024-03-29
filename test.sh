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
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

try 0 0
try 42 42
try 21 "5+20-4"
try 9 "5+5-1"
try 12 "10 - 5 + 7"
try 47 "5 +6* 7"
try 15 "5*( 9 - 6)"
try 4 "(3+5)/2"
try 0 "-10 + 10"
try 50 "-10 * 3 + 80"
try 1 "1 == 1"
try 0 "1 == 2"
try 1 "10 == 10"
try 1 "5 + 20 - 4 == 21"
try 0 "1 <= 0"
try 1 "1 >= 0"
try 1 "10 > 2"
try 1 "12 >= 12"
try 0 "11 < 10"
try 0 "31 != 31"
echo OK
