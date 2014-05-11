#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>
#include <iostream>
#include <cassert>

// macros for printing messages of various types
#define ERRORM(x) std::cerr << "ERROR: " << x << std::endl
#define WARNM(x) std::cerr << "WARNING: " << x << std::endl
#define INFOM(x) std::cerr << "INFO: " << x << std::endl
#define DBGM(x) std::cerr << x << std::endl

#endif
