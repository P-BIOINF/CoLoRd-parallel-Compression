#ifndef PARALLEL_H
#define PARALLEL_H

#include "Streams.h"
#include <atomic>
#include <chrono>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <filesystem>

enum class Status
{
	ready,
	not_ready,
	failed,
	max_status,
};

/**
* \brief calculates and prints the time difference
*/
void displayTime(std::string message, const std::chrono::high_resolution_clock::time_point& start, const std::chrono::high_resolution_clock::time_point& end);

class Parallel
{
	Streams m_streams{};
	std::string m_mode{};
	std::int64_t m_count{10000};
	std::string m_arguments{};
	std::int64_t m_index{ 0 };
	std::int64_t m_threads{ -1 };
	std::filesystem::path m_path{};
	std::filesystem::path m_input{};
	std::filesystem::path m_output{};
	std::atomic<int> m_pathIndex{ 0 };
	std::filesystem::path m_extension{};
	Status m_status{ Status::not_ready };
	std::int64_t m_maxNumberOfFilesToOutput{};
	std::vector<std::filesystem::path> m_directories{};
	std::chrono::high_resolution_clock::time_point m_new_files_end{};
	std::chrono::high_resolution_clock::time_point m_new_files_start{};
	std::chrono::high_resolution_clock::time_point m_compression_end{};
	std::chrono::high_resolution_clock::time_point m_compression_start{};

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
