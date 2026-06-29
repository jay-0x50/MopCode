#pragma once

#include <string>

namespace mopcode
{
class FileResolver
{
public:
    static std::string resolve(const std::string& target);
};
}
