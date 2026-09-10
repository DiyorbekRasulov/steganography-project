OS := $(shell uname -s)

ifeq ($(OS), Darwin)
	INCLUDE_PATH := /opt/homebrew/Cellar/criterion/2.4.1_1/include
	LIB_PATH := /opt/homebrew/Cellar/criterion/2.4.1_1/lib
endif

ifeq ($(OS), Linux)
	INCLUDE_PATH := /util/criterion/include
	LIB_PATH := /util/criterion/lib/x86_64-linux-gnu
endif

CC = gcc

CFLAGS = -Wall -Wextra -Werror -pedantic \
		  -std=c11 -ggdb \
		  -fprofile-arcs -ftest-coverage

SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

SRC_FILES = $(SRC_DIR)/main.c \
			$(SRC_DIR)/steg.c \
			$(SRC_DIR)/encode.c \
			$(SRC_DIR)/decode.c \
			$(SRC_DIR)/image.c \
			$(SRC_DIR)/fileio.c \
			$(SRC_DIR)/utils.c

OBJ_FILES = $(SRC_FILES:.c=.o)

TARGET = steg

TEST_TARGET = tests

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ_FILES)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

tests: $(OBJ_FILES) $(TEST_DIR)/tests.o
	$(CC) $(CFLAGS) \
	-L $(LIB_PATH) \
	-lcriterion \
	-o $(TEST_TARGET) \
	$(OBJ_FILES) \
	$(TEST_DIR)/tests.o

$(TEST_DIR)/tests.o: $(TEST_DIR)/tests.c
	$(CC) $(CFLAGS) \
	-I $(INCLUDE_PATH) \
	-c $(TEST_DIR)/tests.c \
	-o $(TEST_DIR)/tests.o

run_tests: tests
	./tests

coverage: clean run_tests
	@mkdir -p results
	gcov $(SRC_DIR)/*.c > results/coverage.txt
	@cat results/coverage.txt

memcheck: $(TARGET)
	valgrind \
	--tool=memcheck \
	--leak-check=full \
	--show-leak-kinds=all \
	--track-origins=yes \
	./$(TARGET)

callgrind: $(TARGET)
	valgrind \
	--tool=callgrind \
	--callgrind-out-file=results/callgrind.out \
	./$(TARGET)

debug: $(TARGET)
	gdb ./$(TARGET)

clean:
	rm -f $(SRC_DIR)/*.o
	rm -f $(TEST_DIR)/*.o
	rm -f *.gcda *.gcno *.gcov
	rm -f $(TARGET) $(TEST_TARGET)
	rm -rf results

.PHONY: all clean tests run_tests coverage memcheck callgrind debug
