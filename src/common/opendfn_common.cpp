#include "common/opendfn_common.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>

#include "common/opendfn_message.h"

namespace ns_common {
namespace {

std::string readToken(FILE* inputFile) {
    int value = 0;
    do {
        value = std::fgetc(inputFile);
    } while (value != EOF && std::isspace(static_cast<unsigned char>(value)));

    if (value == EOF)
        return {};

    std::string token;
    const bool quoted = value == '\'' || value == '"';
    const int quote = quoted ? value : 0;
    if (!quoted)
        token.push_back(static_cast<char>(value));

    while ((value = std::fgetc(inputFile)) != EOF) {
        if ((quoted && value == quote) || (!quoted && std::isspace(static_cast<unsigned char>(value))))
            break;
        token.push_back(static_cast<char>(value));
    }
    return token;
}

std::string readLogicalLine(FILE* inputFile) {
    std::string result;
    char buffer[MAX_KEYWORD_LENGTH] = {};
    while (std::fgets(buffer, sizeof(buffer), inputFile) != nullptr) {
        std::string part(buffer);
        part.erase(std::remove(part.begin(), part.end(), '\r'), part.end());
        part.erase(std::remove(part.begin(), part.end(), '\n'), part.end());
        const std::size_t continuation = part.find('&');
        if (continuation != std::string::npos) {
            part.erase(continuation, 1);
            result += part;
            result.push_back(' ');
            continue;
        }
        result += part;
        break;
    }
    return result;
}

}  // namespace

void parseStringinQuotation(FILE* inputFile, char* value) {
    const std::string token = readToken(inputFile);
    std::snprintf(value, MAXARGC, "%s", token.c_str());
}

void parse_string(FILE* inputFile, char* value) {
    parseStringinQuotation(inputFile, value);
}

void parseStringinLine(FILE* inputFile, Keywords& keywords) {
    if (keywords.count > 0)
        keywords.clean();
    keywords.count = 0;
    keywords.findbracket = false;
    std::fill(std::begin(keywords.keywords), std::end(keywords.keywords), nullptr);

    std::string line = readLogicalLine(inputFile);
    std::snprintf(keywords.copy, sizeof(keywords.copy), "%s", line.c_str());
    keywords.findbracket = line.find('(') != std::string::npos || line.find('[') != std::string::npos;

    for (char& value : line) {
        if (value == ',' || value == '"' || value == '\'' || value == '(' || value == ')' || value == '[' || value == ']')
            value = ' ';
    }

    std::istringstream stream(line);
    std::string token;
    while (stream >> token) {
        if (!token.empty() && token.front() == '#')
            break;
        if (keywords.count >= keywords.MAXNUM) {
            OpenDFNMessage::RuntimeStringInt("Error: maximum keyword count is %d.\n", keywords.MAXNUM);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
        keywords.keywords[keywords.count] = static_cast<char*>(std::malloc(MAX_KEYWORD_LENGTH));
        if (keywords.keywords[keywords.count] == nullptr)
            throw std::bad_alloc();
        std::snprintf(keywords.keywords[keywords.count], MAX_KEYWORD_LENGTH, "%s", token.c_str());
        ++keywords.count;
    }
}

long get_line_current_position(FILE* inputFile) {
    const long position = std::ftell(inputFile);
    if (position < 0)
        return 0;
    std::rewind(inputFile);
    long line = 1;
    for (long offset = 0; offset < position; ++offset) {
        if (std::fgetc(inputFile) == '\n')
            ++line;
    }
    std::fseek(inputFile, position, SEEK_SET);
    return line;
}

void removeCharsBeforeSubstring(char* value, const char* substring) {
    char* match = std::strstr(value, substring);
    if (match != nullptr)
        std::memmove(value, match, std::strlen(match) + 1);
}

}  // namespace ns_common
