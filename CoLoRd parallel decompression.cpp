#include <iostream>
#include <filesystem>
#include "Parallel.h"
#include "Timer.h"


int main(const int argc, char** argv)
{
	std::ios_base::sync_with_stdio(false);

	Timer timer{};
	Parallel parallel{};

	if(parallel.parseArguments(argc, argv) != Status::ready)
	{
		std::cerr << "Status code: "<< static_cast<int>(parallel.getStatus()) << '\n';
		std::cerr << "Something went wrong! Please make sure that you have included:\n"
			"-o <output directory> -i <input directory> -a <colord directory> -m {colord mode} -c {count}\n";

		return -1;
	}

	parallel.getInputStream().open(parallel.getInput());
	std::filesystem::create_directory(parallel.getOutput());
	std::filesystem::path tempPath{parallel.getOutput()};
	tempPath /= "logs.txt";
	parallel.getLogsStream().open(tempPath);

	if(!parallel.getInputStream().good() || !parallel.getLogsStream().good())
	{
		std::cerr << "Something went wrong! Please make sure that you have included:\n"
			"-o <output directory> -i <input directory> -a <colord directory> -m {colord mode} -c {count}\n";

		return -2;
	}

	parallel.calculateCount();
	if (!parallel.createFiles())
	{
		std::cerr << "Something went wrong! Please make sure that you have included:\n"
			"-o <output directory> -i <input directory> -a <colord directory> -m {colord mode} -c {count}\n";

		return -3;
	}

	parallel.compress();
	parallel.printAvgRatio();
	parallel.printFileSizes();
	parallel.totalSequences();

	return 0;
}