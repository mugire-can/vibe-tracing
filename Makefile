CC      ?= gcc
CFLAGS  ?= -Wall -Wextra -Werror -std=c11 -O2
AR      ?= ar

INCDIR  = include
SRCDIR  = src
OBJDIR  = build
LIBDIR  = build
EXDIR   = examples

CFLAGS += -I$(INCDIR)

# Library
LIB_SRC  = $(SRCDIR)/pdf_loader.c
LIB_OBJ  = $(OBJDIR)/pdf_loader.o
LIB_NAME = $(LIBDIR)/libpdfloader.a

# Example
EX_SRC   = $(EXDIR)/load_pdf.c
EX_BIN   = $(OBJDIR)/load_pdf

# Test
TEST_SRC = tests/test_pdf_loader.c
TEST_BIN = $(OBJDIR)/test_pdf_loader

.PHONY: all lib examples test clean

all: lib examples

lib: $(LIB_NAME)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(LIB_OBJ): $(LIB_SRC) $(INCDIR)/pdf_loader.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_NAME): $(LIB_OBJ)
	$(AR) rcs $@ $^

examples: $(EX_BIN)

$(EX_BIN): $(EX_SRC) $(LIB_NAME)
	$(CC) $(CFLAGS) $< -L$(LIBDIR) -lpdfloader -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(LIB_NAME) | $(OBJDIR)
	$(CC) $(CFLAGS) $< -L$(LIBDIR) -lpdfloader -o $@

clean:
	rm -rf $(OBJDIR)
