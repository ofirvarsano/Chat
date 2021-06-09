# this is my target
TARGET = ServerManager
# All .c files in this directory are my sources
SOURCES = ServerManager.c ServerNet.c UsersTable.c protocol.c List.c HashMap.c Groups.c IPTable.c queue.c genericVector.c
# Include directory
INC_DIR = include/

# General compilation flags
# The compiler will be gcc("CC" is MAKE variable)
CC = gcc
# The compiler flags will be... ("CFLAGS" is MAKE variable)
CFLAGS = -ansi -pedantic -Wall -g3 -Werror
LDFLAGS = -g
CPPFLAGS += -I$(INC_DIR)

# my objects are the .o of my sources
OBJECTS = $(SOURCES:.c=.o)

# convention
.PHONY: all
all: $(TARGET)

# The default command, link all the objects
# $(CC) $(LDFLAGS) $(^) -o $(@)
$(TARGET): $(OBJECTS)

# build-in Ccompiler rule is $(CC) $(CFLAGS) $(CPPFLAGS) -c -o $(@) $(<)

include depends

depends: 
	$(CC) -MM $(CPPFLAGS) $(SOURCES) > $(@)

.PHONY: clean
clean:
	-rm -f $(TARGET) $(OBJECTS) depends core *.log 

.PHONY: run
run:  $(TARGET)
	 ./$(TARGET)

.PHONY: gdb
gdb:  $(TARGET)
	 gdb -q ./$(TARGET)
	  
	
