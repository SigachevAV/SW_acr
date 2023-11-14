#pragma once
#include <string>
#include <vector>
#include <stdio.h>

// splits string into multiple substrings by a separator.
// example: split("a:b", ":") == {"a", "b"}
inline static std::vector<std::string> split(const std::string& str, const std::string& separator)
{
    std::vector<std::string> result;
    std::string token;
    size_t pos = 0, prev = 0;
    while(pos < str.length() && pos != std::string::npos)
    {
        pos = str.find(separator, prev);
        token = str.substr(prev, pos - prev);
        result.push_back(token);
        prev = pos + separator.length();
    }
    return result;
}

// concatenates strings by a separator.
// example: join({"a", "b"}, ":") == "a:b"
inline static std::string join(const std::vector<std::string>& str, const std::string& separator, size_t start = 0, size_t end = -1)
{
    if (str.empty()) {
        return "";
    }
    std::string result;
    end = end == -1 ? str.size() : end;
    for (size_t i = start; i < end - 1; i++) {
        result += (str[i] + separator);
    }
    return result + str[end - 1];
}

// converts integer to string
inline static std::string toStr(int i)
{
    char buf[1024] = {0};
    snprintf(buf, 1024, "%d", i);
    return std::string(buf);
}

// converts string to integer
inline static int toInt(std::string str)
{
    return atoi(str.c_str());
}
