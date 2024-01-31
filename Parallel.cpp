#include "Parallel.h"
#include <string>


Status Parallel::parseArguments(const int argc, char** argv)
{
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
				numberOfFilesToOutput = std::stoul(argv[++i]);
			}
			catch (...)
			{
				m_status = Status::failed;

				return getStatus();
			}
		}
		else if (param == "-a")
		{
			path = argv[++i];
		}
		else if (param == "-m")
		{
			mode.append(" ").append(argv[++i]);
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

	if()
	{
		
	}

	return getStatus();
}

