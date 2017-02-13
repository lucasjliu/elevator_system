CPPFLAGS=-g -Wall -Wextra -std=c++11 -iquote src/ -Wno-sign-compare
CXX=g++
SRC_DIR=src
BIN_DIR=bin
OBJ_DIR=obj
LDFLAGS=
LIBS=

SOURCES=$(wildcard $(SRC_DIR)/*.cpp)
OBJS=$(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(OBJ_DIR) $(BIN_DIR) test

test: $(OBJS)
	$(CXX) $(CPPFLAGS) $(LIBS) $(OBJS) -o $(BIN_DIR)/test

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(LIBS) -c $< -o $@

clean:
	rm $(BIN_DIR)/* $(OBJ_DIR)/*.o