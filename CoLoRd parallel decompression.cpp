#include <cstdio>
#include <filesystem>

#include "Parallel.h"



int main(const int argc, char** argv)
{
	Parallel parallel{};

	if(parallel.parseArguments(argc, argv) != Status::ready)
	{
		
	}

	parallel.getInputStream();

	/*std::ifstream inputStream{  parallel.getInput()};
	std::ofstream outputStream{};
	std::ofstream logStream{  parallel.getOutput().substr(0, parallel.getOutput().find_last_of('\\') == std::string::npos ? 0 :  parallel.getOutput().find_last_of('\\') + 1).append("logs.txt")};*/

	//Od lini 29 do 47 trzeba pozmieniaæ i dodaæ wywo³anie funkcji klasy Parallel w odpowiedniej kolejnoœci

	if(!inputStream || parallel.getMode().empty() || parallel.getPath().empty())
	{
		fprintf(stderr, "There was a problem!\n"
			    "Try using:\n"
				"-o <output directory> -i <input directory> -a <colord directory> -m {colord mode}\n");
		return -1;
	}

	if (parallel.getStatus() != no)
	{
		fprintf(stderr, "Invalid count\n"
				"Try using:\n"
				"-c {int}\n");
		return -1;
	}
	return 0;
}