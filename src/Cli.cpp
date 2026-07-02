#include "Cli.h"

#include "FileResolver.h"
#include "Interpreter.h"
#include "Lexer.h"

#include <fstream>
#include <iostream>
#include <sstream>

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
    if (command != "run" && command != "tokens")
    {
        printUsage();
        return 1;
    }

    const std::string target = argc == 3 ? argv[2] : "main";
    const auto path = FileResolver::resolve(target);

    if (command == "tokens")
    {
        return printTokens(path) ? 0 : 1;
    }

    Interpreter interpreter;
    return interpreter.runFile(path) ? 0 : 1;
}

void Cli::printUsage()
{
    std::cerr << "MopCode Error: unknown command\n";
    std::cerr << "Usage:\n";
    std::cerr << "  mopc run\n";
    std::cerr << "  mopc run <file>\n";
    std::cerr << "  mopc tokens\n";
    std::cerr << "  mopc tokens <file>\n";
}

bool Cli::printTokens(const std::string& path)
{
    std::ifstream file(path);
    if (!file)
    {
        std::cerr << "MopCode Error: file not found: " << path << '\n';
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    Lexer lexer(buffer.str());
    const auto tokens = lexer.scanTokens();
    if (lexer.hadError())
    {
        return false;
    }

    for (const auto& token : tokens)
    {
        std::cout << tokenTypeName(token.type);
        if (!token.lexeme.empty())
        {
            std::cout << ' ' << token.lexeme;
        }
        std::cout << '\n';
    }

    return true;
}
}
