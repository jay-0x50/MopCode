#pragma once

#include <string>

namespace mopcode
{
class Cli
{
public:
    static int run(int argc, char** argv);

private:
    static void printUsage();
    static bool printTokens(const std::string& path);
};
}
