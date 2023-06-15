# Copyright (C) 2023 Ethan Uppal. All rights reserved.

SRCDIR		:= .
INCLUDEDIR	:= .

CC			:= $(shell which gcc || which clang)
CFLAGS		:= -std=c99 -pedantic -Wall -Wextra -I $(INCLUDEDIR)
CDEBUG		:= -g
CRELEASE	:= -O2
TARGET		:= config

# CFLAGS 		+= $(CRELEASE)
# CFLAGS 		+= $(CDEBUG)

SRC			:= config.c
OBJ			:= $(SRC:.c=.o)

PUBHEA		:= config.h

INSTLIB		:= /usr/local/lib
INSTHEA		:= /usr/local/include/$(TARGET)

# NOT MY CODE
# Customizes ar for macOS
ifeq ($(shell uname), Darwin)
AR 		:= /usr/bin/libtool
AR_OPT 	:= -static
else
AR 		:= ar
AR_OPT 	:= rcs $@ $^
endif

all: static dynamic
static: $(TARGET).a
dynamic: $(TARGET).so

%.o: %.c
	@echo 'Compiling $@'
	$(CC) $(CFLAGS) $^ -c -o $@

clean:
	rm -rf $(TARGET).a $(TARGET).so $(OBJ) test/main docs

$(TARGET).a: $(OBJ)
	@echo 'Creating static $@'
	$(AR) $(AR_OPT) $^ -o $@

$(TARGET).so: $(OBJ)
	@echo 'Creating dynamic $@'
	$(CC) $(CFLAGS) -shared $< -o $@

install: $(TARGET).a $(TARGET).so
	mv $(TARGET).a $(INSTLIB)
	mv $(TARGET).so $(INSTLIB)
	mkdir -p $(INSTHEA)
	cp $(PUBHEA) $(INSTHEA)


# ------------------------------ Docs + Testing ------------------------------ #

PY 			:= python3
TESTSRC     := ./test/main.c

docs:
	doxygen

serve: docs
	$(PY) -m http.server 8080

build_test: $(TESTSRC) $(TARGET).a
	$(CC) $(CFLAGS) $^ -o ./test/main

test: static build_test
	@echo 'Running test program main.c'
	cd ./test; ./main config.txt
