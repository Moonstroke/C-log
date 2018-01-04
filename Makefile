DEBUG := n
OPT_L := s

LIB := log

CC := gcc
CFLAGS := -finline-functions -pedantic -Wall -Wextra -Wpadded -Wdeclaration-after-statement
ifeq ($(DEBUG), y)
	CFLAGS += -g
endif
ifneq ($(OPT_L), n)
	CFLAGS += -O$(OPT_L)
endif

LDLIBS :=
LDFLAGS :=

TEST_FILES := $(wildcard test*.c)
SRC_FILES := $(filter-out $(TEST_FILES), $(wildcard *.c))
OBJ_FILES := $(SRC_FILES:.c=.o)

AR := ar
AR_FLAGS := rcs
AR_FILE := lib$(LIB).a

TEST_X := test
TEST_LOG := test.log

DOC_PRG := doxygen
DOC_CFG := Doxyfile
DOC_DIR := doc

INST_DIR := /usr/local


.PHONY: all clean distclean doc cleandoc test install


all: $(OBJ_FILES) ar

%.o: %.c
	$(CC) -c $< -o$@ $(CFLAGS)

ar: $(OBJ_FILES)
	$(AR) $(AR_FLAGS) $(AR_FILE) $(OBJ_FILES)

clean:
	rm -rf $(OBJ_FILES) $(TEST_X) $(TEST_LOG)

distclean: clean cleandoc
	rm -rf $(AR_FILE)

doc:
	$(DOC_PRG) $(DOC_CFG)

cleandoc:
	rm -rf $(DOC_DIR)

test: ar
	$(CC) $(TEST_FILES) -o$(TEST_X) -l$(LIB)
	./$(TEST_X)

install: all
	cp --update --target-directory=$(PREFIX_DIR)/include log.h
	cp --update --target-directory=$(PREFIX_DIR)/lib liblog.a
