#include "Interpreter.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>

namespace mopcode
{
bool Interpreter::runFile(const std::string& path) const
{
    std::ifstream file(path);
    if (!file)
    {
        std::cerr << "MopCode Error: file not found: " << path << '\n';
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    return executeSource(buffer.str());
}

bool Interpreter::executeSource(const std::string& source) const
{
    std::string output;
    if (!tryReadPrintString(source, output))
    {
        std::cerr << "MopCode Error: invalid Print syntax\n";
        return false;
    }

    std::cout << output << '\n';
    return true;
}

bool Interpreter::tryReadPrintString(const std::string& source, std::string& output)
{
    const std::string callStart = "Print";
    const auto printPos = source.find(callStart);
    if (printPos == std::string::npos)
    {
        return false;
    }

    std::size_t index = printPos + callStart.size();
    while (index < source.size() && std::isspace(static_cast<unsigned char>(source[index])))
    {
        ++index;
    }

    if (index >= source.size() || source[index] != '(')
    {
        return false;
    }
    ++index;

    while (index < source.size() && std::isspace(static_cast<unsigned char>(source[index])))
    {
        ++index;
    }

    if (index >= source.size() || source[index] != '"')
    {
        return false;
    }
    ++index;

    std::string value;
    bool escaping = false;
    for (; index < source.size(); ++index)
    {
        const char current = source[index];
        if (escaping)
        {
            switch (current)
            {
            case 'n':
                value.push_back('\n');
                break;
            case 't':
                value.push_back('\t');
                break;
            case '"':
                value.push_back('"');
                break;
            case '\\':
                value.push_back('\\');
                break;
            default:
                value.push_back(current);
                break;
            }
            escaping = false;
            continue;
        }

        if (current == '\\')
        {
            escaping = true;
            continue;
        }

        if (current == '"')
        {
            break;
        }

        value.push_back(current);
    }

    if (index >= source.size() || source[index] != '"')
    {
        return false;
    }
    ++index;

    while (index < source.size() && std::isspace(static_cast<unsigned char>(source[index])))
    {
        ++index;
    }

    if (index >= source.size() || source[index] != ')')
    {
        return false;
    }

    output = value;
    return true;
}
}
