BIN	:= zuwum
SRC	:= $(shell find src -name "*.c")
PKGS	:= sdl2
LIBS	:= `pkg-config --libs $(PKGS)`
CFLAGS	:= -Wall -Wextra -pedantic -std=c23 -pedantic `pkg-config --cflags $(PKGS)`

$(BIN): src/main.c
	gcc $(CFLAGS) -o $(BIN) $(SRC) $(LIBS)
