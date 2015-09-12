# Executable
EXE    = 2lan-easy-hiero-multithread

# Compiler, Linker Defines
CC      = g++ -I ./include/
PG      = #-pg
CFLAGS = -g
#CFLAGS  = -Wall -O2 -Wno-deprecated $(PG) -std=c++0x
LIBS = ./lib/liboolm.a ./lib/libdstruct.a ./lib/libmisc.a ./lib/liblattice.a ./lib/libflm.a -lpthread
LDFLAGS = $(LIBS)

# Compile and Assemble C++ Source Files into Object Files
#echo compiling...
%.o: %.cpp
	$(CC) -c $(CFLAGS) $<
# -o $@
# Source and Object files
SRC    = $(wildcard *.cpp)
OBJ    = $(patsubst %.cpp, %.o, $(SRC))

# Link all Object Files with external Libraries into Binaries
#echo linking...
$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $(EXE) -lz

.PHONY: clean
clean:
	 -rm -f core *.o $(EXE)

