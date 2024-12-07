# Copyright (C) 2023 Ethan Uppal. All rights reserved.
#
# Purpose: builds and installs the config library.

SRCDIR		:= .
INCLUDEDIR	:= .

CC			:= $(shell which gcc || which clang)
CFLAGS		:= -std=c99 -pedantic -Wall -Wextra -I $(INCLUDEDIR) -D _XOPEN_SOURCE=700
CDEBUG		:= -g
CRELEASE	:= -O2
TARGET		:= config

CFLAGS 		+= $(CRELEASE)

SRC			:= config.c
OBJ			:= $(SRC:.c=.o)

PUBHEA		:= config.h

STATICLIB	:= lib$(TARGET).a
DYNLIB		:= lib$(TARGET).so
LCLLIBS		:= $(STATICLIB) $(DYNLIB)

INSTLIB		:= /usr/local/lib
INSTHEA		:= /usr/local/include/$(TARGET)

# NOT MY CODE
# Customizes ar for macOS
ifeq ($(shell uname), Darwin)
AR 		:= /usr/bin/libtool
AR_OPT 	:= -static -o
else
AR 		:= ar
AR_OPT 	:= rcs -o
endif

all: static dynamic
static: $(STATICLIB)
dynamic: $(DYNLIB)

%.o: %.c
	@echo 'Compiling $@'
	$(CC) $(CFLAGS) $^ -c -o $@

clean:
	rm -rf $(LCLLIBS) $(OBJ) test/main docs

$(STATICLIB): $(OBJ)
	@echo 'Creating static $@'
	$(AR) $(AR_OPT) $@ $^

$(DYNLIB): $(OBJ)
	@echo 'Creating dynamic $@'
	$(CC) $(CFLAGS) -shared $< -o $@

install: $(LCLLIBS)
	mv $(STATICLIB) $(INSTLIB)
	mv $(DYNLIB) $(INSTLIB)
	mkdir -p $(INSTHEA)
	cp $(PUBHEA) $(INSTHEA)

uninstall:
	rm -rf $(INSTLIB)/$(STATICLIB) \
		$(INSTLIB)/$(DYNLIB) \
		$(INSTHEA)

# check:
# 	if [ ! -f "$(INSTLIB)/$(TARGET).a" ]
# 	then
# 	    echo "Static library not installed." >&2
# 	fi
# 	if [ ! -f "$(INSTLIB)/$(TARGET).so" ]
# 	then
# 	    echo "Dynamic library not installed." >&2
# 	fi
# 	if [ ! -f "$(INSTHEA)" ]
# 	then
# 	    echo "Library headers not installed." >&2
# 	fi

# Requires doxygen to be installed.
# Link: https://www.doxygen.nl
docs:
	if ! command -v doxygen &> /dev/null
	then
	    echo "'doxygen' could not be found"
		echo "You can install it from https://www.doxygen.nl"
	    exit
	fi
	doxygen
