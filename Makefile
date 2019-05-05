CC=g++ # define the compiler to use
TARGET=disksim # define the name of the executable
SOURCES=src/DiskSim.C src/FileSystem.C src/IDevice.C inc/BlockDevice.C inc/hexdump.C
CFLAGS=-O3 
LFLAGS=

# define list of objects
OBJSC=$(SOURCES:.c=.o)
OBJS=$(OBJSC:.C=.o)

# the target is obtained linking all .o files
all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(TARGET)

purge: clean
	rm -f $(TARGET)

clean:
	rm -f *.o

