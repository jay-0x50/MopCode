#include "Cli.h"

#include "FileResolver.h"
#include "Interpreter.h"

#include <iostream>

namespace mopcode
{
int Cli::run(int argc, char** argv)
{
    if (argc < 2 || argc > 3)
    {
        printUsage();
        return 1;
    }

    const std::string command = argv[1];
    if (command != "run")
    {
        printUsage();
        return 1;
    }

    const std::string target = argc == 3 ? argv[2] : "main";
    const auto path = FileResolver::resolve(target);

    Interpreter interpreter;
    return interpreter.runFile(path) ? 0 : 1;
}

void Cli::printUsage()
{
    std::cerr << "MopCode Error: unknown command\n";
    std::cerr << "Usage:\n";
    std::cerr << "  mopc run\n";
    std::cerr << "  mopc run <file>\n";
}
}
