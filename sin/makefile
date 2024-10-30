libsin1.dylib: sin1.o
	ld -o libsin1.dylib sin1.o -dylib -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path`
sin1.o: sin1.s
	as -arch arm64 -o sin1.o sin1.s

libsin2.dylib: sin2.o
	ld -o libsin2.dylib sin2.o -dylib -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path`
sin2.o: sin2.s
	as -arch arm64 -o sin2.o sin2.s

libsin3.dylib: sin3.o
	ld -o libsin3.dylib sin3.o -dylib -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path`
sin3.o: sin3.s
	as -arch arm64 -o sin3.o sin3.s

libreduce.dylib: reduce.o
	ld -o libreduce.dylib reduce.o -dylib -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path`
reduce.o: reduce.s
	as -arch arm64 -o reduce.o reduce.s

test: test.o libsin1.dylib libsin2.dylib libsin3.dylib
	clang -o test test.o -L. -lsin1 -lsin2 -lsin3 -L/usr/local/lib -lgsl
test.o: test.c
	gcc -c test.c -o test.o -I/usr/local/include

benchmark: benchmark.o libsin3.dylib
	clang -o benchmark benchmark.o -L. -lsin3 -L/usr/local/lib -lgsl
test.o: benchmark.c
	gcc -c benchmark.c -o benchmark.o -I/usr/local/include
