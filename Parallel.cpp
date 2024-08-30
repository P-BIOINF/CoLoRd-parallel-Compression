#include "Parallel.h"
#include <execution>
#include <thread>
#include <fstream>
#include <iomanip>
#include <unordered_set>

void displayTime(std::string message, const std::chrono::high_resolution_clock::time_point& start, const std::chrono::high_resolution_clock::time_point& end) 
{
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
	int hours = duration / 3600;
	int minutes = (duration % 3600) / 60;
	int seconds = duration % 60;
	std::cerr << "--------------------------------------------\n";
	std::cerr << message << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << seconds << '\n';
	std::cerr << "--------------------------------------------\n";
}

Status Parallel::parseArguments(const int argc, char** argv)
{
	const std::unordered_set<std::string> setOfTwoParams{ "-k", "--kmer - len", "-t", "--threads", "-p", "--priority", "-q", "--qual", "-T","--qual-thresholds", "-D", "--qual-values", "-i", "--identifier", "-R", "--Ref-reads-mod" };
	const std::unordered_set<std::string> setOfOneParam{ "-G", "--reference-genome", "-s", "--store-reference","-v","--verbose", "-a", "--anchor-len", "-L", "--Lowest-count", "-H", "--Highest-count", "-f", "--filter-modulo", "---max-candidates", "-c", "-e", "--edit-script-mult", "-r", "--max-recurence-level", "--min-to-alt", "--min-mmer-frac", "--min-mmer-force-enc", "--max-matches-mult", "--fill-factor-filtered-kmers", "--fill-factor-kmers-to-reads", "--min-anchors", "-x", "--sparse-exponent", "-g", "--sparse-range", "-h", "--help" };
	for (std::int64_t i{ 0 }; i < argc; ++i)
	{
		if (std::string param{ argv[i] }; param == "--input")
		{
			m_input = argv[++i];
			m_extension = std::filesystem::path(m_input).extension();
		}
		else if (param == "--output")
		{
			m_output = std::string(argv[++i]);
			m_output.remove_filename();
		}
		else if (param == "--maxFiles")
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
				if (std::thread::hardware_concurrency() < m_threads)
					m_threads = std::thread::hardware_concurrency();
			}
			catch (const std::invalid_argument& ex)
			{
				std::cerr << ex.what() << '\n';
				m_threads = -1;
			}
		}
		else if (param == "--count")
		{
			try
			{
				m_count = std::stoul(argv[++i]);
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
	getInputStream().open(getInput(), std::ios::binary);
	std::filesystem::create_directory(getOutput());
	if (!getInputStream())
	{
		m_status = Status::failed;
		return getStatus();
	}

	m_status = Status::ready;
	return getStatus();
}


bool Parallel::createFiles()
{
	std::vector<std::ofstream> openOutputStreams{};
	std::filesystem::create_directory(m_output);
	for (int index{ 0 }; index == 0 || (index < m_maxNumberOfFilesToOutput); ++index)
	{
		std::filesystem::path tempPath{ m_output };
		tempPath.append(std::to_string(index + 1) + m_extension.string());
		openOutputStreams.emplace_back(std::ofstream{ tempPath, std::ios::binary});
		m_directories.emplace_back(tempPath);
	}
	std::string identifier{};
	std::string sequence{};
	std::string signAndIdentifier{};
	std::string qualityScores{};
	std::uint64_t currentSequence{ 0 };
	int currentFile{ 0 };
	while (std::getline(getInputStream(), identifier))
	{
		std::getline(getInputStream(), sequence);
		std::getline(getInputStream(), signAndIdentifier);
		std::getline(getInputStream(), qualityScores);
		openOutputStreams[currentFile] << identifier << '\n' << sequence << '\n' << signAndIdentifier << '\n' << qualityScores << '\n';
		if (++currentSequence; currentSequence % m_count == 0)
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
	std::vector<std::thread> threads{};
	threads.reserve(m_threads);
	m_compression_start = std::chrono::high_resolution_clock::now();
	for (std::int64_t i = 0; i < m_threads; i++)
		threads.emplace_back([this]() { this->handleCompression(); });
	for (auto& thread : threads)
		thread.join();
	m_compression_end = std::chrono::high_resolution_clock::now();
	displayTime("Time elapsed during compression: ", m_compression_start, m_compression_end);
}

void Parallel::handleCompression()
{
	int index{};
	while (true)
	{
		index = m_pathIndex.fetch_add(1, std::memory_order_relaxed);
		if (index >= std::ssize(m_directories))
			break;
		systemCompression(index);
	}
}

void Parallel::systemCompression(const int index) const
{
	std::filesystem::path tempPath{ m_directories[index] };
	tempPath.replace_extension(m_extension.string() + "colord");
	auto err = tempPath;
	err.replace_extension(".err");
	const std::string command{ ' ' + m_path.string() + m_mode + m_arguments + m_directories[index].string() + ' ' + tempPath.string() + " 2> " + err.string() };
	std::system(command.c_str());
}