#ifndef OPENDFN_DECK_PARSER_H
#define OPENDFN_DECK_PARSER_H

#include "core/opendfn_context.h"

namespace ns_io {

class DeckParser {
public:
    static int parseGeneral(OpenDFNHandle context, char** argv, char* keyword);
    static void parseGeometry(OpenDFNHandle context, char** argv, char* keyword);
};

}  // namespace ns_io

#endif
