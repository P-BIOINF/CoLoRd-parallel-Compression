#include <iostream>
#include <filesystem>
#include "Parallel.h"



int main(const int argc, char** argv)
{
	Parallel parallel{};

	if(parallel.parseArguments(argc, argv) != Status::ready)
	{
		std::cerr << "Something went wrong!\n"
			"Please make sure that you have included:\n"
			"-o <output directory> -i <input directory> -a <colord directory> -m {colord mode} -c {int}\n";

		return -1;
	}

	parallel.getInputStream().open(parallel.getInput());
	parallel.getOutputStream().open(std::filesystem::current_path().append("/logs/logs.txt"));

	return 0;
}