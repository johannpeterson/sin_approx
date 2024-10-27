libsin1.dylib: sin1.o
	ld -o libsin1.dylib sin1.o -dylib -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path`

sin1.o: sin1.s
	as -arch arm64 -o sin1.o sin1.s

test: test.o libsin1.dylib
	clang -o test test.o -L. -lsin1

test.o: test.c
	gcc -c test.c -o test.o
