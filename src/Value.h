#pragma once

#include <sstream>
#include <string>
#include <utility>

namespace mopcode
{
class Value
{
public:
    enum class Type
    {
        Void,
        Number,
        String,
        Bool
    };

    Value()
        : type_(Type::Void), number_(0.0), bool_(false)
    {
    }

    explicit Value(double number)
        : type_(Type::Number), number_(number), bool_(false)
    {
    }

    explicit Value(std::string text)
        : type_(Type::String), number_(0.0), string_(std::move(text)), bool_(false)
    {
    }

    explicit Value(bool value)
        : type_(Type::Bool), number_(0.0), bool_(value)
    {
    }

    Type type() const
    {
        return type_;
    }

    double asNumber() const
    {
        return number_;
    }

    const std::string& asString() const
    {
        return string_;
    }

    bool asBool() const
    {
        return bool_;
    }

    bool isTruthy() const
    {
        switch (type_)
        {
        case Type::Void:
            return false;
        case Type::Number:
            return number_ != 0.0;
        case Type::String:
            return !string_.empty();
        case Type::Bool:
            return bool_;
        }

        return false;
    }

    std::string toString() const
    {
        switch (type_)
        {
        case Type::Void:
            return "void";
        case Type::Number:
        {
            std::ostringstream stream;
            stream << asNumber();
            return stream.str();
        }
        case Type::String:
            return asString();
        case Type::Bool:
            return asBool() ? "true" : "false";
        }

        return "";
    }

private:
    Type type_;
    double number_;
    std::string string_;
    bool bool_;
};
}
