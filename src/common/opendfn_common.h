#ifndef OPENDFN_COMMON_H
#define OPENDFN_COMMON_H

#include <cstdio>

#include "common/opendfn_enum.h"
#include "common/opendfn_error.h"
#include "common/opendfn_math.h"
#include "common/opendfn_struct.h"
#include "common/opendfn_vector2.h"
#include "common/opendfn_vector2Utils.h"

namespace ns_common {

void parseStringinQuotation(FILE* inputFile, char* value);
void parse_string(FILE* inputFile, char* value);
void parseStringinLine(FILE* inputFile, Keywords& keywords);
long get_line_current_position(FILE* inputFile);
void removeCharsBeforeSubstring(char* value, const char* substring);

}  // namespace ns_common

#endif
