#include "utils.h"

void replaceAll(std::string& str, std::string const& from, std::string const& to)
{
    if (from.empty()) return;

    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}
