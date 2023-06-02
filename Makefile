# Copyright (C) 2023 Ethan Uppal. All rights reserved.

SRCDIR		:= ./src
INCLUDEDIR	:= ./src

CC			:= $(shell which gcc || which clang)
CFLAGS		:= -std=c99 -pedantic -Wall -Wextra -I $(INCLUDEDIR)
CDEBUG		:= -g
CRELEASE	:= -O2
TARGETDIR	:= lib
TARGET		:= $(TARGETDIR)/$config

# CFLAGS 		+= $(CRELEASE)
CFLAGS 		+= $(CDEBUG)

SRC			:= $(shell find $(SRCDIR) -name "*.c" -type f)
OBJ			:= $(SRC:.c=.o)

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
dynamic: $(TARGET).o

$(TARGET):

%.o: %.c
	@echo 'Compiling $@'
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(TARGET).a $(TARGET).so $(OBJ)

$(TARGET).a: $(OBJ)
	@echo 'Creating static $@'
	$(AR) $(AR_OPT) $^ -o $@

$(TARGET).so: $(OBJ)
	@echo 'Creating dynamic $@'
	$(CC) $(CFLAGS) -shared $< -o $@

PY := python3

serve:
	$(PY) -m http.server 8080

test: main.c static
	@echo 'Running test program main.c'
	$(CC) $(CFLAGS) $< -o main
	./main
