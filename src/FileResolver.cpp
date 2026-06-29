#include "FileResolver.h"

namespace mopcode
{
std::string FileResolver::resolve(const std::string& target)
{
    std::string path = target.empty() ? "main" : target;

    const auto lastSlash = path.find_last_of("/\\");
    const auto lastDot = path.find_last_of('.');
    const bool hasExtension = lastDot != std::string::npos
        && (lastSlash == std::string::npos || lastDot > lastSlash);

    if (!hasExtension)
    {
        path.append(".mopc");
    }

    return path;
}
}
