#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <iostream>

#include "../inc/BlockDevice.h"
#include "../inc/hexdump.h"
#include "FileSystem.h"

void printHelp()
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
			printHelp();
			break;

		case '?':
			if (verbose)
				printf("\n%s is an invalid argument, exiting...\n", optarg);

		default:
			break;
		}
	}
	printf("arguments parsed...       \n");
	printf("number of blocks used:  %i\n", number_blocks);
	printf("disk file:              %s\n", disk_file);
	printf("command file:           %s\n", cmd_file);

	// warn the user if cmd are missing
	// exit if any required params are missing
	if (cmd_file == nullptr)
	{
		if (verbose)
			printf("\ncommand file is required, exiting...\n");
		exit(0);
	}

	// -------------------------------
	// LOAD OR CREATE THE BLOCK DEVICE
	// -------------------------------
	if (verbose)
		printf("\ninstantiating the block device...\n");
	BlockDevice *device;
	struct stat statbuf;
	bool created = false;
	// See if the the disk exists
	if (stat(disk_file, &statbuf) == -1)
	{
		// couldn't stat, try to create new disk
		device = new BlockDevice(disk_file, number_blocks);
		created = true;
		if (verbose)
			printf("creating new disk...\n");
	}
	else
	{
		// otherwise load the exisiting disk from the .dsk file
		device = new BlockDevice(disk_file);
		if (verbose)
			printf("loading existing disk from file...\n");
	}

	// ------------------
	// RUN THE SIMULATION
	// ------------------
	if (verbose)
		printf("\nstarting the siumulation...\n");
	FileSystem *fs = new FileSystem(device);

	// parse the commands in the command file
	// run each command and print out results

	// clean up and close
	if (verbose)
		printf("done...\n\n");
	return 0;
}
