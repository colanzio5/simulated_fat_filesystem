
CXX = g++  # C++ compiler
CXXFLAGS = -g -c  # C++ flags

# Replace test_suite.C with your code

disksim : BlockDevice.o DiskSim.o hexdump.o
	$(CXX) -o disksim -g BlockDevice.o DiskSim.o hexdump.o 

BlockDevice.o : BlockDevice.cpp BlockDevice.h

hexdump.o : hexdump.cpp hexdump.h

disksim.o : DiskSim.cpp BlockDevice.h

clean :
	rm *.o
