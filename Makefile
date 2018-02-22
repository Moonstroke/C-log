DEBUG := n
OPT_L := 2

LIB := log

CFLAGS := -std=c11 -pedantic -Wall -Wextra -Wpadded
ifeq ($(DEBUG), y)
	CFLAGS += -g
endif
ifneq ($(OPT_L), n)
	CFLAGS += -O$(OPT_L)
endif

LDLIBS := -l$(LIB)
LDFLAGS :=

TEST_SRC := $(wildcard test*.c)
SRC := $(filter-out $(TEST_SRC), $(wildcard *.c))
OBJ := $(SRC:.c=.o)

AR := ar
AR_FLAGS := rcs
AR_FILE := lib$(LIB).a

TEST_X := test_log
TEST_LOG := test.log

DOC_PRG := doxygen
DOC_CFG := Doxyfile
DOC_DIR := doc

INST_DIR := /usr/local


.PHONY: all clean distclean doc cleandoc test install


all: $(OBJ_FILES) $(AR_FILE)

%.o: %.c
	$(CC) -c $< -o$@ $(CFLAGS)

$(AR_FILE): $(OBJ)
	$(AR) $(AR_FLAGS) $(AR_FILE) $(OBJ)

clean:
	rm -rf $(OBJ) $(TEST_X) $(TEST_LOG)

distclean: clean docclean
	rm -rf $(AR_FILE)

doc:
	$(DOC_PRG) $(DOC_CFG)

docclean:
	rm -rf $(DOC_DIR)

test: distclean
	$(CC) $(TEST_SRC) -o$(TEST_X) $(LDFLAGS) $(LDLIBS)
	./$(TEST_X)

install: all
	cp --update --target-directory=$(INST_DIR)/include log.h
	cp --update --target-directory=$(INST_DIR)/lib $(AR_FILE)

uninstall:
	rm -f $(INST_DIR)/include/log.h
	rm -f $(INST_DIR)/lib/$(AR_FILE)
