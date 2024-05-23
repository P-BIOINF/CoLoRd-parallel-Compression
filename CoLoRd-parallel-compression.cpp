#include "Parallel.h"
#include <iostream>
#include <filesystem>

int main(const int argc, char** argv)
{
	std::ios_base::sync_with_stdio(false);

	const std::string announcement{ "Something went wrong! Please make sure that you have included:\n"
		"--output <output directory> --input <input directory> --colord <colord directory> -m <colord mode> --lpthread <maximum number of threads> --maxFiles <max number of files to output> --count <sequences how often to change the file>\n" };

	Parallel parallel{};

	if (parallel.parseArguments(argc, argv) != Status::ready)
	{
		std::cerr << "Status code: " << static_cast<int>(parallel.getStatus()) << '\n';
		std::cerr << announcement;
		return -1;
	}

	if (!parallel.getInputStream().good())
	{
		std::cerr << announcement;
		return -2;
	}

	if (!parallel.createFiles())
	{
		std::cerr << announcement;
		return -3;
	}

	parallel.compress();

	return 0;
}