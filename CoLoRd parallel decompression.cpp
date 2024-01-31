#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <sstream>

#include "Parallel.h"



int main(const int argc, char** argv)
{
	Parallel parallel{};

	if(parallel.parseArguments(argc, argv) != Status::ready)
	{
		
	}

	parallel.getInputStream();

	/*std::ifstream inputStream{ colord.getInput()};
	std::ofstream outputStream{};
	std::ofstream logStream{ colord.getOutput().substr(0,colord.getOutput().find_last_of('\\') == std::string::npos ? 0 : colord.getOutput().find_last_of('\\') + 1).append("logs.txt")};*/

	//Od lini 29 do 45 trzeba pozmieniaæ lub gdzieœ indziej ustawiæ ni¿ w mainie np. dodam metode

	if(!inputStream || colord.getMode().empty() || colord.getPath().empty())
	{
		fprintf(stderr, "There was a problem!\n"
			    "Try using:\n"
				"-o <output directory> -i <input directory> -a <colord directory> -m {colord mode}\n");
		return -1;
	}

	if(colord.getFailed() != no)
	{
		fprintf(stderr, "Invalid count\n"
				"Try using:\n"
				"-c {int}\n");
		return -1;
	}

	//Do t¹d da³em do klasy, dokoñczyæ trzeba

	std::vector<std::size_t> sizesWithoutCompression{};
	std::vector<std::size_t> sizesWithCompression{};

	for(const auto& path: directories)
	{
		sizesWithoutCompression.emplace_back(file_size(std::filesystem::path(path)));
		std::string tempOutput{path.substr(0, path.find_last_of('.')).append("c.fastqcomp") };
		std::string temp{" " + colord.getPath()};
		temp.append(colord.getMode()).append(colord.getArguments()).append(path).append(" " + tempOutput);

		std::system(temp.data());
		sizesWithCompression.emplace_back(file_size(std::filesystem::path(tempOutput)));
		std::cout << std::endl;
	}

	const std::size_t originalSizeWithoutCompression{ std::filesystem::file_size(colord.getInput( ))};
	//const std::size_t originalSizeWithoutCompression{ std::filesystem::file_size(std::filesystem::current_path().append(input).string()) };
	std::string tempOutput{ colord.getInput().substr(0, colord.getInput().find_last_of('.')).append("c.fastqcomp") };
	std::string temp{ " " + colord.getPath()};
	temp.append(colord.getMode()).append(colord.getArguments()).append(colord.getInput()).append(" " + tempOutput);
	std::system(temp.data());
	std::size_t originalSizeWithCompression{ file_size(std::filesystem::path(tempOutput)) };


	long double avgRatio{};
	std::cout << '\n';
	for(std::size_t i{0}; i < index; ++i)
	{
		const auto ratio{ sizesWithoutCompression[i] / static_cast<long double> (sizesWithCompression[i]) };
		avgRatio += ratio;	
		std::stringstream tempStream{};
		tempStream << std::setprecision(3) << std::fixed << "Size of file #" << i + 1 << ":\nw/o compression: " << sizesWithoutCompression[i] / static_cast<long double>(1024)
			<< "kbs\tw/ compression: " << sizesWithCompression[i] / static_cast<long double>(1024) << "kbs\tcompression ratio: " << ratio << '\n';
		logStream << tempStream.view();
		std::cout << tempStream.view();
	}
	avgRatio /= index;

	{
		std::stringstream sstream{};
		sstream << std::setprecision(3) << std::fixed << "\nAverage compression ratio: " << avgRatio << '\n';
		logStream << sstream.view();
		std::cout << sstream.view();
	}

	auto ratio{ originalSizeWithoutCompression / static_cast<long double>(originalSizeWithCompression) };
	{
		std::stringstream sstream{};
		sstream << std::setprecision(3) << std::fixed << "Size of the original file w/o compression: " << originalSizeWithoutCompression / static_cast<long double>(1024)
			<< "kbs\tw/ compression: " << originalSizeWithCompression / static_cast<long double>(1024) << "kbs\tcompression ratio: " << ratio << "\n\n";
		std::cout << sstream.view();
		logStream << sstream.view();
	}

	{
		std::stringstream sstream{};
		sstream << std::setprecision(3) << std::fixed << "Total sequences: " << count <<"\tDelta: " << ratio - avgRatio<< "\n";
		std::cout << sstream.view();
		logStream << sstream.view();
	}

	return 0;
}