#include "Parallel.h"
#include <iostream>
#include <filesystem>


int main(const int argc, char** argv)
{
	std::ios_base::sync_with_stdio(false);

	Parallel parallel{};

	if (parallel.parseArguments(argc, argv) != Status::ready)
	{
		std::cerr << "Status code: " << static_cast<int>(parallel.getStatus()) << '\n';
		std::cerr << "Something went wrong! Please make sure that you have included:\n"
			"--output <output directory> --input <input directory> --colord <colord directory> -m {colord mode} --lpthread <maximum number of threads> --count {count}\n";

		return -1;
	}

	if (!parallel.getInputStream().good())
	{
		std::cerr << "Something went wrong! Please make sure that you have included:\n"
			"--output <output directory> --input <input directory> --colord <colord directory> -m {colord mode} --lpthread <maximum number of threads> --count {count}\n";

		return -2;
	}

	parallel.calculateCount();
	if (!parallel.createFiles())
	{
		std::cerr << "Something went wrong! Please make sure that you have included:\n"
			"--output <output directory> --input <input directory> --colord <colord directory> -m {colord mode} --lpthread <maximum number of threads> --count {count}\n";

		return -3;
	}

	parallel.compress();
	return 0;
}