
GXX49_VERSION := $(shell g++-4.9 --version 2>/dev/null)

ifdef GXX49_VERSION
	CXX_COMMAND := g++-4.9
else
	CXX_COMMAND := g++
endif

CXX = ${CXX_COMMAND} -std=c++17 -Wall

run_test: maxcalorie_test
	./maxcalorie_test

headers: rubrictest.hh maxcalorie.hh

maxcalorie_test: headers maxcalorie_test.cc
	${CXX} maxcalorie_test.cc -o maxcalorie_test

clean:
	rm -f maxcalorie_test
