#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <sstream>

enum Failed
{
	no,
	yes,

	max_failed,
};

struct Colord
{
	std::string path{};
	std::string mode{};
	std::string arguments{};
};

int main(const int argc, char** argv)
{
	std::string input{};
	std::string output{};
	Colord colord{};
	int numberOfFilesToOutput{};
	Failed failed{no};

	for (int i{ 0 }; i < argc - 1; ++i)
	{
		if (std::string param{ argv[i] }; param == "-i")
			input = argv[++i];
		else if (param == "-o")
			output = argv[++i];
		else if (param == "-c")
		{
			try
			{
				numberOfFilesToOutput = std::stoi(argv[++i]);
			}
			catch (...)
			{
				failed = yes;

				break;
			}
		}
		else if (param == "-a")
		{
			colord.path = argv[++i];
		}
		else if (param == "-m")
		{
			colord.mode.append(" ").append(argv[++i]);
		}
		else if (param == "-k" || param == "--kmer - len")
		{
			colord.arguments.append(" -k ").append(argv[++i]);
		}
		else if (param == "-t" || param == "--threads")
		{
			colord.arguments.append(" -t ").append(argv[++i]);
		}
		else if (param == "-p" || param == "--priority")
		{
			colord.arguments.append(" -p ").append(argv[++i]);
		}
		else if (param == "-q" || param == "--qual")
		{
			colord.arguments.append(" -q ").append(argv[++i]);
		}
		else if (param == "-T" || param == "--qual-thresholds")
		{
			colord.arguments.append(" -T ").append(argv[++i]);
		}
		else if (param == "-D" || param == "--qual-values")
		{
			colord.arguments.append(" -D ").append(argv[++i]);
		}
		else if (param == "-G" || param == "--reference-genome")
		{
			colord.arguments.append(" -DG").append(argv[++i]);
		}
		else if (param == "-s" || param == "--store-reference")
		{
			colord.arguments.append(" -s ").append(argv[++i]);
		}
		else if (param == "-v" || param == "--verbose")
		{
			colord.arguments.append(" -v ").append(argv[++i]);
		}
	}

	colord.arguments.append(" ");

	std::ifstream inputStream{ input };
	std::ofstream outputStream{};
	std::ofstream logStream{ output.substr(0,output.find_last_of('\\') == std::string::npos ? 0 : output.find_last_of('\\')+1).append("logs.txt")};

	if(!inputStream || colord.mode.empty() || colord.path.empty())
	{
		std::cerr << "There was a problem!\n"
			    "Try using:\n"
				"-o <output directory> -i <input directory> -a <colord directory> -m {colord mode}\n";
		return -1;
	}

	if(failed != no)
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
		if(line[0] == '@' && line.find("length") != std::string::npos)
			++count;
	}


	inputStream.clear();
	inputStream.seekg(std::ios_base::beg);

	int repEvery{ count / numberOfFilesToOutput };

	std::vector<std::string> directories{};
	std::size_t index{0};
	std::size_t current{0};
		
	while (std::getline(inputStream, line))
	{
		if (line[0] == '@' && line.find("length") != std::string::npos)
		{
			if (current % repEvery == 0)
			{
				outputStream.close();
				auto tempString{ output};
				outputStream.open(tempString.insert(output.length(), std::to_string(++index)).append(".fastq"));
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
	//czemu bez tego nie dziala?

	std::vector<std::size_t> sizesWithoutCompression{};
	std::vector<std::size_t> sizesWithCompression{};

	for(const auto& path: directories)
	{
		sizesWithoutCompression.emplace_back(file_size(std::filesystem::path(path)));
		std::string tempOutput{path.substr(0, path.find_last_of('.')).append("c.fastqcomp") };
		std::string temp{" " + colord.path};
		temp.append(colord.mode).append(colord.arguments).append(path).append(" " + tempOutput);

		std::system(temp.data());
		sizesWithCompression.emplace_back(file_size(std::filesystem::path(tempOutput)));
		std::cout << std::endl;
	}

	const std::size_t originalSizeWithoutCompression{ std::filesystem::file_size(input )};
	//const std::size_t originalSizeWithoutCompression{ std::filesystem::file_size(std::filesystem::current_path().append(input).string()) };
	std::string tempOutput{input.substr(0, input.find_last_of('.')).append("c.fastqcomp") };
	std::string temp{ " " + colord.path };
	temp.append(colord.mode).append(colord.arguments).append(input).append(" " + tempOutput);
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
		sstream << "\nAverage compression ratio: " << avgRatio << '\n';
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
		sstream << std::setprecision(3) << std::fixed << "Delta: " << avgRatio-ratio << "\n";
		std::cout << sstream.view();
		logStream << sstream.view();
	}

	return 0;
}