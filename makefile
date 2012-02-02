# Calc
# Copyright (C) 2012 David Jolly

CC=g++
APP=cli-calc
MAIN=calc
SRC=src/
INSTALL=/usr/bin
DOC=/usr/share/doc/cli-calc

all: build calc

clean:
	rm -f $(SRC)*.o $(APP)

build: $(SRC)exc_code.o $(SRC)lexer.o $(SRC)parser.o $(SRC)pb_buffer.o $(SRC)sym_table.o $(SRC)syn_tree.o $(SRC)token.o

install:
	install -s $(APP) ${INSTALL}
	mkdir ${DOC}
	chmod 755 ${DOC}
	install -m 644 -t ${DOC} ChangeLog COPYING INSTALL README

uninstall:
	rm ${INSTALL}/bin/$(APP)

calc: build $(SRC)$(MAIN).cpp $(SRC)$(MAIN).hpp
	$(CC) -std=c++0x -lm -o $(APP) $(SRC)$(MAIN).cpp $(SRC)exc_code.o $(SRC)lexer.o $(SRC)parser.o $(SRC)pb_buffer.o $(SRC)sym_table.o $(SRC)syn_tree.o $(SRC)token.o

exc_code.o: $(SRC)exc_code.cpp $(SRC)exc_code.hpp
	$(CC) -std=c++0x -c $(SRC)exc_code.cpp

lexer.o: $(SRC)lexer.cpp $(SRC)lexer.hpp
	$(CC) -std=c++0x -c $(SRC)lexer.cpp

parser.o: $(SRC)parser.cpp $(SRC)parser.hpp
	$(CC) -c $(SRC)parser.cpp

pb_buffer.o: $(SRC)pb_buffer.cpp $(SRC)pb_buffer.hpp
	$(CC) -c $(SRC)pb_buffer.cpp

sym_table.o: $(SRC)sym_table.cpp $(SRC)sym_table.hpp
	$(CC) -c $(SRC)sym_table.cpp

syn_tree.o: $(SRC)syn_tree.cpp $(SRC)syn_tree.hpp
	$(CC) -c $(SRC)syn_tree.cpp

token.o: $(SRC)token.cpp $(SRC)token.hpp
	$(CC) -c $(SRC)token.cpp
