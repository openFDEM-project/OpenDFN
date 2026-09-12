/******************************************************************************
 * \file      parser.cpp
 * \brief     Top-level .dfn reader and geometry-command dispatcher.
 *
 * \details   Provides DeckReader::readDeck (the same entry the full engine's
 *            RunModel::readRunFile calls) plus a lightweight parseGeneral.
 *            Only odfn.geometry.* commands affect the model. Other odfn.*
 *            commands are ignored for compatibility with legacy decks.
 ******************************************************************************/

#include <cstring>
#include <string>
#include "common/opendfn_common.h"
#include "common/opendfn_macros.h"
#include "common/opendfn_message.h"
#include "iohelper/opendfn_io_module.h"
#include "parser/parser_module.h"

using namespace ns_common;

namespace ns_io {

/** Consume the remainder of the current physical line. */
static void skipRestOfLine(FILE* fp) {
    char rest[1024];
    if (fgets(rest, sizeof(rest), fp) == nullptr)
        return;
}

/**
 * \brief Lightweight stand-in for the full-engine parseGeneral.
 *
 * Every non-geometry control/config keyword is consumed line-by-line. The geometry
 * keywords are handled by the caller (readDeck) via parseGeometry, so
 * this function declines them (returns 0) and consumes everything else.
 * Return contract used by readDeck: 1 = consumed here, 0 = not ours.
 */
int DeckParser::parseGeneral(OpenDFNHandle context, char** argv, char* keyword) {
    General general = &context->general;
    (void)argv;

    // geometry is the caller's job -> decline so it dispatches parseGeometry
    if (isSamePrefix(keyword, "OpenDFN.geometry.", 17) || isSamePrefix(keyword, "odfn.geometry.", 14))
        return 0;

    // any other OpenDFN./odfn. command is a non-geometry directive: skip it
    if (isSamePrefix(keyword, "OpenDFN.", 8) || isSamePrefix(keyword, "odfn.", 5)) {
        skipRestOfLine(general->inputfile);
        return 1;
    }
    return 0;  // not an OpenDFN keyword (comment / blank) -> handled by caller
}

/**
 * \brief Open \p filename and drive the keyword read/dispatch loop.
 *
 * Same shape as the full engine: read one keyword, hand it to parseGeneral,
 * and if that declines (return 0) route it by prefix. odfn.geometry.* does the
 * real work; skipped module prefixes and ''' / # comment lines are consumed;
 * a genuinely unknown keyword is reported and ends the parse.
 */
long DeckReader::readDeck(char* filename, OpenDFNHandle context, char** argv) {
    General general = &context->general;
    char    keyword[300];

    if (general->inputfile == FileNull) {
        general->inputfile      = fopen(filename, "r");
        general->FileID         = 0;
        general->FilePointer[0] = general->inputfile;
    }
    if (general->inputfile == FileNull) {
        OpenDFNMessage::RuntimeInfo("Error: fails to open input file!");
        return 0;
    }

    getCharValue(general->inputfile, keyword);
    while (!feof(general->inputfile)) {
        if (isSamePrefix(keyword, "OpenDFN.geometry.", 17) || isSamePrefix(keyword, "odfn.geometry.", 14)) {
            DeckParser::parseGeometry(context, argv, keyword);
        } else if (DeckParser::parseGeneral(context, argv, keyword) > 0) {
            // non-geometry control/config keyword: consumed and skipped
        } else if (isSamePrefix(keyword, "'''", 3)) {
            int guard = 0;  // block comment: read until the closing '''
            do {
                getCharValue(general->inputfile, keyword);
            } while (!feof(general->inputfile) && !isSamePrefix(keyword, "'''", 3) && ++guard < 100000);
        } else if (keyword[0] == '#' || keyword[0] == '\0') {
            skipRestOfLine(general->inputfile);
        } else {
            OpenDFNMessage::RuntimeKeywordError(keyword, get_line_current_position(general->inputfile));
            break;
        }
        getCharValue(general->inputfile, keyword);
    }
    return 0;
}

}  // namespace ns_io
