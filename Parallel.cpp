#include "Parallel.h"
#include <string>
#include <stdio.h>
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



	return getStatus();
}

int Parallel::CalculateCount()
{
	std::string line{};
	while (std::getline(m_streams.getInputStream(), line))
	{
		if (line[0] == '@' && line.find("@ERR") != std::string::npos)
			++m_count;
	}
	m_streams.getInputStream().clear();
	m_streams.getInputStream().seekg(std::ios_base::beg);
	m_repEvery = m_count / m_numberOfFilesToOutput;
	return m_count;
}

bool Parallel::createFiles()
{
	std::string line{};
	std::size_t index{ 0 };
	std::size_t current{ 0 };

	while (std::getline(m_streams.getInputStream(), line))
	{
		if (line[0] == '@' && line.find("@ERR") != std::string::npos)
		{
			if (current % m_repEvery == 0 && index < m_numberOfFilesToOutput)
			{
				m_streams.getOutputStream().close();
				auto tempString{ m_output };
				m_streams.getOutputStream().open(tempString.insert(m_output.length(), std::to_string(++index)).append(".fastq"));
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