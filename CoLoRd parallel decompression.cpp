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
	std::ofstream outputStream{ };
	std::ofstream logStream{ output.substr(0,output.find_last_of('\\') == std::string::npos ? 0 : output.find_last_of('\\')+1).append("logs.txt")};


	if(!inputStream || !outputStream || colord.mode.empty() || colord.path.empty())
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
	std::size_t index{};
	std::size_t current{};

	while (std::getline(inputStream, line))
	{
		if (line[0] == '@' && line.find("length") != std::string::npos)
		{
			if (current % repEvery == 0)
			{
				outputStream.close();
				auto tempString{ output};
				outputStream.open(tempString.insert(output.length(), std::to_string(++index)).append(".fastq"));
				directories.emplace_back(std::filesystem::current_path().append(tempString).string());
			}
			++current;
		}
		outputStream << line << '\n';
	}
	for (const auto& path : directories)
		std::cout << path << '\n';

	std::vector<std::size_t> sizesWithoutCompression(index);
	std::vector<std::size_t> sizesWithCompression(index);

	for(const auto& path: directories)
	{
		sizesWithoutCompression.emplace_back(file_size(std::filesystem::path(path)));
		std::string tempOutput{ " " + path.substr(0, path.find_last_of('.')).append("c.fastq") };
		std::string temp{colord.path};
		temp.append(colord.mode).append(colord.arguments).append(path).append(tempOutput);

		std::system(temp.data());
		sizesWithCompression.emplace_back(file_size(std::filesystem::path(tempOutput)));
	}

	const std::size_t originalSizeWithoutCompression{ std::filesystem::file_size(input )};
	//const std::size_t originalSizeWithoutCompression{ std::filesystem::file_size(std::filesystem::current_path().append(input).string()) };
	std::string tempOutput{ " " + input.substr(0, input.find_last_of('.')).append("c.fastq") };
	std::string temp{ colord.path };
	temp.append(colord.mode).append(colord.arguments).append(input).append(tempOutput);
	std::system(temp.data());
	std::size_t originalSizeWithCompression{ file_size(std::filesystem::path(tempOutput)) };


	std::stringstream sstream{};
	sstream << std::setprecision(3) << "Size of the original file W/O compression: " << originalSizeWithoutCompression / static_cast<long double>(1024)
	<< "kbs\tW/ compression: " << originalSizeWithCompression / static_cast<long double>(1024) << "kbs\tCompression ratio: " << originalSizeWithoutCompression / originalSizeWithCompression << '\n' << "kbs\n\n\n";

	std::cout << sstream.str();
	logStream << sstream.str();
	sstream.ignore(std::numeric_limits<std::streamsize>::max());
	sstream.ignore(std::numeric_limits<std::streamsize>::max());
	sstream.ignore(std::numeric_limits<std::streamsize>::max());
	std::size_t avgRatio{};

	for(std::size_t i{0}; i < index; ++i)
	{
		auto ratio{ sizesWithoutCompression[i] / sizesWithCompression[i] };
		avgRatio += ratio;	

		std::stringstream tempSstream{};
		tempSstream << std::setprecision(3) << "Size of file #" << i << "\nW/O compression: " << sizesWithoutCompression[i] / static_cast<long double>(1024)
			<< "kbs\tW/ compression: " << sizesWithCompression[i] / static_cast<long double>(1024) << "kbs\tCompression ratio: " << ratio << '\n';

		logStream << tempSstream.str();
		std::cout << tempSstream.str();
	}
	avgRatio /= index;

	sstream << "\n\nAverage compression ratio: " << avgRatio << '\n';
	logStream << sstream.str();
	std::cout << sstream.str();

	return 0;
}