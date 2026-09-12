/******************************************************************************
 * \file      main.cpp
 * \brief     Program entry point for the standalone OpenDFN jset / joint / DFN
 *            geometry pre-processor.
 *
 * \details   Resolves a .dfn input deck, parses odfn.geometry.* commands and
 *            writes the generated Gmsh mesh beside the deck.
 *
 *            Usage: opendfn -in <path/to/deck.dfn>
 ******************************************************************************/

#include <cstdio>
#include "common/opendfn_manager.h"
#include "core/opendfn_context.h"
#include "iohelper/opendfn_io_module.h"

using namespace ns_common;
using namespace ns_io;

/**
 * \brief Program entry point for OpenDFN.
 *
 * \param argc argument count.
 * \param argv expects: -in <deck.dfn> or a bare deck path.
 * \return OPENDFN_EXIT_SUCCESS on normal completion.
 */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        OpenDFNManager::printCopyrightPage();
        OpenDFNManager::useHelp(argv[0]);
        return OPENDFN_EXIT_SUCCESS;
    }

    OpenDFNManager::printCopyrightPage();

    OpenDFNContext contextData;
    OpenDFNHandle context = &contextData;
    General general = &context->general;

    char fullPath[300];
    char baseName[300];
    char deckName[300];
    if (!OpenDFNManager::configureInput(argc, argv, fullPath, baseName, deckName, general))
        return OPENDFN_EXIT_FAILURE;

    general->logfile = fopen("opendfn.log", "w");

    general->inputfile = FileNull;
    DeckReader::readDeck(deckName, context, argv);

    if (general->logfile != FileNull)
        fclose(general->logfile);

    printf("Done. Geometry deck '%s' processed.\n", deckName);
    return OPENDFN_EXIT_SUCCESS;
}
