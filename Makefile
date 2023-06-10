#
# A simple makefile for compiling a c project
#

CXX       := clang
CXX_FLAGS := -ggdb

BIN     := bin
SRC     := src
INCLUDE := include

GLIB    := /opt/homebrew/Cellar/glib/2.76.3
GTEXT   := /opt/homebrew/opt/gettext
PCRE    := /opt/homebrew/Cellar/pcre2/10.42

INC =-I$(GLIB)/include/glib-2.0 -I$(GLIB)/lib/glib-2.0/include -I$(GTEXT)/include -I$(PCRE)/include
LIB = -L$(GLIB)/lib -L$(GTEXT)/lib -lglib-2.0 -lintl

LIBRARIES   :=
EXECUTABLE  := main


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.c
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $(INC) $^ -o $@ $(LIBRARIES) $(LIB)

clean:
	-rm -r $(BIN)/*