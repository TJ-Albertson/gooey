# Makefile
CC = gcc
CFLAGS = -std=c89 -Iinclude
LDFLAGS = -Llib -lglfw3 -lglfw3 -lopengl32 -lkernel32 -luser32 -lgdi32 -lfreetype

SRC = main.c glad.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: custom_ui

custom_ui: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: custom_ui
	./custom_ui

clean:
	del $(OBJ) custom_ui.exe
