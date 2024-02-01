#ifndef PARALLEL_H
#define PARALLEL_H

#include <utility>
#include <string>
#include <vector>
#include "Streams.h"

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
	std::string m_path{};
	std::string m_mode{};
	std::string m_arguments{};
	std::string m_input{};
	std::string m_output{};
	std::uint32_t m_numberOfFilesToOutput{};
	Status m_status{ Status::not_ready };
	Streams m_streams{};
	int m_count{ 0 };
	int m_repEvery{ 0 };
	std::vector<std::string> m_directories{};
	long double m_avgRatio{};
	std::size_t m_originalSizeWithCompression{};
	std::size_t m_index{ 0 };
	std::vector<std::size_t> m_sizesWithoutCompression{};
	std::vector<std::size_t> m_sizesWithCompression{};

public:
	Parallel() = default;

	Status parseArguments(const int argc, char** argv);

	int calculateCount();

	bool createFiles();

	const std::size_t jakasnazwa(std::ofstream& logStream);

	void AverageRatio(std::ofstream& logStream);

	auto fileSizes(std::ofstream& logStream, const std::size_t& originalSizeWithoutCompression);

	void totalSequences(std::ofstream& logStream, auto& ratio);

	[[nodiscard]] Status getStatus() const
	{
		return m_status;
	}

	[[nodiscard]] std::string_view getPath() const
	{
		return m_path;
	}

	[[nodiscard]] std::string_view getMode() const
	{
		return m_mode;
	}

	[[nodiscard]] std::ifstream& getInputStream()
	{
		return m_streams.getInputStream();
	}

	[[nodiscard]] std::ofstream& getOutputStream()
	{
		return m_streams.getOutputStream();
	}

	[[nodiscard]] std::ofstream& getLogsStream()
	{
		return m_streams.getLogsStream();
	}

	[[nodiscard]] int& getrepEvery()
	{
		return m_repEvery;
	}
};

#endif