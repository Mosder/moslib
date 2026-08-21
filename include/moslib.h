// moslib.h
// Main header file of the library
// Contains all other header files

// By default the library doesn't force the use of a prefix
// You can force the use of the "mos" prefix by defining (before including any header):
//
//      #define MOS_FORCE_PREFIXES
//
// This way you can, for example, avoid collisions

#include "moslib/ds/array.h"
#include "moslib/safe.h"
#include "moslib/string.h"
#include "moslib/tester.h"
