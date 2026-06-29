#pragma once

#include <string>

namespace mopcode
{
class Interpreter
{
public:
    bool runFile(const std::string& path) const;

private:
    bool executeSource(const std::string& source) const;
    static bool tryReadPrintString(const std::string& source, std::string& output);
};
}
