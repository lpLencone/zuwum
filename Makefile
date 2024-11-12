BIN	:= zuwum
SRC	:= $(shell find src -name "*.c")
PKGS	:= x11 opengl glu glx
LIBS	:= `pkg-config --libs $(PKGS)`
CFLAGS	:= -Wall -Wextra -pedantic -std=c23 -pedantic `pkg-config --cflags $(PKGS)`

$(BIN): src/main.c
	gcc $(CFLAGS) -o $(BIN) $(SRC) $(LIBS)
