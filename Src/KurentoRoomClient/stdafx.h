#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
// Windows SDK defines min and max macros, messing up with our std::min and std::max usage
#define NOMINMAX
#endif

#include <QtGui>
#include <QtWidgets>
#include <QtNetwork>

