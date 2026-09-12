#ifndef OPENDFN_GENERAL_H
#define OPENDFN_GENERAL_H

#include <cstdio>

struct General_structure;
using General = General_structure*;

struct General_structure {
    char InputFileName[300] = {};
    FILE* inputfile = nullptr;
    FILE* logfile = nullptr;
    FILE* FilePointer[16] = {};
    short FileID = 0;
    bool debug = true;
};

#endif
