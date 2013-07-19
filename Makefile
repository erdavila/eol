.PHONY: all
all: eol

eol: eol.cpp
	g++ -Wall $+ -o $@
