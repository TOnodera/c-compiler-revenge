#!/bin/bash

assert(){
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -static -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 0
assert 42 42
assert 21 '20+5-4'
assert 41 ' 12 + 34 - 5 '
assert 36 '2*3 + 5*6'
assert 32 '10 / 2 + 5 * 6 - 3'
assert 64 '(10+2) * 5 + 4'
assert 9 '-1+10'
assert 10 '20+(-10*1)'

echo OK