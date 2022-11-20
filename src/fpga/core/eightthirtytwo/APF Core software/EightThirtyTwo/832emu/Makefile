BASE	= 
CPP      = $(BASE)-g++
LD      = $(BASE)-g++
AS      = $(BASE)-as
CP      = $(BASE)-objcopy
DUMP    = $(BASE)-objdump

BUILD_DIR=.obj

ZPUSIM_PRJ = 832e
ZPUSIM_SRC = 832e.cpp pathsupport.cpp util.cpp debug.cpp
ZPUSIM_HEADERS = binaryblob.h hackstream.h pathsupport.h util.h debug.h config.h
ZPUSIM_OBJ = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(ZPUSIM_SRC))

LINKMAP  = 
LIBDIR   = 

# Commandline options for each tool.

CFLAGS  = -c -O3
AFLAGS  = 
LFLAGS  = -O3

# Libraries.
LIBS       =

# Our target.
all: $(BUILD_DIR) $(ZPUSIM_PRJ)

clean:
	rm -f $(BUILD_DIR)/*.o

# Link - this produces an ELF binary.
$(ZPUSIM_PRJ): $(ZPUSIM_OBJ)
	$(LD) $(LFLAGS) -o $@ $+ $(LIBS)

$(BUILD_DIR)/%.o: %.cpp $(HEADERS)
	$(CPP) $(CFLAGS)  -o $@ -c $<

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

