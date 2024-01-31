#include <iostream>
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

	parallel.parseArguments(argc, argv);
		
	/*std::ifstream inputStream{ colord.getInput()};
	std::ofstream outputStream{};
	std::ofstream logStream{ colord.getOutput().substr(0,colord.getOutput().find_last_of('\\') == std::string::npos ? 0 : colord.getOutput().find_last_of('\\') + 1).append("logs.txt")};*/

	if(!inputStream || colord.getMode().empty() || colord.getPath().empty())
	{
		std::cerr << "There was a problem!\n"
			    "Try using:\n"
				"-o <output directory> -i <input directory> -a <colord directory> -m {colord mode}\n";
		return -1;
	}

	if(colord.getFailed() != no)
	{
		std::cerr << "Invalid count\n"
				"Try using:\n"
				"-c {int}\n";
		return -1;
	}

	int count{ 0 };
	std::string line{};
	while (std::getline(inputStream,line))
	{
		if(line[0] == '@' && line.find("@ERR") != std::string::npos)
			++count;
	}


	inputStream.clear();
	inputStream.seekg(std::ios_base::beg);

	int repEvery{ count / colord.getNumberOfFilesToOutput() };

	std::vector<std::string> directories{};
	std::size_t index{0};
	std::size_t current{0};
		
	while (std::getline(inputStream, line))
	{
		if (line[0] == '@' && line.find("@ERR") != std::string::npos)
		{
			if (current % repEvery == 0 && index < colord.getNumberOfFilesToOutput())
			{
				outputStream.close();
				auto tempString{ colord.getOutput()};
				outputStream.open(tempString.insert(colord.getOutput().length(), std::to_string(++index)).append(".fastq"));
				if(!outputStream)
				{
					std::cerr << "There was a problem!\n"
						"Try using:\n"
						"-o <output directory> -i <input directory> -a <colord directory> -m {colord mode}\n";
					return -1;
				}
				directories.emplace_back(std::filesystem::current_path().append(tempString).string());
			}
			++current;
		}
		outputStream << line << '\n';
	}
	outputStream.flush();

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