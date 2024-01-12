CFLAGS = -Wall -Werror -pedantic

TARGET = $(BIN_DIR)/MyShell

BIN_DIR = bin
OBJ_DIR = obj
INC_DIR = inc
LIB_DIR = lib
SRC_DIR = src

$(TARGET) : $(OBJ_DIR)/MyShell.o $(LIB_DIR)/libjobcontrol.a
	mkdir -p $(BIN_DIR)
	gcc $(CFLAGS) $(OBJ_DIR)/MyShell.o -L./$(LIB_DIR) -ljobcontrol -o $(TARGET)

$(OBJ_DIR)/MyShell.o : $(SRC_DIR)/MyShell.c $(INC_DIR)/MyShell.h
	mkdir -p $(OBJ_DIR)
	gcc $(CFLAGS) -c $(SRC_DIR)/MyShell.c -o $(OBJ_DIR)/MyShell.o

$(OBJ_DIR)/JobControl.o : $(SRC_DIR)/JobControl.c $(INC_DIR)/JobControl.h
	gcc $(CFLAGS) -c $(SRC_DIR)/JobControl.c -o $(OBJ_DIR)/JobControl.o

$(LIB_DIR)/libjobcontrol.a : $(OBJ_DIR)/JobControl.o
	mkdir -p $(LIB_DIR)
	ar rs $(LIB_DIR)/libjobcontrol.a $(OBJ_DIR)/JobControl.o

.PHONY: clean
clean:
	rm -f -r $(OBJ_DIR)
	rm -f -r $(BIN_DIR)
	rm -f -r $(LIB_DIR)