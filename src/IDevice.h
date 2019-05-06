#ifndef I_DEVICE_H
#define I_DEVICE_H

#include "../inc/BlockDevice.h"
#include "../inc/hexdump.h"
#include "SuperBlock.h"

class IDevice
{
	private:
		BlockDevice* device;

	public:
		uint32_t block_size;
		uint32_t number_blocks;
		IDevice(BlockDevice* device);
		int writeToDevice(uint32_t start_block, uint32_t offset, uint32_t buffer_size, char* buffer);
		int readFromDevice(uint32_t start_block, uint32_t offset, uint32_t buffer_size, char* buffer);
		SuperBlock* checkForSuperBlock();
		SuperBlock* createSuperBlock();
};

#endif //I_DEVICE_H