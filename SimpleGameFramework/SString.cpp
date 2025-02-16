#include "SString.h"

sgf::String sgf::SString::StringtoUpper(const String& src)
{
    String result;
    for (char c : src) {
        result += std::toupper(c);
    }
    return result;
}

sgf::String sgf::SString::StringReplace(const String& resource_str, const String& sub_str, const String& new_str)
{
    String dst_str = resource_str;
    String::size_type pos = 0;
    while ((pos = dst_str.find(sub_str)) != std::string::npos)
    {
        dst_str.replace(pos, sub_str.length(), new_str);
    }
    return dst_str;
}

sgf::String sgf::SString::StringRemoveFileExtension(const String& filename)
{
    size_t dotPos = filename.find_last_of(".");
    if (dotPos != String::npos && dotPos > 0) {
        return filename.substr(0, dotPos);
    }
    else {
        return filename;
    }
}

sgf::String sgf::SString::StringGetCurrentPath(const String& filename)
{
    size_t dotPos = filename.find_last_of("/");
    if (dotPos != String::npos && dotPos > 0) {
        return filename.substr(0, dotPos);
    }
    else {
        return filename;
    }
}

sgf::String sgf::SString::StringGetCurrentPathWithoutExtension(const String & filename)
{
    size_t dotPos = filename.find_last_of("/");
    if (dotPos != String::npos && dotPos > 0) {
        return StringRemoveFileExtension(filename.substr(dotPos + 1));
    }
    else {
        return filename;
    }
}

bool sgf::SString::IsCharInString(char cha, const String& str)
{
    for (auto& x : str)
    {
        if (cha == x)
            return true;
    }
    return false;
}
