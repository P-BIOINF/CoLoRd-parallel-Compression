#ifndef STREAMS_H
#define STREAMS_H

#include <fstream>

class Streams
{
private:
    std::ifstream m_input{};
    std::ofstream m_output{};
    std::ofstream m_logs{};

public:
    Streams() = default;

    Streams(const std::string& input, const std::string& logs)
        : m_input{ input }, m_logs{ logs } {}

    bool setInput(const std::string& input)
    {
        if (m_input.is_open())
            m_input.close();
        m_input.open(input);
        return m_input.good();
    }

    bool setLogs(const std::string& logs)
    {
        if (m_logs.is_open())
            m_logs.close();
        m_logs.open(logs);
        return m_logs.good();
    }

    bool setOutput(const std::string& output)
    {
        if (m_output.is_open())
            m_output.close();
        m_output.open(output);
        return m_output.good();
    }

    std::ifstream& getInputStream()
    {
        return m_input;
    }

    std::ofstream& getOutputStream()
    {
        return m_output;
    }

    std::ofstream& getLogsStream()
    {
        return m_logs;
    }

    [[nodiscard]] bool good() const
    {
        return m_input.good() && m_output.good() && m_logs.good();
    }

};

#endif