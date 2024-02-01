#include <iostream>
#include <filesystem>
#include "Parallel.h"



int main(const int argc, char** argv)
{
	Parallel parallel{};

	if(parallel.parseArguments(argc, argv) != Status::ready)
	{
		std::cerr << "Status code: "<< static_cast<int>(parallel.getStatus()) << '\n';
		std::cerr << "Something went wrong! Please make sure that you have included:\n"
			"-o <output directory> -i <input directory> -a <colord directory> -m {colord mode} -c {count}\n";

		return -1;
	}

	parallel.getInputStream().open(parallel.getInput());
	parallel.getOutputStream().open(std::filesystem::current_path().append("/logs/logs.txt"));

	parallel.calculateCount();
	if (!parallel.createFiles())
		return -1;

	parallel.calculateFileSizes();
	parallel.averageRatio();
	parallel.printFileSizes();
	parallel.totalSequences();

	return 0;
}