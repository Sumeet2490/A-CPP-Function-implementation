all:compile

compile: Function_test.cpp Function.hpp
	g++ Function_test.cpp -o Function_test -std=c++11 -Wall -Wextra -pedantic

run:
	./Function_test

clean:
	rm Function_test

