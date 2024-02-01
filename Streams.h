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

    /**
     * \brief set the inputStream to the input string
     * \param input string path
     * \return m_input.good()
     */
    bool setInput(const std::string& input)
    {
        if (m_input.is_open())
            m_input.close();
        m_input.open(input);
        return m_input.good();
    }

    /**
     * \brief set the logStream to the logs string
     * \param logs string path 
     * \return m_logs.good()
     */
    bool setLogs(const std::string& logs)
    {
        if (m_logs.is_open())
            m_logs.close();
        m_logs.open(logs);
        return m_logs.good();
    }

    /**
     * \brief set the outputStream to the output
     * \param output string path
     * \return m_logs.good()
     */
    bool setOutput(const std::string& output)
    {
        if (m_output.is_open())
            m_output.close();
        m_output.open(output);
        return m_output.good();
    }

    /**
     * \brief returns the inputStream object
     * \return m_input
     */
    std::ifstream& getInputStream()
    {
        return m_input;
    }

    /**
     * \brief returns the outputStream object
     * \return m_output
     */
    std::ofstream& getOutputStream()
    {
        return m_output;
    }

    /**
     * \brief returns the logsStream object
     * \return m_logs
     */
    std::ofstream& getLogsStream()
    {
        return m_logs;
    }

    /**
     * \brief checks if all streams are set properly
     * \return m_input.good() && m_output.good() && m_logs.good();
     */
    [[nodiscard]] bool good() const
    {
        return m_input.good() && m_output.good() && m_logs.good();
    }

};

#endif  