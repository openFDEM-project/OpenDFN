#ifndef OPENDFN_MANAGER_H
#define OPENDFN_MANAGER_H

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <string>

#include "common/opendfn_config.h"
#include "common/opendfn_general.h"
#include "common/opendfn_message.h"

namespace ns_common {

class OpenDFNManager {
public:
    static void useHelp(const char* program) {
        std::fprintf(stdout, "Usage: %s [-in] <deck.dfn>\n", program);
        std::fprintf(stdout, "Generates a two-dimensional DFN geometry and Gmsh mesh.\n");
        std::fprintf(stdout, "Set OPENDFN_GMSH_GUI=1 to open the optional Gmsh window.\n");
    }

    static bool configureInput(int argc,
                               char** argv,
                               char* inputFileFullPath,
                               char* inputBaseName,
                               char* inputFileName,
                               General general) {
        const char* argument = nullptr;
        if (argc == 2) {
            argument = argv[1];
        } else if (argc == 3 && isSameString(argv[1], "-in")) {
            argument = argv[2];
        } else {
            useHelp(argv[0]);
            return false;
        }

        std::error_code error;
        std::filesystem::path deckPath = std::filesystem::absolute(argument, error);
        if (error || !std::filesystem::is_regular_file(deckPath)) {
            OpenDFNMessage::RuntimebackStringString("Error: input deck does not exist: ", argument);
            return false;
        }

        std::string extension = deckPath.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(),
                       [](unsigned char value) { return static_cast<char>(std::tolower(value)); });
        if (extension != ".dfn") {
            OpenDFNMessage::RuntimebackStringString("Error: expected a .dfn input deck, got ", extension.c_str());
            return false;
        }

        const std::string fullPath = deckPath.string();
        const std::string baseName = deckPath.stem().string();
        const std::string fileName = deckPath.filename().string();
        std::snprintf(inputFileFullPath, 300, "%s", fullPath.c_str());
        std::snprintf(inputBaseName, 300, "%s", baseName.c_str());
        std::snprintf(inputFileName, 300, "%s", fileName.c_str());
        std::snprintf(general->InputFileName, sizeof(general->InputFileName), "%s", baseName.c_str());

        std::filesystem::current_path(deckPath.parent_path(), error);
        if (error) {
            OpenDFNMessage::RuntimebackStringString("Error: cannot enter deck directory: ",
                                                    deckPath.parent_path().string().c_str());
            return false;
        }
        return true;
    }

    static void printVersion() {
        std::fprintf(stdout, "%s\n%s\n%s\n", PRG_VERSION, OPENDFN_COPYRIGHT, OPENDFN_WEBSITE);
    }

    static void printCopyrightPage() {
        OpenDFNMessage::Runtime_message(PRG_HEADER);
        OpenDFNMessage::Runtime_message("OpenDFN: two-dimensional discrete fracture network geometry generator");
        OpenDFNMessage::Runtime_message("Copyright (C) 2017-2026 Xiaofeng Li and OpenDFN contributors\n");
    }
};

}  // namespace ns_common

#endif
