#ifndef PARALLEL_H
#define PARALLEL_H

#include <utility>
#include <string>
#include <vector>
#include "Streams.h"
#include <filesystem>

const std::string g_winTime{ "C:\\Users\\kacpe\\Desktop\\WinTime64.exe " };

enum class Status
{
	ready,
	not_ready,
	failed,

	max_status,
};

class Parallel
{
private:
	std::filesystem::path m_path{};
	std::string m_mode{};
	std::string m_arguments{};
	std::filesystem::path m_input{};
	std::filesystem::path m_output{};
	std::uint32_t m_numberOfFilesToOutput{};
	Status m_status{ Status::not_ready };
	Streams m_streams{};
	std::size_t m_count{ 0 };
	std::size_t m_repEvery{ 0 };
	std::vector<std::filesystem::path> m_directories{};
	long double m_avgRatio{};
	std::size_t m_index{ 0 };
	std::vector<std::size_t> m_sizesWithoutCompression{};
	std::vector<std::size_t> m_sizesWithCompression{};
	std::size_t m_originalSizeWithoutCompression{};
	long double m_ratio{1};
	std::vector<double> m_times{};
	std::filesystem::path m_extension{};

public:
	Parallel() = default;

	/**
	 * \brief parses command arguments
	 * \param argc self-explanatory
	 * \param argv self-explanatory
	 * \return returns status. Status::ready if the program is ready to use
	 */
	Status parseArguments(const int argc, char** argv);

	/**
	 * \brief calculates the amount of sequences
	 * requires parseArguments(const int argc, char** argv)
	 */
	void calculateCount();

	/**
	 * \brief divides the sequences into {count} files (can round one up/down)
	 * \return true if there was not any problem with creating the files
	 * requires parseArguments(const int argc, char** argv), calculateCount()
	 */
	bool createFiles();

	/**
	 * \brief runs colord's compression
	 * requires parseArguments(const int argc, char** argv), calculateCount(), createFiles() to be run before
	 */
	void compress();

	/**
	 * \brief prints avg compression ratio
	 * requires compress() to be run before
	 */
	void printAvgRatio();

	/**
	 * \brief prints the size etc. of the original file (not divided)
	 * requires compress() to be run before
	 */
	void printFileSizes();

	/**
	 * \brief prints the amount of sequences and the delta of the compression ratio (compressionRatioOfTheOriginalFile - averageCompressionRatioOfDividedFiles)
	 * requires compress() to be run before
	 */
	void totalSequences();

	/**
	 * \brief returns current status
	 * \return m_status
	 */
	[[nodiscard]] Status getStatus() const
	{
		return m_status;
	}

	/**
	 * \brief returns path to the colord.exe
	 * \return m_path
	 */
	[[nodiscard]] const std::filesystem::path& getPath() const
	{
		return m_path;
	}

	/**
	 * \brief returns mode in which colord should be run
	 * \return m_mode
	 */
	[[nodiscard]] std::string_view getMode() const
	{
		return m_mode;
	}

	/**
	 * \brief returns the inputStream object
	 * the stream to the original input
	 * \return m_streams.getInputStream()
	 */
	[[nodiscard]] std::ifstream& getInputStream()
	{
		return m_streams.getInputStream();
	}

	/**
	 * \brief checks if all streams are set properly
	 * \return m_streams.good();
	 */
	[[nodiscard]] bool good() const
	{
		return m_streams.good();
	}

	/**
	 * \brief returns the outputStream object
	 * the stream to output the files
	 * \return m_stream.getOutputStream()
	 */
	[[nodiscard]] std::ofstream& getOutputStream()
	{
		return m_streams.getOutputStream();
	}

	/**
	 * \brief returns the logStream object
	 * the stream for outputting logs
	 * \return m_stream.getLogsStream()
	 */
	[[nodiscard]] std::ofstream& getLogsStream()
	{
		return m_streams.getLogsStream();
	}

	/**
	 * \brief returns the number of sequences per divided file
	 * \return m_repEvery
	 */
	[[nodiscard]] std::size_t getRepEvery() const
	{
		return m_repEvery;
	}

	/**
	 * \brief returns the path to the input file
	 * \return m_input
	 */
	[[nodiscard]] const std::filesystem::path& getInput() const
	{
		return m_input;
	}

	/**
	 * \brief returns the output path
	 * \return m_output
	 */
	[[nodiscard]] const std::filesystem::path& getOutput() const
	{
		return m_output;
	}
};

#endif