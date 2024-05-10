#ifndef STREAMS_H
#define STREAMS_H

#include <fstream>

class Streams
{
private:
    std::ifstream m_input{};
    std::ofstream m_output{};

public:
    Streams() = default;

    Streams(const std::string& input)
        : m_input{ input } {}

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
     * \brief checks if all streams are set properly
     * \return m_input.good() && m_output.good();
     */
    [[nodiscard]] bool good() const
    {
        return m_input.good() && m_output.good() ;
    }

};

#endif  