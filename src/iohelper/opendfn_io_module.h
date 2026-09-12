#ifndef OPENDFN_DECK_READER_H
#define OPENDFN_DECK_READER_H

#include "core/opendfn_context.h"

namespace ns_io {

class DeckReader {
public:
    static long readDeck(char* filename, OpenDFNHandle context, char** argv);
};

}  // namespace ns_io

#endif
