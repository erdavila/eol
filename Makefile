.PHONY: all
all: eol

eol: eol.cpp
	g++ -Wall -std=c++11 $+ -o $@
