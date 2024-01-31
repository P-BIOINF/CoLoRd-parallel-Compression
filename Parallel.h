#ifndef PARALLEL_H
#define PARALLEL_H

#include <utility>
#include <string>
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

public:
	Parallel() = default;

	Status parseArguments(const int argc, char** argv);


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
};

#endif
