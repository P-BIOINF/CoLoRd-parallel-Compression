#ifndef PARALLEL_H
#define PARALLEL_H

#include <utility>
#include <string>
#include <vector>
#include "Streams.h"
#include <filesystem>

enum class Status
{
	ready,
	not_ready,
	failed,
	max_status,
};

class Parallel
{
	Streams m_streams{};
	std::string m_mode{};
	std::int64_t m_count{10};
	std::string m_arguments{};
	std::int64_t m_index{ 0 };
	std::int64_t m_threads{ -1 };
	std::int64_t m_repEvery{ 0 };
	std::vector<double> m_times{};
	std::filesystem::path m_path{};
	std::int64_t m_countFiles{ 0 };
	std::filesystem::path m_input{};
	std::filesystem::path m_output{};
	std::atomic<int> m_pathIndex{ 0 };
	std::filesystem::path m_extension{};
	Status m_status{ Status::not_ready };
	std::int64_t m_maxNumberOfFilesToOutput{};
	std::vector<std::filesystem::path> m_directories{};

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
	 * \brief divides the sequences into max {count} files 
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
	 * \brief controls using colord's compression with threads
	 */
	void handleCompression();

	/**
	 * \brief runs colord's compression
	 */
	void systemCompression(const int index)const;

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