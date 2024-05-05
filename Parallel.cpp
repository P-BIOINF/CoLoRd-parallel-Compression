#include "Parallel.h"
#include <execution>
#include "Timer.h"
#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <filesystem>
#include <unordered_set>

Status Parallel::parseArguments(const int argc, char** argv)
{
	const std::unordered_set<std::string> setOfTwoParams{ "-k", "--kmer - len", "-t", "--threads", "-p", "--priority", "-q", "--qual", "-T","--qual-thresholds", "-D", "--qual-values", "-i", "--identifier", "-R", "--Ref-reads-mod" };
	const std::unordered_set<std::string> setOfOneParam{ "-G", "--reference-genome", "-s", "--store-reference","-v","--verbose", "-a", "--anchor-len", "-L", "--Lowest-count", "-H", "--Highest-count", "-f", "--filter-modulo", "---max-candidates", "-c", "-e", "--edit-script-mult", "-r", "--max-recurence-level", "--min-to-alt", "--min-mmer-frac", "--min-mmer-force-enc", "--max-matches-mult", "--fill-factor-filtered-kmers", "--fill-factor-kmers-to-reads", "--min-anchors", "-x", "--sparse-exponent", "-g", "--sparse-range", "-h", "--help" };
	for (int i{ 0 }; i < argc; ++i)
	{
		if (std::string param{ argv[i] }; param == "--input")
		{
			m_input = argv[++i];
			m_extension = std::filesystem::path(m_input).extension();
		}
		else if (param == "--output")
		{
			m_output = std::string(argv[++i]) + "\\";
			m_output.remove_filename();
		}
		else if (param == "--count")
		{
			try
			{
				m_maxNumberOfFilesToOutput = std::stoul(argv[++i]);
			}
			catch (const std::invalid_argument& ex)
			{
				std::cerr << ex.what() << '\n';
				m_maxNumberOfFilesToOutput = 0;
			}
		}
		else if (param == "--lpthread")
		{
			try
			{
				m_threads = std::stoi(argv[++i]);
			}
			catch (const std::invalid_argument& ex)
			{
				std::cerr << ex.what() << '\n';
				m_threads = -1;
			}
		}
		else if (param == "--test")
		{
			try
			{
				m_test = std::stoul(argv[++i]);
			}
			catch (...)
			{
				m_status = Status::failed;

				return getStatus();
			}
		}
		else if (param == "--colord")
			m_path = argv[++i];
		else if (param == "-m")
			m_mode.append(" ").append(argv[++i]);
		else if (setOfTwoParams.contains(param))
			m_arguments.append(" " + param + " ").append(argv[++i]);
		else if (setOfOneParam.contains(param))
			m_arguments.append(" " + param);
	}
	m_arguments.append(" ");
	if (m_path.empty() || m_mode.empty() || m_maxNumberOfFilesToOutput == 0 || m_threads == -1)
	{
		m_status = Status::not_ready;
		return getStatus();
	}
	getInputStream().open(getInput());
	std::filesystem::create_directory(getOutput());
	if(!getInputStream())
	{
		m_status = Status::failed;
		return getStatus();
	}
	std::filesystem::path tempPath{ getOutput() };
	tempPath /= "logs.txt";
	getLogsStream().open(tempPath);
	if(!getLogsStream().good())
	{
		m_status = Status::failed;
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
	m_repEvery = m_count / m_maxNumberOfFilesToOutput;
}

bool Parallel::createFiles()
{
	std::vector<std::ofstream> openOutputStreams{};
	std::filesystem::create_directory(m_output);
	for (int index{ 0 }; index == 0 || (index < m_maxNumberOfFilesToOutput) ; ++index)
	{
		std::filesystem::path tempPath{ m_output };
		tempPath.append(std::to_string(index + 1) + m_extension.string());
		openOutputStreams.emplace_back(std::ofstream{ tempPath });
		m_directories.emplace_back(tempPath);
	}
	std::string identifier{}; 
	std::string sequence{};
	std::string signAndIdentifier{};
	std::string qualityScores{};
	std::uint64_t currentSequence{0};
	int currentFile{0};
	while (std::getline(getInputStream(), identifier))
	{
		std::getline(getInputStream(), sequence);
		std::getline(getInputStream(), signAndIdentifier);
		std::getline(getInputStream(), qualityScores);
		openOutputStreams[currentFile] << identifier << '\n' << sequence << '\n' << signAndIdentifier << '\n' << qualityScores << '\n';
		if (++currentSequence; currentSequence % m_test == 0)
		{
			openOutputStreams[currentFile].flush();
			++currentFile;
			if (currentFile == std::ssize(openOutputStreams))
				currentFile = 0;
		}
	}
	return true;
}

void Parallel::compress()
{
	m_sizesWithoutCompression.resize(m_directories.size());
	m_times.resize(m_directories.size());
	m_sizesWithCompression.resize(m_directories.size());
	std::vector<std::thread> threads{};
	threads.reserve(m_threads);
	for (int i = 0; i < m_threads; i++)
		threads.emplace_back([this]() { this->handleCompression(); });
	for (auto& thread : threads)
		thread.join();
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

void Parallel::handleCompression()
{
	static std::atomic<int> pathIndex{ 0 };
	int index{};
	while ((index = pathIndex++) < m_directories.size())
		systemCompression(index);
}

void Parallel::systemCompression(const int index)
{
	m_sizesWithoutCompression[index] = std::filesystem::file_size(m_directories[index]);
	std::filesystem::path tempPath{ m_directories[index] };
	tempPath.replace_extension(m_extension.string() + "colord");
	std::string temp{ ' ' + m_path.string() + m_mode + m_arguments + m_directories[index].string() + ' ' + tempPath.string() };
	Timer timer{};
	std::system(temp.c_str());
	m_times[index] = timer.elapsed();
	m_sizesWithCompression[index] = std::filesystem::file_size(tempPath);
	std::cout << '\n';
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
	std::cout << sStream.view() << '\n';
	getLogsStream() << sStream.view() << '\n';
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