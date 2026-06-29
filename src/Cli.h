#pragma once

namespace mopcode
{
class Cli
{
public:
    static int run(int argc, char** argv);

private:
    static void printUsage();
};
}
