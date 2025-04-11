CC = gcc
CFLAGS = -Wall -Wextra -g3 -Iinclude
SRC_DIR = src
LIB_DIR = lib
OBJ_DIR = obj
BIN_DIR = bin
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/main.exe

.PHONY: all clean run dirs compile link

all: clean dirs compile link
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)

dirs:
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)

compile: $(OBJ)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

link: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

clean:
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)
	@if exist $(BIN_DIR) rmdir /s /q $(BIN_DIR)
	@if exist ordenado_por_id.csv del /q ordenado_por_id.csv del
	@if exist ordenado_por_data.csv del /q ordenado_por_data.csv del

run: all
	$(TARGET)

