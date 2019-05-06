#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <iostream>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "../inc/BlockDevice.h"
#include "../inc/hexdump.h"
#include "DirEntry.h"
#include "IDevice.h"
#include "SuperBlock.h"

#define DEFAULT_DISK "foobar.dsk"
#define DEFAULT_NUMBER_BLOCKS 1000

class FileSystem
{
  public:
	// datamembers
	IDevice *device;
	SuperBlock *super_block;

	// constructor
	FileSystem(BlockDevice *device);
	// filesyem api functions
	bool formatFileSystem();
	// public utility functions
	void debugFileSystem(bool verbose = false);

  private:
	// returns the number of bytes used by the free map
	inline int _getFreeMapNumberBytes() { return this->device->number_blocks * sizeof(uint32_t); }
	// get number of blocks used by free map
	inline int _getFreeMapNumberBlocks() { return ceil((float)this->_getFreeMapNumberBytes() / (float)this->device->block_size); }
	// returns the number of bytes used by the fat table
	inline uint32_t _getFatTableNumberBytes() { return this->device->number_blocks * sizeof(DirEntry); }
};

#endif // FILE_STSTEM_H