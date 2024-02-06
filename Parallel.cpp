#include "Parallel.h"
#include "Timer.h"
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_set>

Status Parallel::parseArguments(const int argc, char** argv)
{
	std::unordered_set<std::string> setOfTwoParams{ "-k", "--kmer - len", "-t", "--threads", "-p", "--priority", "-q", "--qual", "-T","--qual-thresholds", "-D", "--qual-values", "-i", "--identifier", "-R", "--Ref-reads-mod" };
	std::unordered_set<std::string> setOfOneParam{ "-G", "--reference-genome", "-s", "--store-reference","-v","--verbose", "-a", "--anchor-len", "-L", "--Lowest-count", "-H", "--Highest-count", "-f", "--filter-modulo", "---max-candidates", "-c", "-e", "--edit-script-mult", "-r", "--max-recurence-level", "--min-to-alt", "--min-mmer-frac", "--min-mmer-force-enc", "--max-matches-mult", "--fill-factor-filtered-kmers", "--fill-factor-kmers-to-reads", "--min-anchors", "-x", "--sparse-exponent", "-g", "--sparse-range", "-h", "--help" };
	for (int i{ 0 }; i < argc - 1; ++i)
	{
		if (std::string param{ argv[i] }; param == "--input")
		{
			m_input = argv[++i];
			m_extension = std::filesystem::path(m_input).extension();
		}
		else if (param == "-o")
		{
			m_output = argv[++i];
			m_output.remove_filename();
		}
		else if (param == "--count")
		{
			try
			{
				m_numberOfFilesToOutput = std::stoul(argv[++i]);
			}
			catch (...)
			{
				m_status = Status::failed;

				return getStatus();
			}
		}
		else if (param == "--colord")
		{
			m_path = argv[++i];
		}
		else if (param == "-m")
		{
			m_mode.append(" ").append(argv[++i]);
		}
		else if (setOfTwoParams.find(param) != setOfTwoParams.end())
		{
			m_arguments.append(" " + param + " ").append(argv[++i]);
		}
		else if (setOfOneParam.find(param) != setOfOneParam.end())
		{
			m_arguments.append(" " + param);
		}
	}
	m_arguments.append(" ");

	if (m_path.empty() && m_mode.empty())
	{
		m_status = Status::not_ready;

		return getStatus();
	}

	m_status = Status::ready;

	return getStatus();
}

void Parallel::calculateCount()
{
	std::string identifier{};
	std::string sequence{};
	std::string signAndIdentifier{};
	std::string qualityScores{};

	while(std::getline(getInputStream(), identifier))
	{
		std::getline(getInputStream(), sequence);
		std::getline(getInputStream(), signAndIdentifier);
		std::getline(getInputStream(), qualityScores);
		++m_count;

	}
	
	getInputStream().clear();
	getInputStream().seekg(std::ios_base::beg);
	m_repEvery = m_count / m_numberOfFilesToOutput;
}

bool Parallel::createFiles()
{
	std::string identifier{};
	std::string sequence{};
	std::string signAndIdentifier{};
	std::string qualityScores{};

	std::size_t current{ 0 };
	std::size_t index{};
	std::filesystem::create_directory(m_output);

	while(std::getline(getInputStream(),identifier))
	{
		std::getline(getInputStream(), sequence);
		std::getline(getInputStream(), signAndIdentifier);
		std::getline(getInputStream(), qualityScores);

		if(current++ % m_repEvery == 0 && index < m_numberOfFilesToOutput)
		{
			getOutputStream().close();
			std::filesystem::path tempPath{ m_output };
			getOutputStream().open(tempPath.append(std::to_string(++index) + m_extension.string()));

			if (!getOutputStream())
				return false;
			m_directories.emplace_back(tempPath);
		}

		getOutputStream() << identifier << '\n' << sequence<< '\n' << signAndIdentifier<< '\n' << qualityScores <<'\n';
	}

	getOutputStream().flush();
	return true;
}

void Parallel::compress()
{
	for (const auto& path : m_directories)
	{
		m_sizesWithoutCompression.emplace_back(std::filesystem::file_size(path));
		std::filesystem::path tempPath{path};
		tempPath.replace_extension(m_extension.string() + "colord");
		std::string temp{" " + m_path.string() + m_mode + m_arguments + path.string() + " " + tempPath.string()};
		Timer timer{};
		std::system(temp.c_str());
		m_times.emplace_back(timer.elapsed());
		m_sizesWithCompression.emplace_back(std::filesystem::file_size(tempPath));
		std::cout << '\n';
	}

	m_originalSizeWithoutCompression = std::filesystem::file_size(m_input);

	std::cout<< '\n';
	for (std::size_t i{ 0 }; i < m_directories.size(); ++i)
	{
		const auto ratio{ m_sizesWithoutCompression[i] / static_cast<long double> (m_sizesWithCompression[i]) };
		m_avgRatio += ratio;
		std::stringstream tempStream{};
		tempStream << std::setprecision(3) << std::fixed << "Size of file #" << i + 1 << ":\nw/o compression: " << m_sizesWithoutCompression[i] 
			<< "\tw/ compression: " << m_sizesWithCompression[i]  << "\tcompression ratio: " << ratio << "\trun time: " << m_times[i]<< '\n';
		getLogsStream() << tempStream.view();
		std::cout << tempStream.view();
	}
	m_avgRatio /= m_directories.size();
}

void Parallel::printAvgRatio()
{
	std::stringstream sStream{};
	sStream << std::setprecision(3) << std::fixed << "\nAverage compression ratio:\t\t\t\t" << m_avgRatio << '\n';
	getLogsStream() << sStream.view();
	std::cout << sStream.view();
}

void Parallel::printFileSizes()
{
	std::stringstream sStream{};
	std::size_t totalSize{0};
	for (const auto element : m_sizesWithCompression)
		totalSize += element;

	sStream << std::setprecision(3) << std::fixed << "\nSize of the input file:\nw/o Compression:\t\t\t\t\t" << m_originalSizeWithoutCompression;
	std::cout << sStream.view();
	getLogsStream() << sStream.view();
}

void Parallel::totalSequences()
{	
	std::stringstream sStream{};
	double tempTime{};
	for (const auto element : m_times)
		tempTime += element;

	sStream << std::setprecision(3) << std::fixed << "Total sequences:\t\t\t\t\t" << m_count << "\nCompression ratio delta:\t\t\t\t" << m_avgRatio - m_ratio << "\nCompression time:\t\t\t\t\t"<< tempTime <<"\n";
	std::cout << sStream.view();
	getLogsStream() << sStream.view();
}