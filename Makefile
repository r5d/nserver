# SPDX-License-Identifier: BSD-3-Clause
#
# Copyright © 2010, Zed A. Shaw.
# Copyright © 2020 rsiddharth <s@ricketyspace.net>
#

CFLAGS=-g -O2 -Wall -Wextra -Isrc -I/usr/local/include -DNDEBUG $(OPTFLAGS)
LIBS=-ldl $(OPTLIBS)
PREFIX?=/usr/local
CTAGS=`which ectags`

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

PROGRAMS_SRC=$(wildcard bin/*.c)
PROGRAMS=$(patsubst %.c,%,$(PROGRAMS_SRC))

TARGET=build/libnserve.a
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

# The Target Build
all: $(TARGET) $(SO_TARGET) $(PROGRAMS) TAGS tests

dev: CFLAGS=-g -Wall -Isrc -I/usr/local/include -Wall -Wextra $(OPTFLAGS)
dev: all

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@
$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

$(PROGRAMS): LDLIBS += $(TARGET) -lm -L/usr/local/lib -lgdbm

build:
	@mkdir -p build
	@mkdir -p bin

# The Unit Tests
.PHONY: tests
tests: LDLIBS += $(TARGET) -lm -L/usr/local/lib -lgdbm
tests: $(TESTS)
	sh ./tests/runtests.sh

# The Cleaner
clean:
	rm -rf TAGS build $(OBJECTS) $(TESTS) $(PROGRAMS)
	rm -f tests/tests.log
	find . -name "*.gc" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`
	rm -rf nserver.db

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/

# Generate TAGS file
TAGS: $(SOURCES) $(TEST_SRC)
	find ./ -type f  -name '*.h' -or -name '*.c' | $(CTAGS) -e -L -

# The Checker
check:
	@echo Files with potentially dangerous functions.
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)\
			|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true
