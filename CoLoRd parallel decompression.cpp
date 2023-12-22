#include <iostream>
#include <fstream>
#include <string>
#include <charconv>
#include <string_view>

enum Failed
{
	no,
	yes,

	max_failed,
};

int main(const int argc, char** argv)
{
	std::string input{};
	std::string output{};
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
			
	}

	std::ifstream inputStream{ input };
	std::ofstream outputStream{ };
	std::ofstream logStream{ output.substr(0,output.find_last_of('\\') == std::string::npos ? 0 : output.find_last_of('\\')+1).append("logs.txt")};


	if(!inputStream || !outputStream)
	{
		std::cerr << "There was a problem with loading the files\n"
			    "Try using:\n"
				"-o {output.*} -i {input.*}\n";
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

	int repEvery{count/numberOfFilesToOutput};

	inputStream.clear();
	inputStream.seekg(std::ios_base::beg);

	int index{};
	int current{};
	while(std::getline(inputStream, line))
	{
		if (line[0] == '@' && line.find("length") != std::string::npos)
		{
			if(current % repEvery == 0)
			{
				outputStream.close();
				const auto temp{ output.find_last_of('.') };
				const auto pos{ temp == std::string::npos ? output.length():  temp};
				std::string tempString{output};
				outputStream.open(tempString.insert(pos,std::to_string(++index)));
			}
			++current;
		}
		outputStream << line << '\n';
	}

	logStream
	<< "Number of segments: " << count << '\n'
	<< "Number of files generated: index: " << index << '\n';

	return 0;
}