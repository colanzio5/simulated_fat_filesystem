#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <iostream>

#include "BlockDevice.h"
#include "SuperBlock.h"
#include "hexdump.h"

#define DEFAULT_DISK "foobar.dsk"
#define DEFAULT_NUMBER_BLOCKS 1000

void PrintHelp()
{
	std::cout << "-N | --number_blocks <n>:  Number of Blocks to Use\n"
				 "-d | --disk_file <fname>:  Command File to Use\n"
				 "-c | --cmd_file <fname>:   Command File to Use\n"
				 "-v | --debug:              Show Verbose Output\n"
				 "-h | --help:               Show help\n\n";
}

int main(int argc, char **argv)
{

	// --------------------------
	// PARSE THE USER'S ARGUMENTS
	// --------------------------

	int number_blocks = DEFAULT_NUMBER_BLOCKS;
	const char *disk_file = DEFAULT_DISK;
	const char *cmd_file = nullptr;
	bool verbose = false;

	const char *const short_opts = "N:d:c:vh";
	const option long_opts[] = {
		{"number_blocks", required_argument, nullptr, 'N'},
		{"disk_file", required_argument, nullptr, 'd'},
		{"cmd_file", required_argument, nullptr, 'c'},
		{"debug", no_argument, nullptr, 'v'},
		{"help", no_argument, nullptr, 'h'},
		{nullptr, no_argument, nullptr, 0}};

	while (true)
	{
		const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);
		if (-1 == opt)
			break;

		switch (opt)
		{

		// the number of blocks to use if creating a new disk device
		case 'N':
			number_blocks = std::stoi(optarg);
			break;

		// the disk file to use or create if it doesn't exist
		case 'd':
			disk_file = optarg;
			break;

		// the command file to use in the disk simulation
		case 'c':
			cmd_file = optarg;
			break;

		// sets verbose output to be true
		case 'v':
			verbose = true;
			break;

		// show what each command can does
		case 'h':
			PrintHelp();
			break;

		case '?':
			if (verbose) printf("\n%s is an invalid argument, exiting...\n", optarg);

		default:
			break;
		}
	}

	if (verbose) printf("arguments parsed...       \n");
	if (verbose) printf("number of blocks used:  %i\n", number_blocks);
	if (verbose) printf("disk file:              %s\n", disk_file);
	if (verbose) printf("command file:           %s\n", cmd_file);

	// warn the user if cmd are missing
	// exit if any required params are missing
	if (cmd_file == nullptr)
	{
		if (verbose) printf("\ncommand file is required, exiting...\n");
		exit(0);
	}

	// -------------------------------
	// LOAD OR CREATE THE BLOCK DEVICE
	// -------------------------------

	if (verbose) printf("\ninstantiating the block device...\n");
	uint32_t numblocks = DEFAULT_NUMBER_BLOCKS;
	BlockDevice *device;

	// See if the the disk exists
	struct stat statbuf;

	// couldn't stat, try to create  new disk
	if (stat(disk_file, &statbuf) == -1)
	{
		if (verbose) printf("creating new disk...\n");
		device = new BlockDevice(disk_file, numblocks);
	}
	else
	{
		if (verbose) printf("loading existing disk from file...\n");
		device = new BlockDevice(disk_file);
	}

	// ------------------
	// RUN THE SIMULATION
	// ------------------
	if (verbose) printf("\nstarting the siumulation...\n");

	uint32_t block_size = device->getBlockSize();

	BlockDevice::result result;

	// allocate block buffer
	char *block = new char[block_size];

	const int fill_blocks = 5;
	for (int blk_idx = 0; blk_idx < fill_blocks; blk_idx++)
	{
		// Fill buffer with data
		char fill_char = 'a' + blk_idx;
		for (int didx = 0; didx < block_size; didx++)
			if (didx == 0)
				block[didx] = '<';
			else if (didx == block_size - 1)
				block[didx] = '>';
			else
				block[didx] = fill_char;

		result = device->writeBlock(blk_idx, block);
		if(verbose) printf("writeBlock %d result %s\n", blk_idx, device->resultMessage(result));
	}

	/* Read blocks */
	for (int blk_idx = 0; blk_idx < fill_blocks; blk_idx++)
	{
		result = device->readBlock(blk_idx, block);
		if(verbose) printf("readBlock %d result %s\n", blk_idx, device->resultMessage(result));
		if(verbose) hexDump(block, block_size);
	}

	/* Read block that has not yet been written  */
	int blk = fill_blocks + 10;
	result = device->readBlock(blk, block);

	if(verbose) printf("readBlock %d result %s\n", blk, device->resultMessage(result));

	if (result == BlockDevice::success)
		if(verbose) hexDump(block, block_size);

	return 0;
}
