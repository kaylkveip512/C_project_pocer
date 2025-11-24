CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -std=c99 -g
CXXFLAGS = -Wall -Wextra -std=c++17 -g
TARGET = poker
TEST_TARGET = poker_tests

GTEST_LIB = -lgtest -lgtest_main -lpthread

all: $(TARGET)

$(TARGET): poker.c poker_main.c
	$(CC) $(CFLAGS) -o $(TARGET) poker.c poker_main.c

$(TEST_TARGET): poker_tests.cpp poker.c
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) poker_tests.cpp poker.c $(GTEST_LIB)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

test_verbose: $(TEST_TARGET)
	./$(TEST_TARGET) --gtest_verbose=1

test_specific: $(TEST_TARGET)
	./$(TEST_TARGET) --gtest_filter=PokerTest.*

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o

run: $(TARGET)
	./$(TARGET)

all_tests: $(TARGET) $(TEST_TARGET)
	./$(TEST_TARGET) && ./$(TARGET)

.PHONY: all clean run test test_verbose test_specific all_tests