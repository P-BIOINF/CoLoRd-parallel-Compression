#include "Parallel.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <fstream>
#include <filesystem>


Status Parallel::parseArguments(const int argc, char** argv)
{
	for (int i{ 0 }; i < argc - 1; ++i)
	{
		if (std::string param{ argv[i] }; param == "-i")
			m_input = argv[++i];
		else if (param == "-o")
			m_output = argv[++i];
		else if (param == "-c")
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
		else if (param == "-a")
		{
			m_path = argv[++i];
		}
		else if (param == "-m")
		{
			m_mode.append(" ").append(argv[++i]);
		}
		else if (param == "-k" || param == "--kmer - len")
		{
			m_arguments.append(" -k ").append(argv[++i]);
		}
		else if (param == "-t" || param == "--threads")
		{
			m_arguments.append(" -t ").append(argv[++i]);
		}
		else if (param == "-p" || param == "--priority")
		{
			m_arguments.append(" -p ").append(argv[++i]);
		}
		else if (param == "-q" || param == "--qual")
		{
			m_arguments.append(" -q ").append(argv[++i]);
		}
		else if (param == "-T" || param == "--qual-thresholds")
		{
			m_arguments.append(" -T ").append(argv[++i]);
		}
		else if (param == "-D" || param == "--qual-values")
		{
			m_arguments.append(" -D ").append(argv[++i]);
		}
		else if (param == "-G" || param == "--reference-genome")
		{
			m_arguments.append(" -DG").append(argv[++i]);
		}
		else if (param == "-s" || param == "--store-reference")
		{
			m_arguments.append(" -s ").append(argv[++i]);
		}
		else if (param == "-v" || param == "--verbose")
		{
			m_arguments.append(" -v ").append(argv[++i]);
		}
	}
	m_arguments.append(" ");

	if(m_path.empty() && m_mode.empty())
	{
		m_status = Status::not_ready;

		return getStatus();
	}

	m_status = Status::ready;

	return getStatus();
}

void Parallel::calculateCount()
{
	std::string line{};
	while (std::getline(m_streams.getInputStream(), line))
	{
		if (line[0] == '@' && (line.substr(0,4) != "@SRR" || line.substr(0, 4) != "@ERR"))
			++m_count;
	}
	m_streams.getInputStream().clear();
	m_streams.getInputStream().seekg(std::ios_base::beg);
	m_repEvery = m_count / m_numberOfFilesToOutput;
}

bool Parallel::createFiles()
{
	std::string line{};
	std::size_t current{ 0 };

	while (std::getline(m_streams.getInputStream(), line))
	{
		if (line[0] == '@' && (line.substr(0, 4) != "@SRR" || line.substr(0, 4) != "@ERR"))
		{
			if (current % m_repEvery == 0 && m_index < m_numberOfFilesToOutput)
			{
				m_streams.getOutputStream().close();
				auto tempString{ m_output };
				m_streams.getOutputStream().open(tempString.insert(m_output.length(), std::to_string(++m_index)).append(".fastq"));
				if (!m_streams.getOutputStream())
				{
					fprintf(stderr, "There was a problem!\n"
						"Try using:\n"
						"-o <output directory> -i <input directory> -a <colord directory> -m {colord mode}\n");
					return false;
				}
				m_directories.emplace_back(std::filesystem::current_path().append(tempString).string());
			}
			++current;
		}
		m_streams.getOutputStream() << line << '\n';
	}
	m_streams.getOutputStream().flush();
	return true;
}

void Parallel::calculateFileSizes()
{
	for (const auto& path : m_directories)
	{
		m_sizesWithoutCompression.emplace_back(file_size(std::filesystem::path(path)));
		std::string tempOutput{ path.substr(0, path.find_last_of('.')).append("c.fastqcomp") };
		std::string temp{ " " + m_path };
		temp.append(m_mode).append(m_arguments).append(path).append(" " + tempOutput);

		std::system(temp.data());
		m_sizesWithCompression.emplace_back(file_size(std::filesystem::path(tempOutput)));
		printf("\n");
	}

	m_originalSizeWithoutCompression = std::filesystem::file_size(m_input);
	//const std::size_t originalSizeWithoutCompression{ std::filesystem::file_size(std::filesystem::current_path().append(input).string()) };
	const std::string tempOutput{ m_input.substr(0, m_input.find_last_of('.')).append("c.fastqcomp") };
	std::string temp{ " " +m_path };
	temp.append(m_mode).append(m_arguments).append(m_input).append(" " + tempOutput);
	std::system(temp.data());

	m_originalSizeWithCompression = file_size(std::filesystem::path(tempOutput));
	printf("\n");
	for (std::size_t i{ 0 }; i < m_index; ++i)
	{
		const auto ratio{ m_sizesWithoutCompression[i] / static_cast<long double> (m_sizesWithCompression[i]) };
		m_avgRatio += ratio;
		std::stringstream tempStream{};
		tempStream << std::setprecision(3) << std::fixed << "Size of file #" << i + 1 << ":\nw/o compression: " << m_sizesWithoutCompression[i] / static_cast<long double>(1024)
			<< "kbs\tw/ compression: " << m_sizesWithCompression[i] / static_cast<long double>(1024) << "kbs\tcompression ratio: " << ratio << '\n';
		m_streams.getLogsStream() << tempStream.view();
		std::cout << tempStream.view();
	}
	m_avgRatio /= m_index;
}

void Parallel::averageRatio()
{
	{
		std::stringstream sStream{};
		sStream << std::setprecision(3) << std::fixed << "\nAverage compression ratio: " << m_avgRatio << '\n';
		m_streams.getLogsStream() << sStream.view();
		std::cout << sStream.view();
	}
}

void Parallel::printFileSizes()
{
	const long double ratio{ m_originalSizeWithoutCompression / static_cast<long double>(m_originalSizeWithCompression) };
	std::stringstream sStream{};
	sStream << std::setprecision(3) << std::fixed << "Size of the original file w/o compression: " << m_originalSizeWithoutCompression / static_cast<long double>(1024)
		<< "kbs\tw/ compression: " << m_originalSizeWithCompression / static_cast<long double>(1024) << "kbs\tcompression ratio: " << ratio << "\n\n";
	std::cout << sStream.view();
	m_streams.getLogsStream() << sStream.view();
}

void Parallel::totalSequences()
{
	std::stringstream sStream{};
	sStream << std::setprecision(3) << std::fixed << "Total sequences: " << m_count << "\tDelta: " << m_ratio - m_avgRatio << "\n";
	std::cout << sStream.view();
	m_streams.getLogsStream() << sStream.view();
}