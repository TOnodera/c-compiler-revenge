#!/bin/bash

cat <<EOF | gcc -xc -c -o tmp2.o -
int ret3() { return 3; }
int ret5() { return 5; }
EOF

assert(){
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -static -o tmp tmp.s tmp2.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 '0;'
assert 42 '42;'
assert 21 '20+5-4;'
assert 41 ' 12 + 34 - 5 ;'
assert 36 '2*3 + 5*6;'
assert 32 '10 / 2 + 5 * 6 - 3;'
assert 64 '(10+2) * 5 + 4;'
assert 9 '-1+10;'
assert 10 '20+(-10*1);'

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'
assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'
assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

assert 15 'a = 8;b = 5 * 6 - 8; (a + b) / 2;'
assert 14 'a = 3;b = 5 * 6 - 8;a + b / 2;'
assert 202 'a=10;b=20;c=b/a; c + (19+1)*10;'
assert 14 'a = 3;b = 5 * 6 - 8;return a + b / 2;'

assert 14 'foo = 3;bar = 5 * 6 - 8;return foo + bar / 2;'

assert 3 'if (0) return 2; return 3;'
assert 3 'if (1-1) return 2; return 3;'
assert 2 'if (1) return 2; return 3;'
assert 2 'if (2-1) return 2; return 3;'

assert 10 'i=0; while(i<10) i=i+1; return i;'
assert 3 'for (;;) return 3; return 5;'

assert 3 '{1; 2; return 3;}'

assert 3 'return ret3();'
assert 5 'return ret5();'
echo OK