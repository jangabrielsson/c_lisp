#
# A simple makefile for compiling a c project
#

CXX       := clang
CXX_FLAGS := -ggdb

BIN     := bin
SRC     := src
INCLUDE := include

INC =-I/opt/homebrew/Cellar/glib/2.76.3/include/glib-2.0 -I/opt/homebrew/Cellar/glib/2.76.3/lib/glib-2.0/include -I/opt/homebrew/opt/gettext/include -I/opt/homebrew/Cellar/pcre2/10.42/include
LIB = -L/opt/homebrew/Cellar/glib/2.76.3/lib -L/opt/homebrew/opt/gettext/lib -lglib-2.0 -lintl

LIBRARIES   :=
EXECUTABLE  := main


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.c
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $(INC) $^ -o $@ $(LIBRARIES) $(LIB)

clean:
	-rm $(BIN)/*