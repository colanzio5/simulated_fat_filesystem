#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <stdint.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../inc/BlockDevice.h"
#include "../inc/hexdump.h"
#include "SuperBlock.h"
#include "DirEntry.h"
#include "IDevice.h"

#define DEFAULT_DISK "foobar.dsk"
#define DEFAULT_NUMBER_BLOCKS 1000

class FileSystem 
{ 
	public:
		// datamembers
		IDevice* device;
		SuperBlock* super_block;

		// constructor
		FileSystem(BlockDevice* device);
		// filesyem api functions
		bool formatFileSystem();
		// public utility functions
		void debugFileSystem(bool verbose = false);

	private:
		// file alocation table functions
		int _getFatTableSize();
		// free block map functions
		int _getFreeTableSize();
};


#endif //FILE_STSTEM_H