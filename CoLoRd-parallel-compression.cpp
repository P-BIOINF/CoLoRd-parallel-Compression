#include "Parallel.h"

int main(const int argc, char** argv)
{
	std::chrono::high_resolution_clock::time_point start{ std::chrono::high_resolution_clock::now() };
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

	std::chrono::high_resolution_clock::time_point end{ std::chrono::high_resolution_clock::now() };
	displayTime("Elapsed time during the entire program", start, end);

	return 0;
}